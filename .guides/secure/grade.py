#!/usr/bin/env python3

import io
import sys
import tempfile
from contextlib import redirect_stdout
from pathlib import Path
from typing import Tuple

SECURE_DIRECTORY = Path(__file__).resolve().parent
WORKSPACE = SECURE_DIRECTORY.parent.parent
SUBMISSION_DIRECTORY = WORKSPACE / "grading/hw5.submission"
COURSE_SUPPORT = SECURE_DIRECTORY / ".support"
MAX_FEEDBACK_BYTES = 950_000

sys.dont_write_bytecode = True
if not COURSE_SUPPORT.is_dir():
    raise RuntimeError(
        "Course grading files are missing. Run make export before publishing."
    )
sys.path.insert(0, str(COURSE_SUPPORT / "tests"))

from runner.execution import CheckRunner
from runner.output import print_results
from runner.processes import BuildError, CheckError
from runner.results import calculate_grade
from submission import SubmissionError, prepare_submission


def evaluate_solution(project: Path) -> Tuple[float, str]:
    try:
        results = CheckRunner(project=project).run()
    except BuildError as error:
        return 0.0, "Score: 0.0%\nYour submission could not build.\n\n" + str(error)
    feedback = io.StringIO()
    with redirect_stdout(feedback):
        print_results(results)
    return calculate_grade(results), feedback.getvalue()


def grade_submission() -> Tuple[float, str]:
    try:
        with tempfile.TemporaryDirectory(prefix="hw5-grading-") as directory:
            project = Path(directory) / "hw5"
            prepare_submission(SUBMISSION_DIRECTORY, project, COURSE_SUPPORT)
            return evaluate_solution(project)
    except SubmissionError as error:
        return 0.0, f"Score: 0.0%\nSubmission error: {error}\n"


def submit_grade(score: float, feedback: str) -> None:
    sys.path.insert(0, "/usr/share/codio/assessments")
    from lib.grade import FORMAT_V2_TXT, send_grade_v2

    encoded = feedback.encode("utf-8")
    if len(encoded) > MAX_FEEDBACK_BYTES:
        suffix = "\n[Feedback truncated.]\n"
        available = MAX_FEEDBACK_BYTES - len(suffix.encode("utf-8"))
        feedback = encoded[:available].decode("utf-8", errors="ignore") + suffix

    if not send_grade_v2(score, feedback, FORMAT_V2_TXT):
        raise CheckError("Codio did not accept the grade.")


def main() -> int:
    try:
        score, feedback = grade_submission()
        print(feedback, end="")
        submit_grade(score, feedback)
        return 0
    except KeyboardInterrupt:
        print("Grading interrupted; no grade submitted.", file=sys.stderr)
        return 130
    except Exception as error:
        print(f"Grading error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.stdout = io.TextIOWrapper(
        sys.stdout.buffer, encoding="utf-8", errors="replace", line_buffering=True
    )
    raise SystemExit(main())
