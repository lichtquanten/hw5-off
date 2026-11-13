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


def assignment_data(sha):
    return json.dumps({'assessments': {'info': [
        {'name': 'Another Question', 'answer': {'correctAnswer': 'irrelevant'}},
        {'name': 'Final SHA', 'answer': {'correctAnswer': sha}},
    ]}})


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
        remote = self.root / 'remote.git'
        run(['git', 'init', '--quiet', '--bare', str(remote)], self.root)
        run(['git', 'symbolic-ref', 'HEAD', 'refs/heads/main'], remote)
        self.git('remote', 'add', 'origin', str(remote))
        self.git('push', '--quiet', '--set-upstream', 'origin', 'main')

    def git(self, *arguments):
        return run(['git', *arguments], self.project)

    def prepare(self, sha, name='checkout'):
        checkout = self.root / name
        commit = prepare_submission(self.project, sha, checkout, SECURE / '.support')
        return checkout, commit

    def break_newer_code(self):
        (self.project / 'search.cpp').write_text('#error Newer commit must not be graded\n')
        self.git('add', 'search.cpp')
        self.git('commit', '--quiet', '-m', 'Newer broken commit')
        self.git('push', '--quiet')
        (self.project / 'search.cpp').write_text('#error Uncommitted code must not be graded\n')

    def test_exact_commit_and_course_files_replace_student_infrastructure(self):
        self.break_newer_code()
        checkout, commit = self.prepare(self.sha[:10].upper())
        self.assertEqual(commit, self.sha)
        self.assertEqual((checkout / 'search.cpp').read_bytes(), self.original_source)
        self.assertFalse((checkout / 'build').exists())
        self.assertEqual((checkout / '.support/tests/runner/execution.py').read_bytes(),
                         (SECURE / '.support/tests/runner/execution.py').read_bytes())
        self.assertEqual((checkout / 'mothership.h').read_bytes(),
                         (WORKSPACE / 'hw5/mothership.h').read_bytes())
        self.assertIn('Uncommitted', (self.project / 'search.cpp').read_text())

    def test_bad_missing_and_unpushed_shas_never_use_the_workspace(self):
        for sha in ('', 'main', '--help', self.sha + '; echo invalid'):
            with self.subTest(sha=sha), self.assertRaises(SubmissionError):
                self.prepare(sha)
        (self.project / 'search.cpp').write_text('// Unpushed solution\n')
        self.git('add', 'search.cpp')
        self.git('commit', '--quiet', '-m', 'Unpushed commit')
        with self.assertRaisesRegex(SubmissionError, 'git push'):
            self.prepare(self.git('rev-parse', 'HEAD'))
        self.git('remote', 'remove', 'origin')
        with self.assertRaisesRegex(SubmissionError, 'origin'):
            self.prepare(self.sha, 'missing-origin')

    def test_replacing_symlinks_does_not_modify_their_targets(self):
        outside = self.root / 'outside'
        outside.mkdir()
        sentinel = outside / 'keep.txt'
        sentinel.write_text('keep')
        for name in ('.support', 'build'):
            shutil.rmtree(str(self.project / name))
            (self.project / name).symlink_to(outside, target_is_directory=True)
        (self.project / 'mothership.h').unlink()
        (self.project / 'mothership.h').symlink_to(sentinel)
        self.git('add', '-A')
        self.git('commit', '--quiet', '-m', 'Symlink infrastructure')
        self.git('push', '--quiet')
        checkout, _ = self.prepare(self.git('rev-parse', 'HEAD'))
        self.assertFalse((checkout / '.support').is_symlink())
        self.assertFalse((checkout / 'mothership.h').is_symlink())
        self.assertFalse((checkout / 'build').exists())
        self.assertEqual(sentinel.read_text(), 'keep')

    def test_reads_only_the_submitted_sha_from_codio(self):
        with patch.dict(os.environ, {'CODIO_AUTOGRADE_ENV': assignment_data(self.sha)}, clear=True):
            self.assertEqual(grade.read_submitted_sha(), self.sha)
        with patch.dict(os.environ, {'CODIO_FREE_TEXT_ANSWER': self.sha}, clear=True):
            self.assertEqual(grade.read_submitted_sha(), self.sha)
        with patch.dict(os.environ, {'CODIO_AUTOGRADE_ENV': assignment_data('')}, clear=True):
            self.assertEqual(grade.read_submitted_sha(), '')
        with patch.dict(os.environ, {}, clear=True), self.assertRaisesRegex(RuntimeError, 'Final SHA'):
            grade.read_submitted_sha()

    def test_grading_infrastructure_failures_do_not_become_zero_scores(self):
        with patch.object(grade, 'STUDENT_REPOSITORY', self.project), \
                patch.object(grade, 'evaluate_solution', side_effect=RuntimeError('Broken course setup')) as evaluate:
            with self.assertRaisesRegex(RuntimeError, 'Broken course setup'):
                grade.grade_submission(self.sha)
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
            'def send_grade_v2(*args): return record("assignment", *args)\n'
            'def send_partial_v2(*args): return record("assessment", *args)\n')
        receipt = self.root / 'receipt.json'
        env = {key: value for key, value in os.environ.items() if not key.startswith('CODIO_')}
        env.update(environment, PYTHONPATH=str(library.parent), GRADE_RECEIPT=str(receipt))
        output = run([sys.executable, '-B', str(secure / 'grade.py')], self.root, env=env, timeout=150)
        self.assertFalse((secure / 'build').exists())
        self.assertFalse(list(secure.rglob('__pycache__')))
        return json.loads(receipt.read_text()), output

    def test_actual_assignment_command_grades_old_commit_with_course_tests(self):
        self.break_newer_code()
        receipt, output = self.run_command({'CODIO_AUTOGRADE_ENV': assignment_data(self.sha)})
        self.assertEqual(receipt['api'], 'assignment')
        self.assertAlmostEqual(receipt['score'], 100 * 8 / 13)
        self.assertIn('Graded commit: ' + self.sha, receipt['feedback'])
        self.assertIn('Correctness: 13/13 passed', receipt['feedback'])
        self.assertIn('Score: 61.5% (8/13 checks)', output)

    def test_assessment_command_reports_invalid_submission_as_zero(self):
        receipt, _ = self.run_command({'CODIO_FREE_TEXT_ANSWER': 'main',
                                       'CODIO_PARTIAL_POINTS_V2_URL': 'unused-test-endpoint'})
        self.assertEqual(receipt['api'], 'assessment')
        self.assertEqual(receipt['score'], 0.0)
        self.assertIn('Submission error', receipt['feedback'])


if __name__ == '__main__':
    unittest.main()
