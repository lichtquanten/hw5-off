from pathlib import Path
import sys

WORKSPACE = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(WORKSPACE / 'hw5/.support/tests'))

from runner.catalog import TESTS
from runner.execution import CheckRunner, memory_report, profile_peak_memory
from runner.processes import CheckError


def main() -> int:
    log = WORKSPACE / 'build/references/memory.log'
    log.parent.mkdir(parents=True, exist_ok=True)
    log.write_text('')
    runner = CheckRunner()
    try:
        for strategy, executable in runner.reference_binaries.items():
            for test in TESTS:
                if not test.is_performance:
                    continue
                print(f'Profiling {strategy}: {test.full_name}', flush=True)
                report = memory_report(runner.reference_profiles, strategy, test)
                profile_peak_memory(executable, test, report, log)
    except (CheckError, OSError) as error:
        print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
