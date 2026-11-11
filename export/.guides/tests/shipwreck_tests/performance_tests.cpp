#include "search_test_fixture.h"
#include "performance_scenarios.h"

using shipwreck_tests::DebrisPattern;
using shipwreck_tests::PathLayout;
using shipwreck_tests::WreckAvailability;

namespace
{
constexpr int BENCHMARK_PATH_LENGTH = 250'000;
constexpr int BENCHMARK_SCENARIOS = 8;

Scenario performanceScenario(
    unsigned seed, DebrisPattern pattern, int scenario,
    PathLayout layout = PathLayout::Uniform)
{
    const int robot = 12 + 25 * (scenario / 2);
    const int depthPercent = scenario % 2 == 0 ? 25 : 75;
    const int background = pattern == DebrisPattern::Sparse ? 2 : BENCHMARK_PATH_LENGTH / 5000;
    return shipwreck_tests::makePerformanceScenario(
        seed + robot * 10 + depthPercent, background, pattern, layout,
        WreckAvailability::Reachable, BENCHMARK_PATH_LENGTH,
        Location{robot, BENCHMARK_PATH_LENGTH * depthPercent / 100});
}
}

TEST_F(Shipwreck, LargeClusteredDebris)
{
    for (int scenario = 0; scenario < BENCHMARK_SCENARIOS; ++scenario)
    {
        SCOPED_TRACE(scenario);
        checkSearch(performanceScenario(155920, DebrisPattern::Clustered, scenario));
    }
}

TEST_F(Shipwreck, LargeMisleadingClusters)
{
    for (int scenario = 0; scenario < BENCHMARK_SCENARIOS; ++scenario)
    {
        SCOPED_TRACE(scenario);
        checkSearch(performanceScenario(155921, DebrisPattern::Misleading, scenario));
    }
}

TEST_F(Shipwreck, LargeWeakSignal)
{
    for (int scenario = 0; scenario < BENCHMARK_SCENARIOS; ++scenario)
    {
        SCOPED_TRACE(scenario);
        checkSearch(performanceScenario(155922, DebrisPattern::WeakSignal, scenario));
    }
}

TEST_F(Shipwreck, LargeSparseDebris)
{
    for (int scenario = 0; scenario < BENCHMARK_SCENARIOS; ++scenario)
    {
        SCOPED_TRACE(scenario);
        checkSearch(performanceScenario(155923, DebrisPattern::Sparse, scenario));
    }
}

TEST_F(Shipwreck, LargeUnequalPaths)
{
    for (int scenario = 0; scenario < BENCHMARK_SCENARIOS; ++scenario)
    {
        SCOPED_TRACE(scenario);
        checkSearch(performanceScenario(
            155924, DebrisPattern::Misleading, scenario, PathLayout::Mixed));
    }
}

TEST_F(Shipwreck, LargeUnfindable)
{
    checkSearch(shipwreck_tests::makePerformanceScenario(
        155925, 1000, DebrisPattern::Clustered,
        PathLayout::Uniform, WreckAvailability::Unfindable));
}
