import os
from pathlib import Path
import re
import shutil
import subprocess
from typing import Sequence


class SubmissionError(Exception):
    pass


def run_git(directory: Path, arguments: Sequence[str], failure_message: str) -> str:
    environment = dict(os.environ, GIT_TERMINAL_PROMPT='0',
                       GIT_SSH_COMMAND='ssh -o BatchMode=yes -o ConnectTimeout=15')
    try:
        result = subprocess.run(
            ['git', *arguments], cwd=str(directory), env=environment,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            universal_newlines=True, errors='replace', check=True, timeout=45,
        )
    except subprocess.SubprocessError as error:
        raise SubmissionError(failure_message) from error
    return result.stdout.strip()


def validate_sha(sha: str) -> str:
    sha = sha.strip().lower()
    if not re.fullmatch(r'[0-9a-f]{7,40}', sha):
        raise SubmissionError('Submit a commit SHA (7–40 hexadecimal characters), not a branch or URL.')
    return sha


def repository_origin(project: Path) -> str:
    if not project.is_dir():
        raise SubmissionError('Your hw5 directory is missing. Clone your repository into hw5 first.')
    root = run_git(project, ['rev-parse', '--show-toplevel'],
                   'Your hw5 directory is not a Git repository. Connect it to your GitHub repository.')
    if Path(root).resolve() != project.resolve():
        raise SubmissionError('Your hw5 directory must be its own Git repository.')
    return run_git(project, ['remote', 'get-url', 'origin'],
                   'Your hw5 repository has no origin remote. Connect it to your GitHub repository.')


def clone_commit(project: Path, origin: str, sha: str, checkout: Path) -> str:
    run_git(project, ['clone', '--quiet', '--no-checkout', '--', origin, str(checkout)],
            'Could not clone your repository. Check origin and your GitHub access.')
    commit = run_git(checkout, ['rev-parse', '--verify', sha + '^{commit}'],
                     'That commit is not available in your remote repository. Check the SHA and git push.')
    if not commit.startswith(sha):
        raise SubmissionError('Submit a commit SHA, not a branch or tag name.')
    run_git(checkout, ['checkout', '--quiet', '--detach', commit],
            'Could not check out the submitted commit.')
    return commit


def install_course_support(checkout: Path, support: Path) -> None:
    for name in ('.support', 'build', 'mothership.h'):
        path = checkout / name
        if path.is_symlink() or path.is_file():
            path.unlink()
        elif path.exists():
            shutil.rmtree(path)
    shutil.copytree(str(support), str(checkout / '.support'))
    shutil.copy2(str(support / 'mothership.h'), str(checkout / 'mothership.h'))


def prepare_submission(project: Path, sha: str, checkout: Path, support: Path) -> str:
    sha = validate_sha(sha)
    if not all((support / name).is_file() for name in ('tests/CMakeLists.txt', 'mothership.h')):
        raise RuntimeError('Course grading files are missing. Run make export before publishing.')
    origin = repository_origin(project)
    commit = clone_commit(project, origin, sha, checkout)
    if not all((checkout / name).is_file() for name in ('search.cpp', 'search.h')):
        raise SubmissionError('The submitted repository must contain search.cpp and search.h at its root.')
    install_course_support(checkout, support)
    return commit
