import hashlib
from pathlib import Path
from typing import List, Optional

from .catalog import TESTS, TestDefinition
from .processes import (
    BuildError, CheckError, ProcessError, remove_if_exists, run_process, write_process_log,
)
from .results import TestResult, parse_result, read_peak_memory


TEST_SUITE = Path(__file__).resolve().parents[1]
BUILD_DIRECTORY = TEST_SUITE.parents[1] / 'build/tests'
TEST_TIMEOUT_SECONDS = 30
PROFILE_TIMEOUT_SECONDS = 300
MEMCHECK = ('valgrind', '--tool=memcheck', '--leak-check=yes')


class CheckRunner:
    def __init__(self, selection: Optional[TestDefinition] = None):
        self.selection = selection
        self.log_path = BUILD_DIRECTORY / 'check.log'
        tests = BUILD_DIRECTORY / 'shipwreck_tests'
        references = TEST_SUITE / 'references'
        self.basic_tests = tests / 'shipwreck_tests'
        self.performance_tests = {
            'Yours': tests / 'shipwreck_runtime_tests',
            'Instructor': references / 'instructor/bin/shipwreck_reference',
            'Round Robin': references / 'round_robin/bin/shipwreck_reference',
        }

    def run(self) -> List[TestResult]:
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
        BUILD_DIRECTORY.mkdir(parents=True, exist_ok=True)
        configure = run_process([
            'cmake', '-H' + str(TEST_SUITE), '-B' + str(BUILD_DIRECTORY), '-G', 'Unix Makefiles',
        ])
        if configure.returncode:
            raise CheckError('Cannot configure the tests:\n' + configure.output)
        build = run_process(['cmake', '--build', str(BUILD_DIRECTORY), '--', '-j2'])
        log = BUILD_DIRECTORY / 'check-build.log'
        write_process_log(log, '', configure.output + build.output, append=False)
        if build.returncode:
            raise BuildError('Build failed:\n' + build.output)

    def run_student_test(self, test: TestDefinition) -> TestResult:
        if test.is_performance:
            return self.run_case(test, 'Yours', [str(self.performance_tests['Yours'])])
        return self.run_case(test, 'Yours', [*MEMCHECK, str(self.basic_tests)])

    def run_references(self, results: List[TestResult]) -> None:
        targets = [result for result in results if result.test.is_performance]
        if not targets:
            return
        print('Running references...', flush=True)
        for result in targets:
            for strategy in ('Instructor', 'Round Robin'):
                command = [str(self.performance_tests[strategy])]
                result.references.append(self.run_case(result.test, strategy, command))

    def run_case(self, test: TestDefinition, strategy: str, command: List[str]) -> TestResult:
        heading = f'{strategy}: {test.full_name}'
        try:
            process = run_process(
                [*command, '--gtest_filter=' + test.full_name],
                BUILD_DIRECTORY, TEST_TIMEOUT_SECONDS,
            )
        except ProcessError as error:
            write_process_log(self.log_path, heading, error.output + '\n' + str(error))
            return TestResult(test, strategy, False, error.output, str(error))
        write_process_log(self.log_path, heading, process.output)
        return parse_result(test, strategy, process)

    def profile_memory(self, results: List[TestResult]) -> None:
        targets = [
            measurement
            for result in results if result.test.is_performance
            for measurement in [result, *result.references] if measurement.passed
        ]
        if not targets:
            return
        print('Profiling memory...', flush=True)
        directory = BUILD_DIRECTORY / 'massif'
        directory.mkdir(exist_ok=True)
        for result in targets:
            self.profile_case(result, directory)

    def profile_case(self, result: TestResult, directory: Path) -> None:
        report = directory / f'{result.strategy}.{result.test.full_name}.out'
        heading = f'Massif {result.strategy}: {result.test.full_name}'
        command = [
            'valgrind', '--tool=massif', '--time-unit=B', '--peak-inaccuracy=0',
            '--massif-out-file=' + str(report), str(self.performance_tests[result.strategy]),
            '--gtest_filter=' + result.test.full_name,
        ]
        try:
            cache = report.with_suffix('.sha256')
            cache_key = None
            if result.strategy != 'Yours':
                executable = self.performance_tests[result.strategy]
                cache_key = hashlib.sha256(
                    executable.read_bytes() + '\0'.join(command).encode('utf-8')
                ).hexdigest()
                try:
                    if cache.read_text() == cache_key:
                        result.peak_memory_bytes = read_peak_memory(report)
                        write_process_log(self.log_path, heading, 'Using saved reference memory profile.\n')
                        return
                except (CheckError, OSError):
                    pass
                remove_if_exists(cache)
            remove_if_exists(report)
            process = run_process(command, BUILD_DIRECTORY, PROFILE_TIMEOUT_SECONDS)
            write_process_log(self.log_path, heading, process.output)
            if process.returncode:
                raise CheckError(f'Massif exited with status {process.returncode}.')
            result.peak_memory_bytes = read_peak_memory(report)
            if cache_key is not None:
                cache.write_text(cache_key)
        except (CheckError, OSError) as error:
            write_process_log(self.log_path, heading, str(error))
            result.failure = '\n'.join(part for part in (
                result.failure, 'Peak memory measurement unavailable: ' + str(error),
            ) if part)
