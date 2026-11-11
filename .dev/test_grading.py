from contextlib import redirect_stdout
import importlib.util
import io
from pathlib import Path
import sys
import unittest
from unittest.mock import patch


WORKSPACE = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(WORKSPACE / '.guides/tests'))

from runner.catalog import TESTS, TestDefinition
from runner.output import format_count, format_memory, print_results
from runner.processes import BuildError, ProcessResult
from runner.results import TestResult, calculate_grade, parse_result


spec = importlib.util.spec_from_file_location('grade', WORKSPACE / '.guides/secure/grade.py')
grade = importlib.util.module_from_spec(spec)
spec.loader.exec_module(grade)


LIMITED_TEST = TestDefinition(
    13, 'LargeClusteredDebris', 'Clustered Debris', is_performance=True,
    max_ticks=650_000_000, max_runtime_seconds=8, max_memory_bytes=4 * 1024 * 1024,
)


def passing_result(test):
    result = TestResult(test, 'Yours', True, runtime_seconds=0.1)
    result.ticks = 1
    result.peak_memory_bytes = 1
    return result


def render(results):
    output = io.StringIO()
    with redirect_stdout(output):
        print_results(results)
    return output.getvalue()


class GradingTests(unittest.TestCase):
    def test_each_limit_is_strict_and_missing_measurements_cannot_pass(self):
        test = LIMITED_TEST
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
        result = passing_result(TESTS[12])
        result.fail('Wrong wreck location.')
        self.assertFalse(result.earns_credit)

    def test_basic_tests_need_only_correctness(self):
        result = TestResult(TESTS[0], 'Yours', True)
        self.assertTrue(result.earns_credit)
        self.assertEqual(result.performance_checks, [])

    def test_unfindable_has_no_tick_limit_but_enforces_resources(self):
        result = passing_result(TESTS[17])
        result.ticks = 10 ** 15
        self.assertTrue(result.earns_credit)
        self.assertNotIn('Ticks', [check.label for check in result.performance_checks])
        result.peak_memory_bytes = result.test.max_memory_bytes
        self.assertFalse(result.earns_credit)

    def test_reference_failures_do_not_change_fixed_threshold_credit(self):
        result = passing_result(TESTS[12])
        result.references = [TestResult(result.test, 'Instructor', False)]
        self.assertTrue(result.earns_credit)

    def test_grade_counts_each_test_once_and_missing_tests_earn_nothing(self):
        results = [passing_result(test) for test in TESTS]
        self.assertEqual(calculate_grade(results), 100)
        results[0].fail('Incorrect result.')
        results[12].ticks = results[12].test.max_ticks
        self.assertAlmostEqual(calculate_grade(results), 100 * 16 / 18)
        self.assertAlmostEqual(calculate_grade([passing_result(TESTS[0])]), 100 / 18)

    def test_display_rounding_does_not_change_credit(self):
        result = passing_result(LIMITED_TEST)
        result.ticks = result.test.max_ticks - 1
        self.assertEqual(format_count(result.ticks), '650M')
        self.assertTrue(result.earns_credit)
        result.ticks += 1
        self.assertEqual(format_count(result.ticks), '650M')
        self.assertFalse(result.earns_credit)

    def test_compact_units(self):
        for value, expected in ((None, '—'), (0, '0'), (999, '999'),
                                (1_000, '1K'), (700_000_000, '700M'),
                                (488_393_110, '488.4M'), (999_999, '1M'),
                                (1_020_318_110, '1.02B')):
            with self.subTest(value=value):
                self.assertEqual(format_count(value), expected)
        self.assertEqual(format_memory(4 * 1024 * 1024), '4 MiB')
        self.assertEqual(format_memory(256 * 1024), '256 KiB')

    def test_checks_and_comparisons_are_in_the_selected_test_output(self):
        result = passing_result(LIMITED_TEST)
        for strategy in ('Instructor', 'Round Robin'):
            reference = passing_result(result.test)
            reference.strategy = strategy
            result.references.append(reference)
        output = render([result])
        for text in ('Correctness:', 'Ticks (<650M):', 'Runtime (<8s):',
                     'Peak memory (<4 MiB):', 'Yours / Instructor',
                     'Yours / Round Robin', '1.00×', 'Credit: 1/1'):
            self.assertIn(text, output)
        self.assertNotIn('Score:', output)
        self.assertNotIn('heap', output)
        self.assertNotIn('Yours vs', output)
        self.assertNotIn('Starter', output)

    def test_unfindable_displays_ticks_without_a_tick_verdict(self):
        output = render([passing_result(TESTS[17])])
        self.assertIn('Ticks', output)
        self.assertNotIn('Ticks (<', output)

    def test_multiple_scenarios_use_the_reported_total_ticks(self):
        output = '[ TICKS    ] scenarios=8 total=12345678\n[  PASSED  ] 1 test.\n'
        result = parse_result(TESTS[12], 'Yours', ProcessResult(0, output, 0.1))
        self.assertTrue(result.passed)
        self.assertEqual(result.scenario_count, 8)
        self.assertEqual(result.ticks, 12_345_678)
        self.assertIn('Clustered Debris — 8 scenarios', render([result]))

    def test_full_summary_distinguishes_correctness_from_credit(self):
        results = [passing_result(test) for test in TESTS]
        results[12].ticks = results[12].test.max_ticks
        output = render(results)
        self.assertIn('Correctness: 18/18 passed', output)
        self.assertIn('Performance: 5/6 passed', output)
        self.assertIn('Score: 94.4% (17/18 credits)', output)

    def test_grader_and_test_output_use_the_same_score_and_feedback(self):
        results = [passing_result(test) for test in TESTS]
        results[12].runtime_seconds = results[12].test.max_runtime_seconds
        with patch.object(grade, 'CheckRunner') as runner:
            runner.return_value.run.return_value = results
            score, feedback = grade.evaluate()
        self.assertEqual(score, calculate_grade(results))
        self.assertEqual(feedback, render(results))

    def test_build_failure_returns_zero(self):
        with patch.object(grade, 'CheckRunner') as runner:
            runner.return_value.run.side_effect = BuildError('Compiler error.')
            score, feedback = grade.evaluate()
        self.assertEqual(score, 0)
        self.assertIn('could not build', feedback)


if __name__ == '__main__':
    unittest.main()
