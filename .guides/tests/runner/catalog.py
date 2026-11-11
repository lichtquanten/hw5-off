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
    TestDefinition(4, 'DriveBoundaries', 'Drive Boundaries'),
    TestDefinition(5, 'BeforeAbyss', 'Wreck Before Abyss'),
    TestDefinition(6, 'BeyondAbyss', 'Wreck Beyond Abyss'),
    TestDefinition(7, 'DebrisAdjacentToWreck', 'Debris Adjacent to Wreck'),
    TestDefinition(8, 'DenseUnsortedDebris', 'Dense Unsorted Debris'),
    TestDefinition(9, 'TetherBoundary', 'Tether Boundary'),
    TestDefinition(10, 'FullFleetNoWreck', 'No Wreck, All Paths Open'),
    TestDefinition(11, 'IndependentSearches', 'Independent Searches'),
    TestDefinition(12, 'SeededScenarios', 'Varied Generated Scenarios'),
    TestDefinition(
        13, 'LargeClusteredDebris', 'Clustered Debris', is_performance=True,
        max_ticks=50_000_000, max_runtime_seconds=8, max_memory_bytes=4 * 1024 * 1024,
    ),
    TestDefinition(
        14, 'LargeMisleadingClusters', 'Misleading Clusters', is_performance=True,
        max_ticks=50_000_000, max_runtime_seconds=8, max_memory_bytes=4 * 1024 * 1024,
    ),
    TestDefinition(
        15, 'LargeWeakSignal', 'Weak Debris Signal', is_performance=True,
        max_ticks=60_000_000, max_runtime_seconds=8, max_memory_bytes=4 * 1024 * 1024,
    ),
    TestDefinition(
        16, 'LargeSparseDebris', 'Sparse Debris', is_performance=True,
        max_ticks=75_000_000, max_runtime_seconds=4, max_memory_bytes=256 * 1024,
    ),
    TestDefinition(
        17, 'LargeUnequalPaths', 'Unequal Path Lengths', is_performance=True,
        max_ticks=20_000_000, max_runtime_seconds=4, max_memory_bytes=4 * 1024 * 1024,
    ),
    TestDefinition(
        18, 'LargeUnfindable', 'No Reachable Wreck', is_performance=True,
        max_runtime_seconds=15, max_memory_bytes=16 * 1024 * 1024,
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
