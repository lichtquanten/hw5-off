"""Recreate export/ from the files listed in .dev/student-files.txt."""
from pathlib import Path
import shutil
import sys


WORKSPACE = Path(__file__).resolve().parent.parent
# Anything else, such as .dev/, must never reach students.
STUDENT_ROOTS = ('src', '.guides', 'Makefile', '.gitignore')


def main() -> int:
    lines = (WORKSPACE / '.dev/student-files.txt').read_text().splitlines()
    names = [line.strip() for line in lines if line.strip() and not line.startswith('#')]
    destination = WORKSPACE / 'export'
    try:
        if destination.exists():
            shutil.rmtree(destination)
        for name in names:
            if Path(name).parts[0] not in STUDENT_ROOTS:
                raise ValueError('Not a student deliverable: ' + name)
            target = destination / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(WORKSPACE / name, target)
    except (OSError, ValueError) as error:
        print('Student export failed: ' + str(error), file=sys.stderr)
        return 1
    print('Student upload ready: ' + str(destination))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
