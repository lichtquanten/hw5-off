"""Regression checks for CTest dashboard reports and grading failures."""
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
import xml.etree.ElementTree as ET

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / '.guides/tests'))

from runner.catalog import TESTS
from runner.execution import CheckRunner, RunOptions
from runner.processes import CheckError, ProcessResult
from runner.results import read_ctest_results, read_reference_result


def dashboard_case(name=TESTS[0].full_name, status='passed', output='', runtime='0.25',
                   completion='Completed'):
    case = ET.Element('Test', Status=status)
    ET.SubElement(case, 'Name').text = name
    ET.SubElement(case, 'Path').text = './shipwreck tests'
    ET.SubElement(case, 'FullCommandLine').text = '"/tmp/build with spaces/test" "--gtest_filter=' + name + '"'
    results = ET.SubElement(case, 'Results')
    for label, value in (('Execution Time', runtime), ('Completion Status', completion)):
        measurement = ET.SubElement(results, 'NamedMeasurement', name=label)
        ET.SubElement(measurement, 'Value').text = value
    ET.SubElement(ET.SubElement(results, 'Measurement'), 'Value').text = output
    return case


def write_dashboard(path, cases):
    root = ET.Element('Site')
    testing = ET.SubElement(root, 'Testing')
    testing.extend(cases)
    ET.ElementTree(root).write(str(path), encoding='utf-8', xml_declaration=True)


class ReportFixture(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix='hw5 runner ')
        self.addCleanup(self.directory.cleanup)
        self.build = Path(self.directory.name)
        self.report = self.build / 'check-results.xml'


class DashboardTests(ReportFixture):
    def read(self, *cases):
        write_dashboard(self.report, cases)
        return read_ctest_results(self.report, self.build)

    def test_passed_report_preserves_command_and_working_directory(self):
        result, = self.read(dashboard_case())
        self.assertTrue(result.passed)
        self.assertEqual(result.runtime_seconds, 0.25)
        self.assertEqual(result.command.arguments[0], '/tmp/build with spaces/test')
        self.assertEqual(result.command.working_directory, self.build / 'shipwreck tests')

    def test_failures_timeouts_and_tests_that_did_not_run_fail(self):
        for status, completion in (('failed', 'Failed'), ('failed', 'Timeout'),
                                   ('notrun', 'Unable to find executable')):
            with self.subTest(status=status, completion=completion):
                result, = self.read(dashboard_case(status=status, completion=completion))
                self.assertFalse(result.passed)
                self.assertEqual(result.failure, completion)

    def test_valgrind_errors_fail_even_when_ctest_passes(self):
        result, = self.read(dashboard_case(output='==123== ERROR SUMMARY: 2 errors from 1 contexts\n'))
        self.assertFalse(result.passed)
        self.assertIn('2 memory errors', result.failure)

    def test_ctest_310_timeout_can_have_completed_status(self):
        case = dashboard_case(status='failed', completion='Completed')
        measurement = ET.SubElement(case.find('Results'), 'NamedMeasurement', name='Exit Code')
        ET.SubElement(measurement, 'Value').text = 'Timeout'
        result, = self.read(case)
        self.assertFalse(result.passed)
        self.assertEqual(result.failure, 'Timeout')

    def test_performance_measurements_are_preserved(self):
        result, = self.read(dashboard_case(name=TESTS[12].full_name,
            output='[ SCENARIO ] seed=1 max_path=10000000\n[ TICKS    ] scenarios=1 total=12345\n'))
        self.assertTrue(result.passed)
        self.assertEqual(result.ticks, 12345)
        self.assertEqual(result.path_limit, 10000000)

    def test_performance_test_without_ticks_fails(self):
        result, = self.read(dashboard_case(name=TESTS[12].full_name))
        self.assertFalse(result.passed)
        self.assertIn('no search-time measurement', result.failure)

    def test_invalid_runtimes_are_rejected(self):
        for value in ('nan', 'inf', '-1', 'not a number'):
            with self.subTest(runtime=value), self.assertRaises(CheckError):
                self.read(dashboard_case(runtime=value))

    def test_duplicate_and_unknown_tests_are_rejected(self):
        for cases in ((dashboard_case(), dashboard_case()),
                      (dashboard_case(name='Shipwreck.Unknown'),)):
            with self.subTest(cases=cases), self.assertRaises(CheckError):
                self.read(*cases)

    def test_missing_and_malformed_commands_are_rejected(self):
        for command in ('', '"unterminated'):
            case = dashboard_case()
            case.find('FullCommandLine').text = command
            with self.subTest(command=command), self.assertRaises(CheckError):
                self.read(case)

    def test_reference_google_test_xml_still_works(self):
        self.report.write_text('<testsuites><testsuite><testcase classname="Shipwreck" '
            'name="NoReachablePaths" status="run" time="0.25"/></testsuite></testsuites>')
        result = read_reference_result(self.report, TESTS[0], 'Starter', '', 0.5)
        self.assertTrue(result.passed)
        self.assertEqual(result.runtime_seconds, 0.5)

    def test_missing_or_broken_xml_is_rejected(self):
        with self.assertRaises(CheckError):
            read_ctest_results(self.report, self.build)
        self.report.write_text('<Site>')
        with self.assertRaises(CheckError):
            read_ctest_results(self.report, self.build)


class RunnerTests(ReportFixture):
    def setUp(self):
        super().setUp()
        self.runner = CheckRunner(RunOptions(self.build, self.build, TESTS[0]))
        testing = self.build / 'Testing'
        testing.mkdir()
        (testing / 'TAG').write_text('20261110-1234\nExperimental\n')
        directory = testing / '20261110-1234'
        directory.mkdir()
        self.dashboard = directory / 'Test.xml'

    def run_cases(self, cases, returncode=0):
        def run(*args):
            write_dashboard(self.dashboard, cases)
            return ProcessResult(returncode, '', 0.1)
        with patch('runner.execution.run_process', side_effect=run):
            return self.runner.run_student_tests()

    def test_selected_test_is_accepted(self):
        result, = self.run_cases([dashboard_case()])
        self.assertTrue(result.passed)

    def test_empty_or_incomplete_reports_are_rejected(self):
        with self.assertRaises(CheckError):
            self.run_cases([])
        self.runner.options = RunOptions(self.build, self.build)
        with self.assertRaises(CheckError):
            self.run_cases([dashboard_case()])

    def test_nonzero_ctest_exit_with_passing_report_is_rejected(self):
        with self.assertRaises(CheckError):
            self.run_cases([dashboard_case()], returncode=1)

    def test_failed_student_test_is_returned_for_grading(self):
        result, = self.run_cases([dashboard_case(status='failed')], returncode=8)
        self.assertFalse(result.passed)

    def test_stale_successful_report_cannot_be_reused(self):
        write_dashboard(self.dashboard, [dashboard_case()])
        with patch('runner.execution.run_process', return_value=ProcessResult(1, 'CTest error', 0.1)):
            with self.assertRaises(CheckError):
                self.runner.run_student_tests()
        self.assertFalse(self.dashboard.exists())
        self.assertFalse(self.report.exists())


if __name__ == '__main__':
    unittest.main()
