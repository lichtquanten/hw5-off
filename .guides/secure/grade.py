import argparse
from contextlib import redirect_stdout
import io
from pathlib import Path
import sys


WORKSPACE = Path(__file__).resolve().parents[2]
TEST_SUITE = WORKSPACE / 'hw5_tests'
BUILD_DIRECTORY = WORKSPACE / 'build/codio-grade'
sys.path.insert(0, str(TEST_SUITE))

from runner.catalog import TESTS
from runner.execution import CheckRunner, RunOptions
from runner.output import print_results
from runner.processes import BuildError, CheckError, run_process


def evaluate() -> tuple[float, str]:
    BUILD_DIRECTORY.mkdir(parents=True, exist_ok=True)
    configured = run_process([
        'cmake', '-S', str(TEST_SUITE), '-B', str(BUILD_DIRECTORY),
        '-G', 'Unix Makefiles', f'-DHW_DIR={WORKSPACE}',
    ])
    print(configured.output, end='')
    if configured.returncode:
        raise CheckError('Cannot configure the grading test suite.')

    try:
        results = CheckRunner(RunOptions(TEST_SUITE, BUILD_DIRECTORY)).run()
    except BuildError as error:
        return 0.0, 'Grade: 0.00%\nYour submission could not build.\n\n' + str(error)
    if len(results) != len(TESTS) or {r.test.full_name for r in results} != {t.full_name for t in TESTS}:
        raise CheckError('Grading did not return every registered test exactly once.')
    for result in results:
        for reference in result.references:
            if not reference.passed:
                raise CheckError(f'{reference.strategy} reference failed: {reference.failure}')
    passed = sum(result.passed for result in results)
    grade = 100.0 * passed / len(TESTS)
    buffer = io.StringIO()
    with redirect_stdout(buffer):
        print(f'Grade: {grade:.2f}% ({passed}/{len(TESTS)} tests passed)')
        print_results(results)
    return grade, buffer.getvalue()


def main() -> int:
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
