import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
from typing import List

WORKSPACE = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(WORKSPACE / 'hw5/.support/tests'))

from runner.catalog import REFERENCES
from runner.execution import CheckRunner

BUILD_VERSION = '18.04'
CHECK_VERSIONS = ('18.04', '20.04')


def run(arguments: List[str]) -> None:
    subprocess.run(arguments, check=True)


def image(version: str) -> str:
    return 'hw5-reference-tools:ubuntu' + version


def build_image(version: str) -> None:
    print(f'Preparing Ubuntu {version} tools...', flush=True)
    context = Path(__file__).resolve().parent
    run(['docker', 'build', '--platform', 'linux/amd64', '--build-arg', 'UBUNTU_VERSION=' + version,
         '-t', image(version), '-f', str(context / 'portable.Dockerfile'), str(context)])


def run_in_container(version: str, output: Path, command: List[str]) -> None:
    run(['docker', 'run', '--rm', '--platform', 'linux/amd64', '--user', f'{os.getuid()}:{os.getgid()}',
         '--mount', f'type=bind,src={WORKSPACE},dst=/workspace,readonly',
         '--mount', f'type=bind,src={output},dst=/output',
         image(version), *command])


def check_binaries(version: str, output: Path) -> None:
    print(f'Checking the binaries on Ubuntu {version}...', flush=True)
    binaries = [f'/output/{directory}/performance_tests' for directory in REFERENCES.values()]
    run_in_container(version, output, ['sh', '-c', ' && '.join(['getconf GNU_LIBC_VERSION', *binaries])])


def publish(output: Path) -> None:
    for strategy, destination in CheckRunner().reference_binaries.items():
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(output / REFERENCES[strategy] / 'performance_tests', destination)


def main() -> None:
    if shutil.which('docker') is None:
        raise SystemExit('Docker is required for the portable reference build.')
    for version in CHECK_VERSIONS:
        build_image(version)
    with tempfile.TemporaryDirectory(prefix='hw5-references-') as directory:
        output = Path(directory)
        print(f'Building references with Ubuntu {BUILD_VERSION} libraries...', flush=True)
        run_in_container(BUILD_VERSION, output, ['python3', '.dev/references/build.py', '/output'])
        for version in CHECK_VERSIONS:
            check_binaries(version, output)
        publish(output)
    print(f'Published references built on Ubuntu {BUILD_VERSION} and checked on {", ".join(CHECK_VERSIONS)}.')


if __name__ == '__main__':
    try:
        main()
    except (OSError, subprocess.CalledProcessError) as error:
        print(f'Portable reference build failed: {error}', file=sys.stderr)
        raise SystemExit(1)
