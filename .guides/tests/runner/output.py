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
    if value is None:
        return '—'
    rounded = float(f'{value:.4g}')
    for scale, suffix in ((1_000_000_000, 'B'), (1_000_000, 'M'), (1_000, 'K')):
        if rounded >= scale:
            return f'{value / scale:.4g}{suffix}'
    return str(value)


def format_memory(value: Optional[int]) -> str:
    if value is None:
        return '—'
    for scale, unit in ((1024 ** 3, 'GiB'), (1024 ** 2, 'MiB'), (1024, 'KiB')):
        if value >= scale:
            number = f'{value / scale:.2f}'.rstrip('0').rstrip('.')
            return f'{number} {unit}'
    return f'{value} B'


def format_runtime(seconds: Optional[float]) -> str:
    return '—' if seconds is None else f'{seconds:.2f} s'


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
    for strategy in ('Instructor', 'Round Robin'):
        reference = next((result for result in yours.references if result.strategy == strategy), None)
        measurements = (
            (yours.ticks, reference.ticks if reference else None),
            (yours.runtime_seconds, reference.runtime_seconds if reference else None),
            (yours.peak_memory_bytes, reference.peak_memory_bytes if reference else None),
        )
        comparisons = [
            f'{mine / theirs:.2f}×'
            if yours.passed and reference is not None and reference.passed
            and mine is not None and theirs else '—'
            for mine, theirs in measurements
        ]
        print(table_row('Yours / ' + strategy, *comparisons))


def print_checks(result: TestResult, style: TerminalStyle) -> None:
    checks = [('Correctness:', result.passed)]
    for check in result.performance_checks:
        if check.label == 'Ticks':
            limit = format_count(check.limit)
        elif check.label == 'Runtime':
            limit = f'{check.limit:g}s'
        else:
            limit = format_memory(check.limit)
        checks.append((f'{check.label} (<{limit}):', check.passed))
    width = max(len(label) for label, _ in checks)
    for label, passed in checks:
        print(f'{label:<{width}}  {style.status(passed)}')


def print_performance_tests(results: List[TestResult], style: TerminalStyle) -> None:
    if not results:
        return
    print('\n' + style.heading + 'Performance Tests' + style.reset)
    for result in results:
        title = f'{result.test.display_id} {result.test.title}'
        if result.scenario_count is not None and result.scenario_count > 1:
            title += f' — {result.scenario_count} scenarios'
        print(f'\n{style.heading}{title}{style.reset}\n')
        print_checks(result, style)
        heading = table_row('', 'Ticks', 'Runtime', 'Peak memory')
        print()
        print(style.heading + heading + style.reset)
        for measurement in [result, *result.references]:
            ticks = format_count(measurement.ticks)
            runtime = format_runtime(measurement.runtime_seconds)
            memory = format_memory(measurement.peak_memory_bytes)
            print(table_row(measurement.strategy, ticks, runtime, memory))
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
    print('\nTo run one test: make test TEST=<number>')
    print_basic_tests([result for result in results if not result.test.is_performance], style)
    print_performance_tests([result for result in results if result.test.is_performance], style)
    print_failures(results, style)
    print(f'\nCorrectness: {sum(result.passed for result in results)}/{len(results)} passed')
    performance = [result for result in results if result.test.is_performance]
    if performance:
        print(f'Performance: {sum(result.earns_credit for result in performance)}/{len(performance)} passed')
    credits = sum(result.earns_credit for result in results)
    if {result.test.number for result in results} == {test.number for test in TESTS}:
        print(f'Score: {calculate_grade(results):.1f}% ({credits}/{len(TESTS)} credits)')
    else:
        print(f'Credit: {credits}/{len(results)}')
