#include "search_test_fixture.h"
#include "performance_scenarios.h"

using shipwreck_tests::makePerformanceScenario;
using shipwreck_tests::ScenarioSettings;
using shipwreck_tests::WRECK_SCENARIOS;
using shipwreck_tests::wreckCenter;

namespace
{
  constexpr unsigned CLUSTERED_DEBRIS_SEED = 155'000;
  constexpr unsigned FALSE_LEADS_SEED = 156'000;
  constexpr unsigned WEAK_SIGNAL_SEED = 157'000;
  constexpr unsigned SINGLE_APPROACH_SEED = 158'000;
  constexpr unsigned VARIED_SITES_SEED = 159'000;
  constexpr unsigned EXHAUSTIVE_SEARCH_SEED = 155'925;
  constexpr unsigned MANY_ROBOTS_SEED = 160'000;
  constexpr double WEAK_EVIDENCE_SCALE = 0.25;

  void checkWreckScenarios(Shipwreck &test, const ScenarioSettings &settings, unsigned seed)
  {
    for (int index = 0; index < WRECK_SCENARIOS; ++index)
    {
      SCOPED_TRACE(index);
      test.checkSearch(makePerformanceScenario(settings, seed + index, wreckCenter(settings, index)));
    }
  }
}

TEST_F(Shipwreck, ClusteredDebris)
{
  checkWreckScenarios(*this, ScenarioSettings{}, CLUSTERED_DEBRIS_SEED);
}

TEST_F(Shipwreck, StrongFalseLeads)
{
  ScenarioSettings settings;
  settings.formerWreckDebris = {{{0.03, 0.002}, {0.04, 0.003}, {0.05, 0.004}}};
  checkWreckScenarios(*this, settings, FALSE_LEADS_SEED);
}

TEST_F(Shipwreck, WeakWreckSignal)
{
  ScenarioSettings settings;
  settings.targetDebris = settings.targetDebris.scaled(WEAK_EVIDENCE_SCALE);
  for (auto &debris : settings.formerWreckDebris)
  {
    debris = debris.scaled(WEAK_EVIDENCE_SCALE);
  }
  checkWreckScenarios(*this, settings, WEAK_SIGNAL_SEED);
}

TEST_F(Shipwreck, SingleApproachTrail)
{
  ScenarioSettings settings;
  settings.singleApproachPath = true;
  checkWreckScenarios(*this, settings, SINGLE_APPROACH_SEED);
}

TEST_F(Shipwreck, VariedWreckSites)
{
  ScenarioSettings settings;
  settings.variedShapes = true;
  settings.minWreckDepth = 20'000;
  settings.maxWreckDepth = 120'000;
  settings.minSiteDepth = 10'000;
  settings.maxSiteDepth = 120'000;
  checkWreckScenarios(*this, settings, VARIED_SITES_SEED);
}

TEST_F(Shipwreck, ExhaustiveSearch)
{
  ScenarioSettings settings;
  settings.pathLength = 2'000'000;
  settings.backgroundProbability = 0.0001;
  checkSearch(makePerformanceScenario(settings, EXHAUSTIVE_SEARCH_SEED));
}

TEST_F(Shipwreck, ManyRobotsSearch)
{
  ScenarioSettings settings;
  settings.robotCount = 100'000;
  settings.pathLength = 10'000;
  settings.backgroundProbability = 0.0001;
  settings.minSiteDepth = 2'000;
  settings.maxSiteDepth = 8'000;
  checkSearch(makePerformanceScenario(settings, MANY_ROBOTS_SEED));
}
