#!/usr/bin/env python3

import argparse
from contextlib import redirect_stdout
import io
from pathlib import Path
import sys
from typing import Tuple


sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tests'))

from runner.execution import CheckRunner
from runner.output import print_results
from runner.processes import BuildError, CheckError
from runner.results import calculate_grade


def evaluate() -> Tuple[float, str]:
    try:
        results = CheckRunner().run()
    except BuildError as error:
        return 0.0, 'Score: 0.0%\nYour submission could not build.\n\n' + str(error)
    grade = calculate_grade(results)
    buffer = io.StringIO()
    with redirect_stdout(buffer):
        print_results(results)
    return grade, buffer.getvalue()


def main() -> int:
    # Python 3.6 prints ASCII under a C locale, but the results use symbols such as '—'.
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, 'utf-8', 'replace', line_buffering=True)
    parser = argparse.ArgumentParser()
    parser.add_argument('--dry-run', action='store_true')
    options = parser.parse_args()
    try:
        if not options.dry_run:
            sys.path.insert(0, '/usr/share/codio/assessments')
            from lib.grade import FORMAT_V2_TXT, send_grade_v2

        grade, feedback = evaluate()
        print(feedback, end='')
        if options.dry_run:
            return 0
        encoded = feedback.encode('utf-8')
        if len(encoded) > 950_000:
            feedback = encoded[:949_900].decode('utf-8', errors='ignore') + '\n[Feedback truncated.]\n'
        if not send_grade_v2(grade, feedback, FORMAT_V2_TXT):
            raise CheckError('Codio did not accept the grade.')
        return 0
    except KeyboardInterrupt:
        print('Grading interrupted; no grade submitted.', file=sys.stderr)
        return 130
    except Exception as error:
        print(f'Grading error: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
