from pathlib import Path
import json
import shutil
from typing import List, Optional

from .catalog import REFERENCES, TESTS, TestDefinition
from .processes import (
    BuildError, CheckError, ProcessError, run_process, write_process_log,
)
from .results import TestResult, parse_result, read_peak_memory


PROJECT = Path(__file__).resolve().parents[3]
TEST_TIMEOUT_SECONDS = 30
PROFILE_TIMEOUT_SECONDS = 300
MEMCHECK = ('valgrind', '--tool=memcheck', '--leak-check=yes')


def memory_report(directory: Path, strategy: str, test: TestDefinition) -> Path:
    return directory / f'{strategy}.{test.full_name}.out'


def profile_peak_memory(executable: Path, test: TestDefinition, report: Path, log: Path) -> int:
    report.parent.mkdir(parents=True, exist_ok=True)
    process = run_process([
        'valgrind', '--tool=massif', '--time-unit=B', '--peak-inaccuracy=0',
        '--massif-out-file=' + str(report), str(executable),
        '--gtest_filter=' + test.full_name,
    ], log.parent, PROFILE_TIMEOUT_SECONDS)
    write_process_log(log, 'Massif ' + report.stem, process.output)
    measurement = parse_result(test, executable.name, process)
    if not measurement.passed:
        raise CheckError(measurement.failure)
    return read_peak_memory(report)


class CheckRunner:
    def __init__(self, selection: Optional[TestDefinition] = None, project: Path = PROJECT):
        self.selection = selection
        self.test_suite = project / '.support/tests'
        self.build_directory = project / 'build/tests'
        self.log_path = self.build_directory / 'check.log'
        references = self.test_suite / 'references'
        self.reference_memory_path = references / 'memory.json'
        self.reference_binaries = {
            strategy: references / directory / 'bin/performance_tests'
            for strategy, directory in REFERENCES.items()
        }

    def run(self) -> List[TestResult]:
        if shutil.which('valgrind') is None:
            raise CheckError('Valgrind is required to check memory errors and peak memory usage.')
        self.build_tests()
        self.log_path.write_text('')
        tests = TESTS if self.selection is None else (self.selection,)
        print('Running tests...', flush=True)
        results = [self.run_student_test(test) for test in tests]
        self.run_references(results)
        self.profile_memory(results)
        return results

    def build_tests(self) -> None:
        print('Building tests...', flush=True)
        self.build_directory.mkdir(parents=True, exist_ok=True)
        configure = run_process([
            'cmake', '-H' + str(self.test_suite), '-B' + str(self.build_directory), '-G', 'Unix Makefiles',
        ])
        if configure.returncode:
            raise CheckError('Cannot configure the tests:\n' + configure.output)
        build = run_process(['cmake', '--build', str(self.build_directory), '--', '-j2'])
        log = self.build_directory / 'check-build.log'
        write_process_log(log, '', configure.output + build.output, append=False)
        if build.returncode:
            raise BuildError('Build failed:\n' + build.output)

    def run_student_test(self, test: TestDefinition) -> TestResult:
        if test.is_performance:
            return self.run_case(test, 'Yours', [str(self.build_directory / 'performance_tests')])
        return self.run_case(test, 'Yours', [*MEMCHECK, str(self.build_directory / 'basic_tests')])

    def run_references(self, results: List[TestResult]) -> None:
        targets = [result for result in results if result.test.is_performance]
        if not targets:
            return
        print('Running references...', flush=True)
        memory_error = ''
        try:
            memory = json.loads(self.reference_memory_path.read_text())
        except (OSError, ValueError) as error:
            memory = {}
            memory_error = str(error)
        for result in targets:
            for strategy, executable in self.reference_binaries.items():
                reference = self.run_case(result.test, strategy, [str(executable)])
                if reference.passed:
                    try:
                        reference.peak_memory_bytes = memory[strategy][result.test.full_name]
                    except KeyError as error:
                        reference.failure = 'Reference memory measurement unavailable: ' + (memory_error or str(error))
                result.references.append(reference)

    def run_case(self, test: TestDefinition, strategy: str, command: List[str]) -> TestResult:
        heading = f'{strategy}: {test.full_name}'
        try:
            process = run_process(
                [*command, '--gtest_filter=' + test.full_name],
                self.build_directory, TEST_TIMEOUT_SECONDS,
            )
        except ProcessError as error:
            write_process_log(self.log_path, heading, error.output + '\n' + str(error))
            return TestResult(test, strategy, False, error.output, str(error))
        write_process_log(self.log_path, heading, process.output)
        return parse_result(test, strategy, process)

    def profile_memory(self, results: List[TestResult]) -> None:
        targets = [result for result in results if result.test.is_performance and result.passed]
        if not targets:
            return
        print('Profiling memory...', flush=True)
        for result in targets:
            report = memory_report(self.build_directory / 'massif', 'Yours', result.test)
            try:
                result.peak_memory_bytes = profile_peak_memory(
                    self.build_directory / 'performance_tests', result.test, report, self.log_path)
            except (CheckError, OSError) as error:
                write_process_log(self.log_path, 'Massif ' + report.stem, str(error))
                result.failure = 'Peak memory measurement unavailable: ' + str(error)
