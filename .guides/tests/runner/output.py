import sys
from typing import List, NamedTuple, Optional, Tuple

from .results import TestResult, measurement_ratio


class TerminalStyle(NamedTuple):
    success: str = ''
    failure: str = ''
    heading: str = ''
    reset: str = ''

    @classmethod
    def detect(cls) -> 'TerminalStyle':
        enabled = sys.stdout.isatty()
        return cls('\033[32m', '\033[31m', '\033[1m', '\033[0m') if enabled else cls()

    def status(self, passed: bool) -> str:
        return f'{self.success}PASS{self.reset}' if passed else f'{self.failure}FAIL{self.reset}'


def format_scaled(value: Optional[int], base: int, units: Tuple[str, ...], separator: str) -> str:
    if value is None:
        return '—'
    index, divisor = 0, 1
    while value >= divisor * base and index < len(units) - 1:
        divisor *= base
        index += 1
    if index == 0:
        return str(value) + (separator + units[0] if units[0] else '')
    tenths = (value * 10 + divisor // 2) // divisor
    if tenths >= base * 10 and index < len(units) - 1:
        tenths = 10
        index += 1
    amount = str(tenths // 10)
    if tenths % 10:
        amount += f'.{tenths % 10}'
    return amount + separator + units[index]


def format_count(value: Optional[int]) -> str:
    return format_scaled(value, 1000, ('', 'K', 'M', 'B', 'T', 'P', 'E'), '')


def format_memory(value: Optional[int]) -> str:
    return format_scaled(value, 1024, ('B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB'), ' ')


def format_runtime(seconds: Optional[float]) -> str:
    return '—' if seconds is None else f'{seconds:.2f} s'


def performance_title(result: TestResult) -> str:
    path_limit = next(
        (item.path_limit for item in [result, *result.references] if item.path_limit is not None),
        None,
    )
    if path_limit is None:
        return result.test.title
    return f'{result.test.title} (≤ {format_count(path_limit)} segm./robot)'


def print_basic_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    width = max(len(result.test.title) for result in results)
    print('\n' + style.heading + 'Basic Tests' + style.reset)
    for result in results:
        print(f'{result.test.display_id} {result.test.title:<{width}}  {style.status(result.passed)}')


def print_comparison(yours: TestResult) -> None:
    instructor = next((result for result in yours.references if result.strategy == 'Instructor'), None)
    if instructor is None or not yours.passed or not instructor.passed:
        return
    measurements = (
        ('ticks', yours.ticks, instructor.ticks),
        ('runtime', yours.runtime_seconds, instructor.runtime_seconds),
        ('memory', yours.peak_memory_bytes, instructor.peak_memory_bytes),
    )
    comparisons = []
    for label, numerator, denominator in measurements:
        ratio = measurement_ratio(numerator, denominator)
        value = '—' if ratio is None else f'{ratio:.2f}×'
        comparisons.append(f'{value} {label}')
    print('\nYours vs Instructor: ' + ' · '.join(comparisons))


def print_performance_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    print('\n' + style.heading + 'Performance Tests' + style.reset)
    for result in results:
        title = f'{result.test.display_id} {performance_title(result)}'
        print(f'\n{style.heading}{title}{style.reset} — {style.status(result.scenario_passed)}\n')
        heading = f'{"Strategy":<12} {"Search time (ticks)":>19} {"Runtime":>9} {"Peak memory":>12}'
        print(style.heading + heading + style.reset)
        for measurement in [result, *result.references]:
            ticks = format_count(measurement.ticks)
            runtime = format_runtime(measurement.runtime_seconds)
            memory = format_memory(measurement.peak_memory_bytes)
            print(f'{measurement.strategy:<12} {ticks:>19} {runtime:>9} {memory:>12}')
        print_comparison(result)


def print_failures(results: List[TestResult], style: TerminalStyle) -> None:
    for result in results:
        for measurement in [result, *result.references]:
            if not measurement.passed:
                label = f'{measurement.strategy}: {measurement.test.full_name}'
                print(f'\n{style.failure}FAIL{style.reset} {label}')
                print(measurement.failure)
                print(measurement.output)


def print_results(results: List[TestResult]) -> None:
    style = TerminalStyle.detect()
    print('\nTo run one test, run >make check TEST=<number>')
    print_basic_tests([result for result in results if not result.test.is_performance], style)
    print_performance_tests([result for result in results if result.test.is_performance], style)
    print_failures(results, style)
    print('\nK = thousand · M = million · B = billion')
    print(f'\n{sum(result.scenario_passed for result in results)}/{len(results)} passed')
