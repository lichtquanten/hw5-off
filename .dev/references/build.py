from pathlib import Path
import sys

WORKSPACE = Path(__file__).resolve().parents[2]
TEST_SUITE = WORKSPACE / 'hw5/.support/tests'
sys.path.insert(0, str(TEST_SUITE))

from runner.catalog import REFERENCES
from runner.processes import CheckError, ProcessError, run_process, write_process_log


def build_reference(directory: str, build: Path) -> None:
    build.mkdir(parents=True, exist_ok=True)
    log_path = build / 'build.log'
    log_path.write_text('')
    solution = WORKSPACE / '.dev/references' / directory / 'src'
    commands = (
        ['cmake', '-H' + str(TEST_SUITE), '-B' + str(build), '-DSOLUTION_DIR=' + str(solution)],
        ['cmake', '--build', str(build)],
        [str(build / 'basic_tests')],
    )
    for command in commands:
        try:
            process = run_process(command, build)
        except ProcessError as error:
            write_process_log(log_path, command[0], error.output)
            raise CheckError(str(error) + '\n' + error.output) from error
        write_process_log(log_path, command[0], process.output)
        if process.returncode:
            raise CheckError(process.output)


def main() -> int:
    output = Path(sys.argv[1]).resolve()
    failed = False
    for strategy, directory in REFERENCES.items():
        print(f'Building {strategy}...', flush=True)
        try:
            build_reference(directory, output / directory)
        except (CheckError, OSError) as error:
            print(error, file=sys.stderr)
            failed = True
    return int(failed)


if __name__ == '__main__':
    raise SystemExit(main())
