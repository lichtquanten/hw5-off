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
    TestDefinition(1, 'EveryRobot', 'Wreck on Any Path'),
    TestDefinition(2, 'SimpleDebris', 'Search with Debris'),
    TestDefinition(3, 'BeforeAbyss', 'Wreck Before Abyss'),
    TestDefinition(4, 'BeyondAbyss', 'Wreck Beyond Abyss'),
    TestDefinition(5, 'TetherBoundary', 'Tether Boundary'),
    TestDefinition(6, 'SeededScenarios', 'Random Scenarios'),
    TestDefinition(
        7, 'ClusteredDebris', 'Strong Wreck Signal', is_performance=True,
        max_ticks=55_000_000, max_runtime_seconds=2, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        8, 'StrongFalseLeads', 'Strong False Leads', is_performance=True,
        max_ticks=62_000_000, max_runtime_seconds=2, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        9, 'WeakWreckSignal', 'Weak Wreck Signal', is_performance=True,
        max_ticks=77_000_000, max_runtime_seconds=2, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        10, 'SingleApproachTrail',
        'Debris Strongest on One Path (Wreck’s Path or Adjacent Path)', is_performance=True,
        max_ticks=65_000_000, max_runtime_seconds=2, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        11, 'VariedWreckSites', 'Small and Large Debris Clusters', is_performance=True,
        max_ticks=117_000_000, max_runtime_seconds=2, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        12, 'ExhaustiveSearch', 'No Wreck, Long Paths', is_performance=True,
        max_runtime_seconds=5, max_memory_bytes=2 * 1024 * 1024,
    ),
    TestDefinition(
        13, 'ManyRobotsSearch', 'No Wreck, 100k Robots', is_performance=True,
        max_runtime_seconds=5, max_memory_bytes=32 * 1024 * 1024,
    ),
)
TESTS_BY_NUMBER = {test.number: test for test in TESTS}
REFERENCES = {'Instructor': 'instructor', 'Round Robin': 'round_robin'}


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
