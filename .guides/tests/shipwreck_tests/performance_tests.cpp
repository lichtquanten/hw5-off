#include "search_test_fixture.h"
#include "performance_scenarios.h"

using shipwreck_tests::DebrisPattern;
using shipwreck_tests::PathLayout;
using shipwreck_tests::WreckAvailability;

namespace
{
constexpr unsigned SCENARIO_SEED = 155920;
constexpr int BACKGROUND_DEBRIS_PER_ROBOT = 1000;

const std::string SCENARIO_LABEL =
    "seed=" + std::to_string(SCENARIO_SEED) + " max_path=" + std::to_string(TETHER_LENGTH);

Scenario performanceScenario(
    DebrisPattern pattern, int backgroundPerRobot = BACKGROUND_DEBRIS_PER_ROBOT,
    PathLayout layout = PathLayout::Uniform,
    WreckAvailability wreckAvailability = WreckAvailability::Reachable)
{
    return shipwreck_tests::makePerformanceScenario(
        SCENARIO_SEED, TETHER_LENGTH, backgroundPerRobot, pattern, layout, wreckAvailability);
}
}

TEST_F(Shipwreck, LargeClusteredDebrisLong)
{
    checkSearch(performanceScenario(DebrisPattern::Clustered), SCENARIO_LABEL);
}

TEST_F(Shipwreck, LargeMisleadingClustersLong)
{
    checkSearch(performanceScenario(DebrisPattern::Misleading), SCENARIO_LABEL);
}

TEST_F(Shipwreck, LargeWeakSignalLong)
{
    checkSearch(performanceScenario(DebrisPattern::WeakSignal), SCENARIO_LABEL);
}

TEST_F(Shipwreck, LargeSparseDebrisLong)
{
    checkSearch(performanceScenario(DebrisPattern::Sparse, 2), SCENARIO_LABEL);
}

TEST_F(Shipwreck, LargeUnequalPathsLong)
{
    checkSearch(performanceScenario(
        DebrisPattern::Misleading, BACKGROUND_DEBRIS_PER_ROBOT, PathLayout::Mixed), SCENARIO_LABEL);
}

TEST_F(Shipwreck, LargeUnfindableLong)
{
    checkSearch(performanceScenario(DebrisPattern::Clustered, BACKGROUND_DEBRIS_PER_ROBOT,
        PathLayout::Uniform, WreckAvailability::Unfindable), SCENARIO_LABEL);
}

