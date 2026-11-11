import argparse
import io
import os
import sys

from runner.catalog import resolve_test_selection
from runner.execution import CheckRunner
from runner.output import print_results
from runner.processes import CheckError


def main() -> int:
    # Python 3.6 prints ASCII under a C locale, but the results use symbols such as '—'.
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, 'utf-8', 'replace', line_buffering=True)
    parser = argparse.ArgumentParser()
    parser.add_argument('--test', default=os.environ.get('TEST', ''))
    try:
        selection = resolve_test_selection(parser.parse_args().test)
        results = CheckRunner(selection).run()
    except KeyboardInterrupt:
        print('Test run interrupted.', file=sys.stderr)
        return 130
    except (CheckError, OSError, ValueError) as error:
        print(error, file=sys.stderr)
        return 1
    print_results(results)
    return int(not all(result.earns_credit for result in results))


if __name__ == '__main__':
    raise SystemExit(main())
