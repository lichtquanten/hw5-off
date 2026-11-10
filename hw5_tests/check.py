import argparse
import os
from pathlib import Path
import sys

from runner.catalog import resolve_test_selection
from runner.execution import CheckRunner, RunOptions
from runner.output import print_results
from runner.processes import CheckError


def parse_options() -> RunOptions:
    test_suite = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-dir', type=Path, default=test_suite)
    parser.add_argument('--test', default=os.environ.get('TEST', ''))
    args = parser.parse_args()
    return RunOptions(
        test_suite=test_suite,
        build_directory=args.build_dir.resolve(),
        selection=resolve_test_selection(args.test),
    )


def main() -> int:
    try:
        options = parse_options()
        results = CheckRunner(options).run()
    except KeyboardInterrupt:
        print('Test run interrupted.', file=sys.stderr)
        return 130
    except (CheckError, OSError, ValueError) as error:
        print(error, file=sys.stderr)
        return 1
    print_results(results)
    return int(not all(result.scenario_passed for result in results))


if __name__ == '__main__':
    raise SystemExit(main())
