from contextlib import redirect_stdout
import io
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch


WORKSPACE = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(WORKSPACE / 'hw5/.support/tests'))
sys.path.insert(0, str(WORKSPACE / '.guides/secure'))

from runner.catalog import REFERENCES, TESTS, resolve_test_selection
from runner.execution import CheckRunner, memory_report
from runner.output import format_count, format_memory, format_ratio, format_runtime, print_results
from runner.processes import BuildError, ProcessResult, run_process
from runner.results import TestResult, calculate_grade, parse_result
import grade


TESTS_BY_NAME = {test.name: test for test in TESTS}
BASIC_TEST = TESTS_BY_NAME['EveryRobot']
CLUSTERED_DEBRIS = TESTS_BY_NAME['ClusteredDebris']
STRONG_FALSE_LEADS = TESTS_BY_NAME['StrongFalseLeads']
EXHAUSTIVE_SEARCH = TESTS_BY_NAME['ExhaustiveSearch']


def passing_result(test):
    result = TestResult(test, 'Yours', True, runtime_seconds=0.1)
    result.ticks = 1
    result.peak_memory_bytes = 1
    return result


def all_passing():
    return {test.number: passing_result(test) for test in TESTS}


def render(results):
    output = io.StringIO()
    with redirect_stdout(output):
        print_results(results)
    return output.getvalue()


class GradingTests(unittest.TestCase):
    def test_catalog_numbers_are_consecutive_and_select_the_displayed_test(self):
        self.assertEqual([test.number for test in TESTS], list(range(1, len(TESTS) + 1)))
        for test in TESTS:
            with self.subTest(number=test.number):
                self.assertIs(resolve_test_selection(str(test.number)), test)
        for selection in ('0', str(len(TESTS) + 1)):
            with self.subTest(selection=selection), self.assertRaises(ValueError):
                resolve_test_selection(selection)

    def test_child_processes_do_not_inherit_codio_grading_endpoints(self):
        with patch.dict(os.environ, {'CODIO_AUTOGRADE_V2_URL': 'private-grade-url',
                                    'HW5_TEST_ENV': 'preserved'}):
            process = run_process([
                sys.executable, '-c', 'import os; print(os.environ.get("CODIO_AUTOGRADE_V2_URL", "absent")); '
                'print(os.environ["HW5_TEST_ENV"])',
            ])
        self.assertEqual(process.output.splitlines(), ['absent', 'preserved'])

    def test_each_limit_is_strict_and_missing_measurements_cannot_pass(self):
        test = CLUSTERED_DEBRIS
        for attribute, limit in (
            ('ticks', test.max_ticks),
            ('runtime_seconds', test.max_runtime_seconds),
            ('peak_memory_bytes', test.max_memory_bytes),
        ):
            for value, earns_credit in ((limit - 1, True), (limit, False),
                                        (limit + 1, False), (None, False)):
                with self.subTest(attribute=attribute, value=value):
                    result = passing_result(test)
                    setattr(result, attribute, value)
                    self.assertEqual(result.earns_credit, earns_credit)
                    self.assertTrue(result.passed)

    def test_correctness_is_required_even_when_every_limit_passes(self):
        result = passing_result(CLUSTERED_DEBRIS)
        result.fail('Wrong wreck location.')
        self.assertFalse(result.earns_credit)

    def test_basic_tests_need_only_correctness(self):
        result = TestResult(BASIC_TEST, 'Yours', True)
        self.assertTrue(result.earns_credit)
        self.assertEqual(result.performance_checks, [])

    def test_exhaustive_search_has_no_tick_limit_but_enforces_resources(self):
        result = passing_result(EXHAUSTIVE_SEARCH)
        result.ticks = 10 ** 15
        self.assertTrue(result.earns_credit)
        self.assertNotIn('Ticks', [check.label for check in result.performance_checks])
        result.peak_memory_bytes = result.test.max_memory_bytes
        self.assertFalse(result.earns_credit)

    def test_reference_failures_do_not_change_fixed_threshold_credit(self):
        result = passing_result(CLUSTERED_DEBRIS)
        result.references = [TestResult(result.test, 'Instructor', False)]
        self.assertTrue(result.earns_credit)

    def test_grade_counts_each_test_once_and_missing_tests_earn_nothing(self):
        by_number = all_passing()
        results = list(by_number.values())
        self.assertEqual(calculate_grade(results), 100)
        by_number[1].fail('Incorrect result.')
        by_number[CLUSTERED_DEBRIS.number].ticks = CLUSTERED_DEBRIS.max_ticks
        self.assertAlmostEqual(calculate_grade(results), 100 * (len(TESTS) - 2) / len(TESTS))
        self.assertAlmostEqual(calculate_grade([passing_result(BASIC_TEST)]), 100 / len(TESTS))

    def test_display_rounding_does_not_change_credit(self):
        result = passing_result(CLUSTERED_DEBRIS)
        result.ticks = result.test.max_ticks - 1
        self.assertEqual(format_count(result.ticks), format_count(result.test.max_ticks))
        self.assertTrue(result.earns_credit)
        result.ticks += 1
        self.assertFalse(result.earns_credit)

    def test_compact_units(self):
        for value, expected in ((None, '—'), (0, '0'), (9_999, '9999'), (10_000, '10K'),
                                (4_440_227, '4440K'), (22_908_711, '23M'),
                                (700_000_000, '700M'), (1_020_318_110, '1020M')):
            with self.subTest(value=value):
                self.assertEqual(format_count(value), expected)
        for value, expected in ((None, '—'), (512, '512 B'), (256 * 1024, '256 KiB'),
                                (int(1.6 * 1024 * 1024), '1638 KiB'), (4 * 1024 * 1024, '4096 KiB'),
                                (30 * 1024 * 1024, '30 MiB')):
            with self.subTest(value=value):
                self.assertEqual(format_memory(value), expected)
        for value, expected in ((None, '—'), (0.0134, '13 ms'), (8, '8000 ms')):
            with self.subTest(value=value):
                self.assertEqual(format_runtime(value), expected)
        for value, expected in ((0.524, '0.52×'), (1.32, '1.3×'), (11.18, '11×'), (1, '1×')):
            with self.subTest(value=value):
                self.assertEqual(format_ratio(value), expected)

    def test_checks_and_comparisons_are_in_the_selected_test_output(self):
        result = passing_result(CLUSTERED_DEBRIS)
        for strategy in REFERENCES:
            reference = passing_result(result.test)
            reference.strategy = strategy
            result.references.append(reference)
        output = render([result])
        for text in ('Correctness:', f'Ticks (<{format_count(CLUSTERED_DEBRIS.max_ticks)}):', 'Runtime (<2000 ms):',
                     'Peak memory (<2048 KiB):', 'Yours / Instructor',
                     'Yours / Round Robin', '1×', 'Credit: 1/1'):
            self.assertIn(text, output)
        self.assertNotIn('Score:', output)

    def test_exhaustive_search_displays_ticks_without_a_tick_verdict(self):
        output = render([passing_result(EXHAUSTIVE_SEARCH)])
        self.assertIn('Ticks', output)
        self.assertNotIn('Ticks (<', output)

    def test_multiple_scenarios_use_the_reported_total_ticks(self):
        output = '[ TICKS    ] scenarios=4 total=12345678\n[  PASSED  ] 1 test.\n'
        result = parse_result(CLUSTERED_DEBRIS, 'Yours', ProcessResult(0, output, 0.1))
        self.assertTrue(result.passed)
        self.assertEqual(result.scenario_count, 4)
        self.assertEqual(result.ticks, 12_345_678)
        heading = f'{CLUSTERED_DEBRIS.display_id} {CLUSTERED_DEBRIS.title}'
        self.assertIn(heading, render([result]).splitlines())

    def test_full_summary_distinguishes_correctness_from_credit(self):
        by_number = all_passing()
        by_number[CLUSTERED_DEBRIS.number].ticks = CLUSTERED_DEBRIS.max_ticks
        output = render(list(by_number.values()))
        total = len(TESTS)
        performance = sum(test.is_performance for test in TESTS)
        self.assertIn(f'Correctness: {total}/{total} passed', output)
        self.assertIn(f'Performance: {performance - 1}/{performance} passed', output)
        self.assertIn(f'Score: {100 * (total - 1) / total:.1f}% ({total - 1}/{total} credits)', output)

    def test_grader_and_test_output_use_the_same_score_and_feedback(self):
        by_number = all_passing()
        by_number[CLUSTERED_DEBRIS.number].runtime_seconds = CLUSTERED_DEBRIS.max_runtime_seconds
        results = list(by_number.values())
        with patch.object(grade, 'CheckRunner') as runner:
            runner.return_value.run.return_value = results
            score, feedback = grade.evaluate_solution(WORKSPACE / 'hw5')
        self.assertEqual(score, calculate_grade(results))
        self.assertEqual(feedback, render(results))

    def test_reference_memory_is_loaded_from_shipped_profiles(self):
        result = passing_result(CLUSTERED_DEBRIS)
        runner = CheckRunner()
        with tempfile.TemporaryDirectory() as directory:
            runner.reference_profiles = Path(directory)
            for strategy in REFERENCES:
                memory_report(runner.reference_profiles, strategy, result.test).write_text('mem_heap_B=1234\nmem_heap_B=5678\n')
            with patch.object(runner, 'run_case', side_effect=lambda test, strategy, command:
                              TestResult(test, strategy, True)), redirect_stdout(io.StringIO()):
                runner.run_references([result])
        self.assertEqual([reference.peak_memory_bytes for reference in result.references],
                         [5678, 5678])

    def test_missing_reference_profile_does_not_change_student_credit(self):
        result = passing_result(CLUSTERED_DEBRIS)
        runner = CheckRunner()
        with tempfile.TemporaryDirectory() as directory:
            runner.reference_profiles = Path(directory)
            with patch.object(runner, 'run_case', side_effect=lambda test, strategy, command:
                              TestResult(test, strategy, True)), redirect_stdout(io.StringIO()):
                runner.run_references([result])
        self.assertTrue(result.earns_credit)
        for reference in result.references:
            self.assertIsNone(reference.peak_memory_bytes)
            self.assertIn('Reference memory profile unavailable', reference.failure)

    def test_only_student_performance_cases_are_profiled_during_checks(self):
        basic = passing_result(BASIC_TEST)
        performance = passing_result(CLUSTERED_DEBRIS)
        performance.references = [passing_result(CLUSTERED_DEBRIS)]
        failed = TestResult(STRONG_FALSE_LEADS, 'Yours', False)
        runner = CheckRunner()
        with patch('runner.execution.profile_peak_memory', return_value=1234) as profile, \
                redirect_stdout(io.StringIO()):
            runner.profile_memory([basic, performance, failed])
        self.assertEqual(profile.call_count, 1)
        self.assertIs(profile.call_args[0][1], performance.test)
        self.assertEqual(performance.peak_memory_bytes, 1234)

    def test_build_failure_returns_zero(self):
        with patch.object(grade, 'CheckRunner') as runner:
            runner.return_value.run.side_effect = BuildError('Compiler error.')
            score, feedback = grade.evaluate_solution(WORKSPACE / 'hw5')
        self.assertEqual(score, 0)
        self.assertIn('could not build', feedback)


if __name__ == '__main__':
    unittest.main()
