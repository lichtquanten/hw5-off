from typing import NamedTuple, Optional


class TestDefinition(NamedTuple):
    number: int
    name: str
    title: str
    is_performance: bool = False
    max_ticks: Optional[int] = None
    max_runtime_seconds: Optional[float] = None
    max_memory_bytes: Optional[int] = None

    @property
    def full_name(self) -> str:
        return 'Shipwreck.' + self.name

    @property
    def display_id(self) -> str:
        return f'[{self.number:02d}]'


TESTS = (
    TestDefinition(1, 'NoReachablePaths', 'All Paths Blocked'),
    TestDefinition(2, 'EmptyUnequalPaths', 'Empty Unequal Paths'),
    TestDefinition(3, 'EveryRobot', 'Wreck on Each Robot Path'),
    TestDefinition(4, 'SimpleDebris', 'Search with Debris'),
    TestDefinition(5, 'BeforeAbyss', 'Wreck Before Abyss'),
    TestDefinition(6, 'BeyondAbyss', 'Wreck Beyond Abyss'),
    TestDefinition(7, 'TetherBoundary', 'Tether Boundary'),
    TestDefinition(8, 'SeededScenarios', 'Mixed Search Scenarios (10 cases)'),
    TestDefinition(
        9, 'ClusteredDebris', 'Clustered Debris', is_performance=True,
        max_ticks=10_000_000, max_runtime_seconds=8, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        10, 'StrongFalseLeads', 'Strong False Leads', is_performance=True,
        max_ticks=12_000_000, max_runtime_seconds=8, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        11, 'WeakWreckSignal', 'Weak Wreck Signal', is_performance=True,
        max_ticks=16_000_000, max_runtime_seconds=8, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        12, 'ExhaustiveSearch', 'No Wreck, Full Search', is_performance=True,
        max_runtime_seconds=8, max_memory_bytes=4 * 1024 * 1024,
    ),
)
TESTS_BY_NUMBER = {test.number: test for test in TESTS}


def resolve_test_selection(selection: str) -> Optional[TestDefinition]:
    if not selection:
        return None
    if not selection.isdecimal():
        raise ValueError(f'TEST must be a number from 1 to {max(TESTS_BY_NUMBER)}.')
    definition = TESTS_BY_NUMBER.get(int(selection))
    if definition is None:
        available = ', '.join(map(str, TESTS_BY_NUMBER))
        raise ValueError(f'Unknown test number: {selection}.\nAvailable IDs: {available}')
    return definition
