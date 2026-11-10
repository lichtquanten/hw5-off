import math
from pathlib import Path
import re
import shlex
from typing import List, Optional
import xml.etree.ElementTree as ET

from .catalog import TESTS_BY_NAME, TestDefinition
from .processes import CheckError, TestCommand


TICK_MEASUREMENT = re.compile(r'\[ TICKS +\] scenarios=\d+ total=(\d+)')
PATH_MEASUREMENT = re.compile(r'\[ SCENARIO +\].*max_path=(\d+)')
MEMCHECK_ERRORS = re.compile(r'==\d+== ERROR SUMMARY: ([\d,]+)')
MEMCHECK_LINES = re.compile(r'^==\d+==.*(?:\n|$)', re.MULTILINE)
HEAP_MEASUREMENT = re.compile(r'^mem_heap_B=(\d+)$', re.MULTILINE)


class TestResult:
    def __init__(
        self,
        test: TestDefinition,
        strategy: str,
        passed: bool,
        ticks: Optional[int] = None,
        runtime_seconds: Optional[float] = None,
        peak_memory_bytes: Optional[int] = None,
        path_limit: Optional[int] = None,
        output: str = '',
        failure: str = '',
        command: Optional[TestCommand] = None,
        references: Optional[List['TestResult']] = None,
    ):
        self.test = test
        self.strategy = strategy
        self.passed = passed
        self.ticks = ticks
        self.runtime_seconds = runtime_seconds
        self.peak_memory_bytes = peak_memory_bytes
        self.path_limit = path_limit
        self.output = output
        self.failure = failure
        self.command = command
        self.references = [] if references is None else references

    @property
    def scenario_passed(self) -> bool:
        return self.passed and all(result.passed for result in self.references)

    def fail(self, message: str, output: str = '') -> None:
        self.passed = False
        self.failure = '\n'.join(part for part in (self.failure, message) if part)
        if output:
            self.output += '\n' + output


def read_xml(path: Path) -> ET.Element:
    try:
        return ET.parse(path).getroot()
    except (OSError, ET.ParseError) as error:
        raise CheckError(f'Cannot read test results from {path}: {error}') from error


def parse_test_result(
    element: ET.Element,
    test: TestDefinition,
    strategy: str = 'Yours',
    output: Optional[str] = None,
    runtime_seconds: Optional[float] = None,
) -> TestResult:
    if element.tag == 'Test':
        output = element.findtext('Results/Measurement/Value', '')
        passed = element.get('Status') == 'passed'
        measurements = {
            child.get('name'): child.findtext('Value', '')
            for child in element.findall('Results/NamedMeasurement')
        }
        failures = [] if passed else [
            measurements.get('Exit Code') or measurements.get('Completion Status')
            or element.get('Status', 'Unknown test status.')
        ]
        time_value = measurements.get('Execution Time')
    else:
        output = element.findtext('system-out', '') if output is None else output
        failures = [
            child.text or child.get('message', '')
            for tag in ('failure', 'error', 'skipped')
            for child in element.findall(tag)
        ]
        passed = element.get('status') in ('run', 'passed') and not failures
        time_value = element.get('time')
    tick_match = TICK_MEASUREMENT.search(output)
    path_match = PATH_MEASUREMENT.search(output)
    if runtime_seconds is None and time_value is not None:
        try:
            runtime_seconds = float(time_value)
        except ValueError as error:
            raise CheckError(f'Invalid runtime for {test.full_name}.') from error
    if runtime_seconds is not None and (not math.isfinite(runtime_seconds) or runtime_seconds < 0):
        raise CheckError(f'Invalid runtime for {test.full_name}.')
    result = TestResult(
        test=test,
        strategy=strategy,
        passed=passed,
        ticks=int(tick_match[1]) if tick_match else None,
        runtime_seconds=runtime_seconds,
        path_limit=int(path_match[1]) if path_match else None,
        output=output,
        failure='\n'.join(failures).strip(),
    )
    memory_errors = sum(int(count.replace(',', '')) for count in MEMCHECK_ERRORS.findall(output))
    if memory_errors:
        result.fail(f'Memcheck reported {memory_errors} memory errors.')
    elif 'Process terminating' not in output:
        result.output = MEMCHECK_LINES.sub('', output)
    if result.passed and test.is_performance and result.ticks is None:
        result.fail('Test produced no search-time measurement.')
    return result


def read_ctest_results(path: Path, build_directory: Path) -> List[TestResult]:
    results = []
    seen = set()
    for element in read_xml(path).findall('./Testing/Test'):
        name = element.findtext('Name', '')
        if name not in TESTS_BY_NAME:
            raise CheckError(f'No test ID registered for {name}. Update runner/catalog.py.')
        if name in seen:
            raise CheckError(f'Duplicate test result for {name}.')
        seen.add(name)
        result = parse_test_result(element, TESTS_BY_NAME[name])
        try:
            arguments = tuple(shlex.split(element.findtext('FullCommandLine', '')))
        except ValueError as error:
            raise CheckError(f'Invalid CTest command for {name}: {error}') from error
        if arguments:
            directory = (build_directory / element.findtext('Path', '.')).resolve()
            result.command = TestCommand(name, arguments, directory)
        elif result.passed:
            raise CheckError(f'CTest produced no command for {name}.')
        results.append(result)
    return sorted(results, key=lambda result: result.test.number)


def read_reference_result(
    path: Path, test: TestDefinition, strategy: str, output: str, elapsed_seconds: float,
) -> TestResult:
    cases = read_xml(path).findall('.//testcase')
    if len(cases) != 1:
        raise CheckError('Reference did not run exactly one test.')
    element = cases[0]
    name = element.get('classname', '') + '.' + element.get('name', '')
    if name != test.full_name:
        raise CheckError('Reference did not run the requested test.')
    return parse_test_result(element, test, strategy, output, elapsed_seconds)


def read_peak_memory(path: Path) -> int:
    values = HEAP_MEASUREMENT.findall(path.read_text())
    if not values:
        raise CheckError(f'No heap measurements in {path}.')
    return max(map(int, values))


def measurement_ratio(numerator: Optional[float], denominator: Optional[float]) -> Optional[float]:
    if numerator is None or denominator is None or denominator <= 0:
        return None
    return numerator / denominator
