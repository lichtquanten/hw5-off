import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

WORKSPACE = Path(__file__).resolve().parents[2]
TEST_SUITE = WORKSPACE / '.guides/tests'


VERSIONS = ('18.04', '20.04')
STRATEGIES = ('instructor', 'round_robin')
VALIDATE = 'getconf GNU_LIBC_VERSION && ' + ' && '.join(
    '/references/' + name + '/shipwreck_reference' for name in STRATEGIES)


def run(arguments):
    subprocess.run(arguments, check=True)


def main():
    references = Path(__file__).resolve().parent
    if shutil.which('docker') is None:
        raise SystemExit('Docker is required for the private portable reference build.')
    images = {}
    for version in VERSIONS:
        image = 'hw5-reference-tools:ubuntu' + version
        print('Preparing Ubuntu ' + version + ' tools...', flush=True)
        run(['docker', 'build', '--platform', 'linux/amd64',
             '--build-arg', 'UBUNTU_VERSION=' + version,
             '-t', image, '-f', str(references / 'portable.Dockerfile'), str(references)])
        images[version] = image

    with tempfile.TemporaryDirectory(prefix='hw5-portable-references-') as directory:
        staging = Path(directory)
        cache = staging / 'build'
        cache.mkdir()
        (WORKSPACE / 'build/references').mkdir(parents=True, exist_ok=True)
        output = staging / 'bin'
        mounts = ['--mount', 'type=bind,src=' + str(WORKSPACE) + ',dst=/workspace,readonly',
                  '--mount', 'type=bind,src=' + str(cache) + ',dst=/workspace/build/references']
        for name in STRATEGIES:
            (TEST_SUITE / 'references' / name / 'bin').mkdir(parents=True, exist_ok=True)
            destination = output / name
            destination.mkdir(parents=True)
            mounts += ['--mount', 'type=bind,src=' + str(destination)
                       + ',dst=/workspace/.guides/tests/references/' + name + '/bin']
        print('Building references with Ubuntu 18.04 libraries...', flush=True)
        run(['docker', 'run', '--rm', '--platform', 'linux/amd64',
             '--user', str(os.getuid()) + ':' + str(os.getgid()), *mounts,
             images['18.04'], 'python3', '.dev/references/build.py'])

        for version in VERSIONS:
            print('Checking the same binaries on Ubuntu ' + version + '...', flush=True)
            run(['docker', 'run', '--rm', '--platform', 'linux/amd64',
                 '--mount', 'type=bind,src=' + str(output) + ',dst=/references,readonly',
                 images[version], 'sh', '-c', VALIDATE])

        for name in STRATEGIES:
            destination = TEST_SUITE / 'references' / name / 'bin/shipwreck_reference'
            with tempfile.NamedTemporaryFile(dir=destination.parent, delete=False) as temporary:
                staged = Path(temporary.name)
            try:
                shutil.copy2(output / name / 'shipwreck_reference', staged)
                staged.replace(destination)
            finally:
                if staged.exists():
                    staged.unlink()
        print('Published references verified on Ubuntu 18.04 and 20.04.', flush=True)


if __name__ == '__main__':
    try:
        main()
    except (OSError, subprocess.CalledProcessError) as error:
        print('Portable reference build failed: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
