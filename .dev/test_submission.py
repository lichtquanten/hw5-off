import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


WORKSPACE = Path(__file__).resolve().parent.parent
SECURE = WORKSPACE / '.guides/secure'
sys.path.insert(0, str(SECURE))
import grade
from submission import SubmissionError, prepare_submission


def run(arguments, directory, **options):
    return subprocess.run(arguments, cwd=str(directory), check=True,
                          stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                          universal_newlines=True, **options).stdout.strip()


class SubmissionTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix='hw5-submission-test-')
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.project = self.root / 'hw5'
        shutil.copytree(str(WORKSPACE / 'hw5'), str(self.project),
                        ignore=shutil.ignore_patterns('.support', 'build', '__pycache__', '.git'))
        self.original_source = (self.project / 'search.cpp').read_bytes()
        (self.project / 'Makefile').write_text('$(error Do not run the submitted Makefile)\n')
        (self.project / 'mothership.h').write_text('#error Do not use the submitted API header\n')
        runner = self.project / '.support/tests/runner/execution.py'
        runner.parent.mkdir(parents=True)
        runner.write_text('raise RuntimeError("Do not import the submitted runner")\n')
        (runner.parent.parent / 'CMakeLists.txt').write_text('message(FATAL_ERROR "Student tests")\n')
        (self.project / 'build').mkdir()
        (self.project / 'build/tracked.cpp').write_text('#error Stale build output\n')
        self.git('init', '--quiet')
        self.git('symbolic-ref', 'HEAD', 'refs/heads/main')
        self.git('config', 'user.name', 'Grading Test')
        self.git('config', 'user.email', 'grading-test@example.invalid')
        self.git('config', 'commit.gpgsign', 'false')
        self.git('add', '.')
        self.git('add', '-f', 'build/tracked.cpp')
        self.git('commit', '--quiet', '-m', 'Submitted solution')
        self.sha = self.git('rev-parse', 'HEAD')
        self.remote = self.root / 'remote.git'
        run(['git', 'init', '--quiet', '--bare', str(self.remote)], self.root)
        run(['git', 'symbolic-ref', 'HEAD', 'refs/heads/main'], self.remote)
        self.git('remote', 'add', 'origin', str(self.remote))
        self.git('push', '--quiet', '--set-upstream', 'origin', 'main')
        self.submission = self.root / 'grading/hw5.submission'
        shutil.copytree(str(self.project), str(self.submission))

    def git(self, *arguments):
        return run(['git', *arguments], self.project)

    def prepare(self, name='checkout'):
        checkout = self.root / name
        prepare_submission(self.submission, checkout, SECURE / '.support')
        return checkout

    def break_newer_code(self):
        (self.project / 'search.cpp').write_text('#error Newer commit must not be graded\n')
        self.git('add', 'search.cpp')
        self.git('commit', '--quiet', '-m', 'Newer broken commit')
        self.git('push', '--quiet')
        (self.project / 'search.cpp').write_text('#error Uncommitted code must not be graded\n')

    def test_saved_files_are_copied_with_course_grading_files(self):
        self.break_newer_code()
        checkout = self.prepare()
        self.assertEqual((checkout / 'search.cpp').read_bytes(), self.original_source)
        self.assertFalse((checkout / 'build').exists())
        self.assertFalse((checkout / '.git').exists())
        self.assertEqual((checkout / '.support/tests/runner/execution.py').read_bytes(),
                         (SECURE / '.support/tests/runner/execution.py').read_bytes())
        self.assertEqual((checkout / 'mothership.h').read_bytes(),
                         (WORKSPACE / 'hw5/mothership.h').read_bytes())
        self.assertIn('Uncommitted', (self.project / 'search.cpp').read_text())
        (checkout / 'search.cpp').write_text('// Only change the temporary copy\n')
        self.assertEqual((self.submission / 'search.cpp').read_bytes(), self.original_source)
        self.assertIn('Do not use', (self.submission / 'mothership.h').read_text())
        self.assertTrue((self.submission / 'build/tracked.cpp').is_file())

    def test_missing_submission_does_not_use_working_repository(self):
        shutil.rmtree(str(self.submission))
        with self.assertRaisesRegex(SubmissionError, 'No saved submission'):
            self.prepare()

    def test_missing_required_files_are_submission_errors(self):
        for name in ('search.cpp', 'search.h'):
            path = self.submission / name
            original = path.read_bytes()
            path.unlink()
            with self.subTest(name=name), self.assertRaisesRegex(SubmissionError, 'must contain'):
                self.prepare()
            path.write_bytes(original)

    def test_replacing_symlinks_does_not_modify_their_targets(self):
        outside = self.root / 'outside'
        outside.mkdir()
        sentinel = outside / 'keep.txt'
        sentinel.write_text('keep')
        for name in ('.support', 'build'):
            shutil.rmtree(str(self.submission / name))
            (self.submission / name).symlink_to(outside, target_is_directory=True)
        (self.submission / 'mothership.h').unlink()
        (self.submission / 'mothership.h').symlink_to(sentinel)
        checkout = self.prepare()
        self.assertFalse((checkout / '.support').is_symlink())
        self.assertFalse((checkout / 'mothership.h').is_symlink())
        self.assertFalse((checkout / 'build').exists())
        self.assertTrue((self.submission / 'mothership.h').is_symlink())
        self.assertEqual(sentinel.read_text(), 'keep')

    def test_grading_infrastructure_failures_do_not_become_zero_scores(self):
        with patch.object(grade, 'SUBMISSION_DIRECTORY', self.submission), \
                patch.object(grade, 'evaluate_solution', side_effect=RuntimeError('Broken course setup')) as evaluate:
            with self.assertRaisesRegex(RuntimeError, 'Broken course setup'):
                grade.grade_submission()
        checkout = evaluate.call_args[0][0]
        self.assertFalse(checkout.parent.exists())

    def run_command(self, environment):
        secure = self.root / '.guides/secure'
        shutil.copytree(str(SECURE), str(secure), ignore=shutil.ignore_patterns('__pycache__'))
        library = self.root / 'stub/lib'
        library.mkdir(parents=True)
        (library / '__init__.py').write_text('')
        (library / 'grade.py').write_text(
            'import json, os\nfrom pathlib import Path\nFORMAT_V2_TXT = "txt"\n'
            'def record(api, score, feedback, format):\n'
            '    Path(os.environ["GRADE_RECEIPT"]).write_text(json.dumps(dict(\n'
            '        api=api, score=score, feedback=feedback, format=format)))\n'
            '    return True\n'
            'def send_grade_v2(*args): return record("assignment", *args)\n')
        receipt = self.root / 'receipt.json'
        env = {key: value for key, value in os.environ.items() if not key.startswith('CODIO_')}
        env.update(environment, PYTHONPATH=str(library.parent), GRADE_RECEIPT=str(receipt))
        output = run([sys.executable, '-B', str(secure / 'grade.py')], self.root, env=env, timeout=150)
        self.assertFalse((secure / 'build').exists())
        self.assertFalse(list(secure.rglob('__pycache__')))
        return json.loads(receipt.read_text()), output

    def test_assignment_grades_saved_files_after_original_and_remote_are_deleted(self):
        self.break_newer_code()
        shutil.rmtree(str(self.project))
        shutil.rmtree(str(self.remote))
        receipt, output = self.run_command({
            'CODIO_FREE_TEXT_ANSWER': 'main', 'CODIO_AUTOGRADE_ENV': 'not JSON',
        })
        self.assertEqual(receipt['api'], 'assignment')
        self.assertAlmostEqual(receipt['score'], 100 * 8 / 13)
        self.assertIn('Correctness: 13/13 passed', receipt['feedback'])
        self.assertIn('Score: 61.5% (8/13 checks)', output)
        self.assertEqual((self.submission / 'search.cpp').read_bytes(), self.original_source)
        self.assertIn('Do not use', (self.submission / 'mothership.h').read_text())
        self.assertTrue((self.submission / 'build/tracked.cpp').is_file())

    def test_assignment_command_reports_missing_submission_as_zero(self):
        shutil.rmtree(str(self.submission))
        receipt, _ = self.run_command({})
        self.assertEqual(receipt['api'], 'assignment')
        self.assertEqual(receipt['score'], 0.0)
        self.assertIn('No saved submission', receipt['feedback'])

    def test_assignment_command_reports_incomplete_submission_as_zero(self):
        (self.submission / 'search.h').unlink()
        receipt, _ = self.run_command({})
        self.assertEqual(receipt['score'], 0.0)
        self.assertIn('must contain search.cpp and search.h', receipt['feedback'])


if __name__ == '__main__':
    unittest.main()
