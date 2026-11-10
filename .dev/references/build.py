from pathlib import Path
import shutil
import sys

WORKSPACE = Path(__file__).resolve().parents[2]
TEST_SUITE = WORKSPACE / '.guides/tests'
sys.path.insert(0, str(TEST_SUITE))

from runner.processes import CheckError, ProcessError, run_process, write_process_log


def build_reference(strategy: str, references_directory: Path) -> None:
    build = WORKSPACE / 'build/references' / strategy
    build.mkdir(parents=True, exist_ok=True)
    log_path = build / 'build.log'
    log_path.write_text('')
    commands = (
        ['cmake', '-H' + str(references_directory), '-B' + str(build), f'-DREFERENCE={strategy}'],
        ['cmake', '--build', str(build)],
        [str(build / 'reference_checks'), '--gtest_filter=*'],
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
    destination = TEST_SUITE / 'references' / strategy / 'bin/shipwreck_reference'
    destination.parent.mkdir(parents=True, exist_ok=True)
    # build_portable.py publishes this copy only after checking it.
    shutil.copy2(build / 'shipwreck_reference', destination)
    print(f'Built {destination}')


def main() -> int:
    references_directory = Path(__file__).resolve().parent
    failed = False
    for name in ('starter', 'instructor'):
        print(f'Building {name.capitalize()}...', flush=True)
        try:
            build_reference(name, references_directory)
        except (CheckError, OSError) as error:
            print(error, file=sys.stderr)
            failed = True
    return int(failed)


if __name__ == '__main__':
    raise SystemExit(main())
