"""Privately build Linux x86-64 references and check both supported runtimes."""
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from reference_tools import reference_fingerprint


VERSIONS = ('18.04', '20.04')
STRATEGIES = ('starter', 'instructor')
VALIDATE = '''
import json
from pathlib import Path
import subprocess
import sys

subprocess.check_call(['getconf', 'GNU_LIBC_VERSION'])
for name in ('starter', 'instructor'):
    binary = str(Path('/references') / name / 'shipwreck_reference')
    metadata = json.loads(subprocess.check_output([binary, '--reference-info']).decode())
    if metadata != {'strategy': name.capitalize(), 'fingerprint': sys.argv[1]}:
        raise SystemExit('Reference metadata does not match: ' + name)
    subprocess.check_call([binary, '--gtest_filter=*'])
'''


def run(arguments):
    subprocess.run(arguments, check=True)


def main():
    references = Path(__file__).resolve().parent
    workspace = references.parents[2]
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
        (references.parent / '.reference-build').mkdir(exist_ok=True)
        output = staging / 'bin'
        mounts = ['--mount', 'type=bind,src=' + str(workspace) + ',dst=/workspace,readonly',
                  '--mount', 'type=bind,src=' + str(cache) + ',dst=/workspace/.guides/tests/.reference-build']
        for name in STRATEGIES:
            destination = output / name
            destination.mkdir(parents=True)
            mounts += ['--mount', 'type=bind,src=' + str(destination)
                       + ',dst=/workspace/.guides/tests/references/' + name + '/bin']
        print('Building references with Ubuntu 18.04 libraries...', flush=True)
        run(['docker', 'run', '--rm', '--platform', 'linux/amd64',
             '--user', str(os.getuid()) + ':' + str(os.getgid()), *mounts,
             images['18.04'], 'python3', '.guides/tests/references/build.py'])

        fingerprint = reference_fingerprint(references.parent)
        for version in VERSIONS:
            print('Checking the same binaries on Ubuntu ' + version + '...', flush=True)
            # Only executables are mounted during validation; no solution source.
            run(['docker', 'run', '--rm', '--platform', 'linux/amd64',
                 '--mount', 'type=bind,src=' + str(output) + ',dst=/references,readonly',
                 images[version], 'python3', '-c', VALIDATE, fingerprint])

        # Publish only after both runtime checks pass.
        for name in STRATEGIES:
            destination = references / name / 'bin/shipwreck_reference'
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
