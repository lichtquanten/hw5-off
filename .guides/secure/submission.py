from pathlib import Path
import shutil


class SubmissionError(Exception):
    pass


def install_course_support(checkout: Path, support: Path) -> None:
    for name in ('.support', 'build', 'mothership.h'):
        path = checkout / name
        if path.is_symlink() or path.is_file():
            path.unlink()
        elif path.exists():
            shutil.rmtree(path)
    shutil.copytree(str(support), str(checkout / '.support'))
    shutil.copy2(str(support / 'mothership.h'), str(checkout / 'mothership.h'))


def prepare_submission(submission: Path, project: Path, support: Path) -> None:
    if not submission.is_dir():
        raise SubmissionError('No saved submission found. Submit your commit SHA in Final SHA first.')
    if not all((submission / name).is_file() for name in ('search.cpp', 'search.h')):
        raise SubmissionError('The saved submission must contain search.cpp and search.h at its root.')
    shutil.copytree(
        str(submission), str(project), symlinks=True,
        ignore=shutil.ignore_patterns('.git', '.support', 'build', '__pycache__'),
    )
    install_course_support(project, support)
