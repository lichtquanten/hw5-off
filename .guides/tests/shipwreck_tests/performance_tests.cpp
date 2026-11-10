#include "search_test_fixture.h"
#include "performance_scenarios.h"

using shipwreck_tests::DebrisPattern;
using shipwreck_tests::PathLayout;
using shipwreck_tests::WreckAvailability;

namespace
{
constexpr unsigned SCENARIO_SEED = 155920;
constexpr int BACKGROUND_DEBRIS_PER_ROBOT = 1000;

Scenario performanceScenario(
    DebrisPattern pattern, int backgroundPerRobot = BACKGROUND_DEBRIS_PER_ROBOT,
    PathLayout layout = PathLayout::Uniform,
    WreckAvailability wreckAvailability = WreckAvailability::Reachable)
{
    return shipwreck_tests::makePerformanceScenario(
        SCENARIO_SEED, backgroundPerRobot, pattern, layout, wreckAvailability);
}
}

TEST_F(Shipwreck, LargeClusteredDebris)
{
    checkSearch(performanceScenario(DebrisPattern::Clustered));
}

TEST_F(Shipwreck, LargeMisleadingClusters)
{
    checkSearch(performanceScenario(DebrisPattern::Misleading));
}

TEST_F(Shipwreck, LargeWeakSignal)
{
    checkSearch(performanceScenario(DebrisPattern::WeakSignal));
}

TEST_F(Shipwreck, LargeSparseDebris)
{
    checkSearch(performanceScenario(DebrisPattern::Sparse, 2));
}

TEST_F(Shipwreck, LargeUnequalPaths)
{
    checkSearch(performanceScenario(
        DebrisPattern::Misleading, BACKGROUND_DEBRIS_PER_ROBOT, PathLayout::Mixed));
}

TEST_F(Shipwreck, LargeUnfindable)
{
    checkSearch(performanceScenario(DebrisPattern::Clustered, BACKGROUND_DEBRIS_PER_ROBOT,
        PathLayout::Uniform, WreckAvailability::Unfindable));
}
