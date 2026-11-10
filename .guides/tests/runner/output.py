import sys
from typing import List, NamedTuple, Optional

from .results import TestResult


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


def format_count(value: Optional[int]) -> str:
    return '—' if value is None else f'{value:,}'


def format_memory(value: Optional[int]) -> str:
    return '—' if value is None else f'{value / 1024:,.0f} KiB'


def format_runtime(seconds: Optional[float]) -> str:
    return '—' if seconds is None else f'{seconds:.2f} s'


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
    for label, mine, theirs in measurements:
        value = '—' if mine is None or not theirs else f'{mine / theirs:.2f}×'
        comparisons.append(f'{value} {label}')
    print('\nYours vs Instructor: ' + ' · '.join(comparisons))


def print_performance_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    print('\n' + style.heading + 'Performance Tests' + style.reset)
    for result in results:
        title = f'{result.test.display_id} {result.test.title}'
        print(f'\n{style.heading}{title}{style.reset} — {style.status(result.passed)}\n')
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
    print('\nTo run one test, run >make test TEST=<number>')
    print_basic_tests([result for result in results if not result.test.is_performance], style)
    print_performance_tests([result for result in results if result.test.is_performance], style)
    print_failures(results, style)
    print(f'\n{sum(result.passed for result in results)}/{len(results)} passed')
