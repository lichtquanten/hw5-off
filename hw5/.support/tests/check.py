import io
import os
import sys

from runner.catalog import resolve_test_selection
from runner.execution import CheckRunner
from runner.output import print_results
from runner.processes import CheckError


def main() -> int:
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, 'utf-8', 'replace', line_buffering=True)
    try:
        selection = resolve_test_selection(os.environ.get('TEST', ''))
        results = CheckRunner(selection).run()
    except KeyboardInterrupt:
        print('Test run interrupted.', file=sys.stderr)
        return 130
    except (CheckError, OSError, ValueError) as error:
        print(error, file=sys.stderr)
        return 1
    print_results(results)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
