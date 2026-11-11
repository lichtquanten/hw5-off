from pathlib import Path
import re
from typing import List, NamedTuple, Optional

from .catalog import TESTS, TestDefinition
from .processes import CheckError, ProcessResult


TICK_MEASUREMENT = re.compile(r'\[ TICKS +\] scenarios=(\d+) total=(\d+)')
MEMCHECK_ERRORS = re.compile(r'==\d+== ERROR SUMMARY: ([\d,]+)')
MEMCHECK_LINES = re.compile(r'^==\d+==.*(?:\n|$)', re.MULTILINE)
HEAP_MEASUREMENT = re.compile(r'^mem_heap_B=(\d+)$', re.MULTILINE)
ONE_TEST_PASSED = '[  PASSED  ] 1 test.'


class PerformanceCheck(NamedTuple):
    label: str
    value: Optional[float]
    limit: float

    @property
    def passed(self) -> bool:
        return self.value is not None and self.value < self.limit


class TestResult:
    def __init__(
        self,
        test: TestDefinition,
        strategy: str,
        passed: bool,
        output: str = '',
        failure: str = '',
        runtime_seconds: Optional[float] = None,
    ):
        self.test = test
        self.strategy = strategy
        self.passed = passed
        self.output = output
        self.failure = failure
        self.runtime_seconds = runtime_seconds
        self.ticks: Optional[int] = None
        self.scenario_count: Optional[int] = None
        self.peak_memory_bytes: Optional[int] = None
        self.references: List['TestResult'] = []

    def fail(self, message: str) -> None:
        self.passed = False
        self.failure = '\n'.join(part for part in (self.failure, message) if part)

    @property
    def performance_checks(self) -> List[PerformanceCheck]:
        measurements = (
            ('Ticks', self.ticks, self.test.max_ticks),
            ('Runtime', self.runtime_seconds, self.test.max_runtime_seconds),
            ('Peak memory', self.peak_memory_bytes, self.test.max_memory_bytes),
        )
        return [
            PerformanceCheck(label, value, limit)
            for label, value, limit in measurements if limit is not None
        ]

    @property
    def earns_credit(self) -> bool:
        return self.passed and all(check.passed for check in self.performance_checks)


def calculate_grade(results: List[TestResult]) -> float:
    return 100.0 * sum(result.earns_credit for result in results) / len(TESTS)


def parse_result(test: TestDefinition, strategy: str, process: ProcessResult) -> TestResult:
    output = process.output
    result = TestResult(test, strategy, True, output, runtime_seconds=process.elapsed_seconds)
    if process.returncode:
        result.fail(f'Test failed (exit status {process.returncode}).')
    elif ONE_TEST_PASSED not in output:
        result.fail('Test did not run.')
    memory_errors = sum(int(count.replace(',', '')) for count in MEMCHECK_ERRORS.findall(output))
    if memory_errors:
        result.fail(f'Memcheck reported {memory_errors} memory errors.')
    elif 'Process terminating' not in output:
        result.output = MEMCHECK_LINES.sub('', output)
    ticks = TICK_MEASUREMENT.search(output)
    if ticks:
        result.scenario_count = int(ticks[1])
        result.ticks = int(ticks[2])
    elif result.passed and test.is_performance:
        result.fail('Test produced no search-time measurement.')
    return result


def read_peak_memory(path: Path) -> int:
    values = HEAP_MEASUREMENT.findall(path.read_text())
    if not values:
        raise CheckError(f'No heap measurements in {path}.')
    return max(map(int, values))
