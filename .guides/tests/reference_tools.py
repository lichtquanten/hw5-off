import hashlib
from pathlib import Path
from typing import Optional


STRATEGIES = ('instructor', 'starter')
SIMULATOR_FILES = (
    'src/mothership.h',
    '.guides/support/scenario.h',
    '.guides/support/scenario.cpp',
    '.guides/support/simulated_robot.h',
    '.guides/support/simulated_robot.cpp',
    '.guides/support/simulated_mothership.h',
    '.guides/support/simulated_mothership.cpp',
)
SCENARIO_FILES = (
    'shipwreck_tests/performance_tests.cpp',
    'shipwreck_tests/performance_scenarios.h',
    'shipwreck_tests/search_test_fixture.h',
)


def reference_fingerprint(test_suite: Path, assignment: Optional[Path] = None) -> str:
    test_suite = test_suite.resolve()
    assignment = assignment.resolve() if assignment is not None else test_suite.parent.parent
    files = [(name, assignment / name) for name in SIMULATOR_FILES]
    files += [('.guides/tests/' + name, test_suite / name) for name in SCENARIO_FILES]
    digest = hashlib.sha256()
    for name, path in sorted(files):
        digest.update(name.encode())
        digest.update(b'\0')
        digest.update(path.read_bytes())
        digest.update(b'\0')
    return digest.hexdigest()
