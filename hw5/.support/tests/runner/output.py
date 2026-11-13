import sys
from typing import List, NamedTuple, Optional

from .catalog import TESTS
from .results import TestResult, calculate_grade


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
    return '—' if value is None else f'{round(value / 1_000_000):,}M'


def format_memory(value: Optional[int]) -> str:
    return '—' if value is None else f'{round(value / 1024):,} KiB'


def format_runtime(seconds: Optional[float]) -> str:
    return '—' if seconds is None else f'{round(seconds * 1000):,} ms'


def format_ratio(ratio: float) -> str:
    rounded = float(f'{ratio:.2g}')
    return f'{rounded:g}×'


LIMIT_FORMATTERS = {
    'Ticks': format_count,
    'Runtime': lambda seconds: f'{seconds:g} s',
    'Peak memory': lambda size: f'{size / 1024 ** 2:g} MiB',
}


def print_basic_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    width = max(len(result.test.title) for result in results)
    print('\n' + style.heading + 'Basic Tests' + style.reset)
    for result in results:
        print(f'{result.test.display_id} {result.test.title:<{width}}  {style.status(result.passed)}')


def table_row(label: str, ticks: str, runtime: str, memory: str) -> str:
    return f'{label:<20} {ticks:>10} {runtime:>10} {memory:>12}'


def print_comparisons(yours: TestResult) -> None:
    print('─' * len(table_row('', '', '', '')))
    for reference in yours.references:
        measurements = (
            (yours.ticks, reference.ticks),
            (yours.runtime_seconds, reference.runtime_seconds),
            (yours.peak_memory_bytes, reference.peak_memory_bytes),
        )
        comparisons = [
            format_ratio(mine / theirs)
            if yours.passed and reference.passed and mine is not None and theirs else '—'
            for mine, theirs in measurements
        ]
        print(table_row('Yours / ' + reference.strategy, *comparisons))


def print_checks(result: TestResult, style: TerminalStyle) -> None:
    checks = [('Correctness:', result.passed)]
    for check in result.performance_checks:
        checks.append((f'{check.label} (<{LIMIT_FORMATTERS[check.label](check.limit)}):', check.passed))
    width = max(len(label) for label, _ in checks)
    for label, passed in checks:
        print(f'{label:<{width}}  {style.status(passed)}')


def print_performance_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    print('\n' + style.heading + 'Performance Tests' + style.reset)
    for result in results:
        title = f'{result.test.display_id} {result.test.title}'
        print(f'\n{style.heading}{title}{style.reset}\n')
        print_checks(result, style)
        print()
        print(style.heading + table_row('', 'Ticks', 'Runtime', 'Peak memory') + style.reset)
        for measurement in [result, *result.references]:
            print(table_row(
                measurement.strategy,
                format_count(measurement.ticks),
                format_runtime(measurement.runtime_seconds),
                format_memory(measurement.peak_memory_bytes),
            ))
        print_comparisons(result)


def print_failures(results: List[TestResult], style: TerminalStyle) -> None:
    for result in results:
        for measurement in [result, *result.references]:
            if not measurement.passed or measurement.failure:
                label = f'{measurement.strategy}: {measurement.test.full_name}'
                print(f'\n{style.failure}FAIL{style.reset} {label}')
                print(measurement.failure)
                print(measurement.output)


def print_results(results: List[TestResult]) -> None:
    style = TerminalStyle.detect()
    basic = [result for result in results if not result.test.is_performance]
    performance = [result for result in results if result.test.is_performance]
    print('\nTo run one test: make test TEST=<number>')
    print_basic_tests(basic, style)
    print_performance_tests(performance, style)
    print_failures(results, style)
    print(f'\nCorrectness: {sum(result.passed for result in results)}/{len(results)} passed')
    if performance:
        print(f'Performance: {sum(result.earns_credit for result in performance)}/{len(performance)} passed')
    checks = sum(result.earns_credit for result in results)
    if {result.test.number for result in results} == {test.number for test in TESTS}:
        print(f'Score: {calculate_grade(results):.1f}% ({checks}/{len(TESTS)} checks)')
    else:
        print(f'Checks: {checks}/{len(results)}')
