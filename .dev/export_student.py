"""Create export/ using only the explicitly listed student deliverables."""
from pathlib import Path, PurePosixPath
import shutil
import sys
import tempfile


WORKSPACE = Path(__file__).resolve().parent.parent


def student_files(workspace: Path):
    files = []
    manifest = workspace / '.dev/student-files.txt'
    for line in manifest.read_text().splitlines():
        name = line.strip()
        if not name or name.startswith('#'):
            continue
        path = PurePosixPath(name)
        if path.is_absolute() or '..' in path.parts or not path.parts:
            raise ValueError('Invalid student file path: ' + name)
        if path.parts[0] not in ('src', '.guides', 'Makefile', '.gitignore'):
            raise ValueError('Not a student deliverable: ' + name)
        if path in files:
            raise ValueError('Duplicate student file: ' + name)
        source = workspace
        for part in path.parts:
            source = source / part
            if source.is_symlink():
                raise ValueError('Student files cannot use symlinks: ' + name)
        if not source.is_file():
            raise ValueError('Missing student file: ' + name)
        files.append(path)
    if not files:
        raise ValueError('The student file list is empty.')
    return files


def export_student(workspace: Path) -> Path:
    files = student_files(workspace)
    destination = workspace / 'export'
    if destination.is_symlink():
        raise ValueError('The student export directory cannot be a symlink.')
    # Finish copying before replacing any previous generated export.
    with tempfile.TemporaryDirectory(prefix='.export-', dir=str(workspace)) as temporary:
        staging = Path(temporary) / 'export'
        staging.mkdir()
        for path in files:
            target = staging / path
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(str(workspace / path), str(target))
        if destination.exists():
            shutil.rmtree(str(destination))
        staging.replace(destination)
    return destination


def main() -> int:
    try:
        destination = export_student(WORKSPACE)
    except (OSError, ValueError) as error:
        print('Student export failed: ' + str(error), file=sys.stderr)
        return 1
    print('Student upload ready: ' + str(destination))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
