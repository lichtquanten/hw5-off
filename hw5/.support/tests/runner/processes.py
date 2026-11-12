import os
from pathlib import Path
import subprocess
import time
from typing import NamedTuple, Optional, Sequence


class CheckError(Exception):
    pass


class BuildError(CheckError):
    pass


class ProcessResult(NamedTuple):
    returncode: int
    output: str
    elapsed_seconds: float


class ProcessError(CheckError):
    def __init__(self, message: str, output: str = ''):
        super().__init__(message)
        self.output = output


def run_process(
    arguments: Sequence[str],
    working_directory: Optional[Path] = None,
    timeout: Optional[float] = None,
) -> ProcessResult:
    started = time.perf_counter()
    try:
        process = subprocess.run(
            arguments,
            cwd=working_directory,
            env={name: value for name, value in os.environ.items() if not name.startswith('CODIO_')},
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
            errors='replace',
            timeout=timeout,
        )
    except subprocess.TimeoutExpired as error:
        output = error.output or ''
        if isinstance(output, bytes):
            output = output.decode(errors='replace')
        raise ProcessError(f'{arguments[0]} timed out after {timeout:g} seconds.', output) from error
    except OSError as error:
        raise ProcessError(f'Cannot run {arguments[0]}: {error}') from error
    return ProcessResult(process.returncode, process.stdout, time.perf_counter() - started)


def write_process_log(path: Path, heading: str, output: str, append: bool = True) -> None:
    with path.open('a' if append else 'w') as stream:
        if heading:
            stream.write(f'\n{heading}\n')
        stream.write(output)

