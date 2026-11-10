import json
from pathlib import Path
import re
import shutil
from typing import List, NamedTuple, Optional

from reference_tools import STRATEGIES, reference_fingerprint

from .catalog import TESTS_BY_NAME, TestDefinition
from .processes import (
    BuildError, CheckError, ProcessError, TestCommand, remove_if_exists,
    run_process, write_process_log,
)
from .results import TestResult, read_ctest_results, read_peak_memory, read_reference_result


TEST_TIMEOUT_SECONDS = 30
PROFILE_TIMEOUT_SECONDS = 300


class RunOptions(NamedTuple):
    test_suite: Path
    build_directory: Path
    selection: Optional[TestDefinition] = None


class ReferenceBinary(NamedTuple):
    strategy: str
    path: Path
    error: str = ''


class CheckRunner:
    def __init__(self, options: RunOptions):
        self.options = options
        self.build_directory = options.build_directory
        self.references_directory = options.test_suite / 'references'
        self.log_path = self.build_directory / 'check.log'

    def run(self) -> List[TestResult]:
        self.log_path.write_text('')
        self.build_tests()
        results = self.run_student_tests()
        self.run_references(results)
        self.profile_memory(results)
        return results

    def build_tests(self) -> None:
        print('Building tests...', flush=True)
        path = self.build_directory / 'check-build.log'
        try:
            process = run_process(['cmake', '--build', str(self.build_directory)])
        except ProcessError as error:
            write_process_log(path, '', error.output + '\n' + str(error), append=False)
            raise
        write_process_log(path, '', process.output, append=False)
        if process.returncode:
            raise BuildError('Build failed:\n' + process.output)

    def dashboard_report(self) -> Path:
        tag_file = self.build_directory / 'Testing/TAG'
        try:
            tag = tag_file.read_text().splitlines()[0]
        except (OSError, IndexError) as error:
            raise CheckError('CTest did not produce a dashboard tag.') from error
        if re.fullmatch(r'\d{8}-\d{4}', tag) is None:
            raise CheckError('CTest produced an invalid dashboard tag.')
        return tag_file.parent / tag / 'Test.xml'

    def run_student_tests(self) -> List[TestResult]:
        report = self.build_directory / 'check-results.xml'
        remove_if_exists(report)
        # CTest can reuse a dashboard tag. Discard its previous result before
        # running so a configuration error cannot reuse a successful report.
        if (self.build_directory / 'Testing/TAG').exists():
            remove_if_exists(self.dashboard_report())
        command = [
            'ctest', '-T', 'Test', '--verbose', '--no-compress-output',
            '-j', '1', '--timeout', str(TEST_TIMEOUT_SECONDS),
        ]
        if self.options.selection is not None:
            command.extend(['-R', '^' + re.escape(self.options.selection.full_name) + '$'])
        print('Running tests...', flush=True)
        process = run_process(command, self.build_directory)
        write_process_log(self.log_path, '', process.output, append=False)
        dashboard_report = self.dashboard_report()
        if not dashboard_report.is_file():
            raise CheckError('CTest did not produce a result file.\n' + process.output)
        shutil.copyfile(dashboard_report, report)
        results = read_ctest_results(report, self.build_directory)
        expected = (
            {self.options.selection.full_name} if self.options.selection
            else set(TESTS_BY_NAME)
        )
        actual = {result.test.full_name for result in results}
        if actual != expected:
            raise CheckError(
                'CTest and runner/catalog.py do not match. '
                f'Unexpected tests: {sorted(actual - expected)}. '
                f'Missing tests: {sorted(expected - actual)}.'
            )
        if process.returncode and all(result.passed for result in results):
            raise CheckError('CTest failed:\n' + process.output)
        return results

    def assignment_directory(self) -> Path:
        cache = self.build_directory / 'CMakeCache.txt'
        for line in cache.read_text().splitlines():
            if line.startswith('HW_DIR:') and '=' in line:
                path = Path(line.split('=', 1)[1])
                return path.resolve() if path.is_absolute() else (self.options.test_suite / path).resolve()
        return self.options.test_suite.parent.parent

    def reference_binaries(self) -> List[ReferenceBinary]:
        expected = reference_fingerprint(self.options.test_suite, self.assignment_directory())
        references = []
        for name in STRATEGIES:
            binary = self.references_directory / name / 'bin/shipwreck_reference'
            error_message = ''
            try:
                process = run_process([str(binary), '--reference-info'], timeout=5)
                if process.returncode:
                    raise CheckError('Cannot read reference metadata:\n' + process.output)
                manifest = json.loads(process.output)
                if not isinstance(manifest, dict):
                    raise ValueError('Reference metadata must be a JSON object.')
                if manifest.get('fingerprint') != expected or manifest.get('strategy') != name.capitalize():
                    raise CheckError(
                        'Binary does not match the current simulator and scenarios; rebuild references.'
                    )
            except (CheckError, ValueError) as error:
                error_message = str(error)
            references.append(ReferenceBinary(name.capitalize(), binary, error_message))
        return references

    def run_references(self, results: List[TestResult]) -> None:
        targets = [result for result in results if result.test.is_performance]
        if not targets:
            return
        print('Running references...', flush=True)
        references = self.reference_binaries()
        directory = self.build_directory / 'reference-results'
        directory.mkdir(exist_ok=True)
        for result in targets:
            for reference in references:
                result.references.append(self.run_reference_case(result, reference, directory))

    def run_reference_case(
        self, student: TestResult, reference: ReferenceBinary, directory: Path,
    ) -> TestResult:
        if reference.error:
            return TestResult(student.test, reference.strategy, passed=False, failure=reference.error)
        report = directory / f'{reference.strategy}.{student.test.full_name}.xml'
        command = TestCommand(
            student.test.full_name,
            (str(reference.path), '--gtest_filter=' + student.test.full_name),
            self.build_directory,
        )
        heading = f'{reference.strategy}: {student.test.full_name}'
        try:
            remove_if_exists(report)
            process = run_process(
                [*command.arguments, '--gtest_output=xml:' + str(report)],
                command.working_directory, TEST_TIMEOUT_SECONDS,
            )
            write_process_log(self.log_path, heading, process.output)
            result = read_reference_result(
                report, student.test, reference.strategy, process.output, process.elapsed_seconds,
            )
            result.command = command
            if process.returncode:
                result.fail(f'Reference exited with status {process.returncode}.')
            return result
        except (CheckError, OSError) as error:
            output = error.output if isinstance(error, ProcessError) else ''
            write_process_log(self.log_path, heading, output + '\n' + str(error))
            return TestResult(student.test, reference.strategy, passed=False, output=output, failure=str(error))

    def profile_memory(self, results: List[TestResult]) -> None:
        targets = [
            measurement
            for result in results if result.test.is_performance
            for measurement in [result, *result.references] if measurement.passed
        ]
        if not targets:
            return
        print('Profiling memory...', flush=True)
        directory = self.build_directory / 'massif'
        directory.mkdir(exist_ok=True)
        for result in targets:
            self.profile_case(result, directory)

    def profile_case(self, result: TestResult, directory: Path) -> None:
        report = directory / f'{result.strategy}.{result.test.full_name}.out'
        heading = f'Massif {result.strategy}: {result.test.full_name}'
        try:
            if result.command is None:
                raise CheckError('No native command is available for memory profiling.')
            remove_if_exists(report)
            command = [
                'valgrind', '--tool=massif', '--time-unit=B',
                '--peak-inaccuracy=0', '--massif-out-file=' + str(report),
                *result.command.arguments,
            ]
            process = run_process(
                command, result.command.working_directory,
                PROFILE_TIMEOUT_SECONDS,
            )
            write_process_log(self.log_path, heading, process.output)
            if process.returncode:
                raise CheckError(f'Massif run exited with status {process.returncode}.\n{process.output}')
            result.peak_memory_bytes = read_peak_memory(report)
        except (CheckError, OSError) as error:
            output = error.output if isinstance(error, ProcessError) else ''
            write_process_log(self.log_path, heading, output + '\n' + str(error))
            result.fail(f'Memory profiling failed: {error}', output)
