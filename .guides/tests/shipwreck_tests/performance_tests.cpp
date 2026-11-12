#include "search_test_fixture.h"
#include "performance_scenarios.h"

using shipwreck_tests::ScenarioSettings;

namespace
{
constexpr unsigned CLUSTERED_DEBRIS_SEED = 155920;
constexpr unsigned FALSE_LEADS_SEED = 155921;
constexpr unsigned WEAK_SIGNAL_SEED = 155922;
constexpr unsigned EXHAUSTIVE_SEARCH_SEED = 155925;
constexpr double WEAK_EVIDENCE_SCALE = 0.25;
constexpr Location WRECK_SITES[] = {
    {12, 30'000}, {37, 50'000}, {62, 30'000}, {87, 50'000},
};

template<class Check>
void checkWreckScenarios(const ScenarioSettings& settings, unsigned seed, Check check)
{
    unsigned scenarioIndex = 0;
    for (Location center : WRECK_SITES)
    {
        SCOPED_TRACE(scenarioIndex);
        check(shipwreck_tests::makePerformanceScenario(settings, seed + scenarioIndex, center));
        ++scenarioIndex;
    }
}
}

TEST_F(Shipwreck, ClusteredDebris)
{
    checkWreckScenarios(ScenarioSettings{}, CLUSTERED_DEBRIS_SEED,
        [this](const Scenario& scenario) { checkSearch(scenario); });
}

TEST_F(Shipwreck, StrongFalseLeads)
{
    ScenarioSettings settings;
    settings.formerWreckDebris = {{{0.03, 0.002}, {0.04, 0.003}, {0.05, 0.004}}};
    checkWreckScenarios(settings, FALSE_LEADS_SEED,
        [this](const Scenario& scenario) { checkSearch(scenario); });
}

TEST_F(Shipwreck, WeakWreckSignal)
{
    ScenarioSettings settings;
    settings.targetDebris = settings.targetDebris.scaled(WEAK_EVIDENCE_SCALE);
    for (auto& debris : settings.formerWreckDebris)
        debris = debris.scaled(WEAK_EVIDENCE_SCALE);
    checkWreckScenarios(settings, WEAK_SIGNAL_SEED,
        [this](const Scenario& scenario) { checkSearch(scenario); });
}

TEST_F(Shipwreck, ExhaustiveSearch)
{
    ScenarioSettings settings;
    settings.pathLength = 1'000'000;
    settings.backgroundProbability = 0.0001;
    checkSearch(shipwreck_tests::makePerformanceScenario(settings, EXHAUSTIVE_SEARCH_SEED));
}
