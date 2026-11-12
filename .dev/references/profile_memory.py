from pathlib import Path
import sys


WORKSPACE = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(WORKSPACE / '.guides/tests'))

from runner.catalog import TESTS
from runner.execution import BUILD_DIRECTORY, TEST_SUITE, profile_peak_memory
from runner.processes import CheckError


def main() -> int:
    BUILD_DIRECTORY.mkdir(parents=True, exist_ok=True)
    log = BUILD_DIRECTORY / 'reference-memory.log'
    log.write_text('')
    references = TEST_SUITE / 'references'
    try:
        for directory in ('instructor', 'round_robin'):
            strategy = directory.replace('_', ' ').title()
            executable = references / directory / 'bin/shipwreck_reference'
            for test in TESTS:
                if not test.is_performance:
                    continue
                print(f'Profiling {strategy}: {test.full_name}', flush=True)
                report = references / 'massif' / f'{strategy}.{test.full_name}.out'
                profile_peak_memory(executable, test, report, log)
    except (CheckError, OSError) as error:
        print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
