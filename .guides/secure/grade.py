#!/usr/bin/env python3

from contextlib import redirect_stdout
import io
import json
import os
from pathlib import Path
import sys
import tempfile
from typing import Tuple


SECURE_DIRECTORY = Path(__file__).resolve().parent
STUDENT_REPOSITORY = SECURE_DIRECTORY.parents[1] / 'hw5'
COURSE_SUPPORT = SECURE_DIRECTORY / '.support'
MAX_FEEDBACK_BYTES = 950_000

sys.dont_write_bytecode = True
if not COURSE_SUPPORT.is_dir():
    raise RuntimeError('Course grading files are missing. Run make export before publishing.')
sys.path.insert(0, str(SECURE_DIRECTORY))
sys.path.insert(0, str(COURSE_SUPPORT / 'tests'))

from runner.execution import CheckRunner
from runner.output import print_results
from runner.processes import BuildError, CheckError
from runner.results import calculate_grade
from submission import SubmissionError, prepare_submission


def evaluate_solution(project: Path) -> Tuple[float, str]:
    try:
        results = CheckRunner(project=project).run()
    except BuildError as error:
        return 0.0, 'Score: 0.0%\nYour submission could not build.\n\n' + str(error)
    feedback = io.StringIO()
    with redirect_stdout(feedback):
        print_results(results)
    return calculate_grade(results), feedback.getvalue()


def grade_submission(sha: str) -> Tuple[float, str]:
    with tempfile.TemporaryDirectory(prefix='hw5-grading-') as directory:
        checkout = Path(directory) / 'hw5'
        try:
            commit = prepare_submission(STUDENT_REPOSITORY, sha, checkout, COURSE_SUPPORT)
        except SubmissionError as error:
            return 0.0, 'Score: 0.0%\nSubmission error: ' + str(error) + '\n'
        score, feedback = evaluate_solution(checkout)
        return score, f'Graded commit: {commit}\n\n{feedback}'


def read_submitted_sha() -> str:
    if 'CODIO_FREE_TEXT_ANSWER' in os.environ:
        return os.environ['CODIO_FREE_TEXT_ANSWER']
    assignment = json.loads(os.environ.get('CODIO_AUTOGRADE_ENV', '{}'))
    assessments = assignment.get('assessments', {}).get('info', [])
    submissions = [item for item in assessments if item.get('name') == 'Final SHA']
    if len(submissions) != 1:
        raise RuntimeError('Configure exactly one Codio assessment named Final SHA.')
    answer = submissions[0].get('answer') or {}
    if answer and 'correctAnswer' not in answer:
        raise RuntimeError('Codio did not include the saved Final SHA text in answer.correctAnswer.')
    sha = answer.get('correctAnswer') or ''
    if not isinstance(sha, str):
        raise RuntimeError('Final SHA must be a text assessment containing a commit SHA.')
    return sha


def submit_grade(score: float, feedback: str) -> None:
    sys.path.insert(0, '/usr/share/codio/assessments')
    from lib.grade import FORMAT_V2_TXT, send_grade_v2, send_partial_v2

    encoded = feedback.encode('utf-8')
    if len(encoded) > MAX_FEEDBACK_BYTES:
        suffix = '\n[Feedback truncated.]\n'
        available = MAX_FEEDBACK_BYTES - len(suffix.encode('utf-8'))
        feedback = encoded[:available].decode('utf-8', errors='ignore') + suffix

    if os.environ.get('CODIO_PARTIAL_POINTS_V2_URL'):
        accepted = send_partial_v2(score, feedback, FORMAT_V2_TXT)
    else:
        accepted = send_grade_v2(score, feedback, FORMAT_V2_TXT)
    if not accepted:
        raise CheckError('Codio did not accept the grade.')


def main() -> int:
    try:
        sha = read_submitted_sha()
        score, feedback = grade_submission(sha)
        print(feedback, end='')
        submit_grade(score, feedback)
        return 0
    except KeyboardInterrupt:
        print('Grading interrupted; no grade submitted.', file=sys.stderr)
        return 130
    except Exception as error:
        print(f'Grading error: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace', line_buffering=True)
    raise SystemExit(main())
