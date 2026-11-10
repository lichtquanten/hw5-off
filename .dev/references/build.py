from pathlib import Path
import shutil
import sys
import tempfile

WORKSPACE = Path(__file__).resolve().parents[2]
TEST_SUITE = WORKSPACE / '.guides/tests'
sys.path.insert(0, str(TEST_SUITE))

from reference_tools import reference_fingerprint
from runner.processes import CheckError, ProcessError, remove_if_exists, run_process, write_process_log


def build_reference(strategy: str, references_directory: Path, scenario_fingerprint: str) -> None:
    source = references_directory / strategy / 'src'
    if not all((source / file).is_file() for file in ('search.h', 'search.cpp')):
        raise CheckError(f'{strategy.capitalize()} source missing: add search.h and search.cpp in {source}.')
    build = WORKSPACE / 'build/references' / strategy
    build.mkdir(parents=True, exist_ok=True)
    log_path = build / 'build.log'
    log_path.write_text('')
    commands = (
        [
            'cmake', '-H' + str(references_directory), '-B' + str(build),
            f'-DREFERENCE={strategy}', f'-DREFERENCE_LABEL={strategy.capitalize()}',
            f'-DREFERENCE_FINGERPRINT={scenario_fingerprint}',
        ],
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
    with tempfile.NamedTemporaryFile(dir=destination.parent, delete=False) as temporary:
        staged = Path(temporary.name)
    try:
        shutil.copy2(build / 'shipwreck_reference', staged)
        staged.replace(destination)
    finally:
        remove_if_exists(staged)
    print(f'Built {destination}')


def main() -> int:
    references_directory = Path(__file__).resolve().parent
    try:
        scenario_fingerprint = reference_fingerprint(TEST_SUITE)
    except OSError as error:
        print(error, file=sys.stderr)
        return 1
    failed = False
    for name in ('starter', 'instructor'):
        print(f'Building {name.capitalize()}...', flush=True)
        try:
            build_reference(name, references_directory, scenario_fingerprint)
        except (CheckError, OSError) as error:
            print(error, file=sys.stderr)
            failed = True
    return int(failed)


if __name__ == '__main__':
    raise SystemExit(main())
