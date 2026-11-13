#include "search_test_fixture.h"

#include <algorithm>
#include <random>

namespace
{
  Scenario blockedScenario()
  {
    Scenario scenario;
    scenario.abysses.assign(DEFAULT_ROBOT_COUNT, 1);
    return scenario;
  }

}

TEST_F(Shipwreck, EveryRobot)
{
  for (int robot = 0; robot < DEFAULT_ROBOT_COUNT; ++robot)
  {
    SCOPED_TRACE(robot);
    Scenario scenario;
    scenario.abysses.assign(DEFAULT_ROBOT_COUNT, 202);
    scenario.shipwreck = Location{robot, 1 + robot % 201};
    checkSearch(scenario);
  }
}

TEST_F(Shipwreck, SimpleDebris)
{
  Scenario scenario;
  scenario.abysses.assign(DEFAULT_ROBOT_COUNT, 501);
  scenario.debris = {{0, 40}, {1, 80}, {1, 220}, {2, 300}};
  scenario.shipwreck = Location{1, 320};
  checkSearch(scenario);
}

TEST_F(Shipwreck, BeforeAbyss)
{
  for (int end : {1, 99, 100, 101, 299, 300, 301})
  {
    SCOPED_TRACE(end);
    Scenario scenario = blockedScenario();
    scenario.abysses[DEFAULT_ROBOT_COUNT - 1] = end + 1;
    scenario.shipwreck = Location{DEFAULT_ROBOT_COUNT - 1, end};
    checkSearch(scenario);
  }

  Scenario scenario = blockedScenario();
  scenario.abysses[0] = 75;
  scenario.shipwreck = Location{0, 50};
  checkSearch(scenario);
}

TEST_F(Shipwreck, BeyondAbyss)
{
  for (int abyss : {1, 100, 101, 301})
  {
    SCOPED_TRACE(abyss);
    Scenario scenario = blockedScenario();
    scenario.abysses[12] = abyss;
    scenario.abysses[73] = 252;
    scenario.shipwreck = Location{12, abyss + 1};
    scenario.debris = {{12, abyss + 2}, {73, 100}};
    checkSearch(scenario);
  }
}

TEST_F(Shipwreck, TetherBoundary)
{
  Scenario scenario = blockedScenario();
  scenario.abysses[DEFAULT_ROBOT_COUNT - 1] = TETHER_LENGTH + 1;
  scenario.debris = {{DEFAULT_ROBOT_COUNT - 1, TETHER_LENGTH - 1}};
  scenario.shipwreck = Location{DEFAULT_ROBOT_COUNT - 1, TETHER_LENGTH};
  checkSearch(scenario);
  scenario.shipwreck.reset();
  checkSearch(scenario);
}

TEST_F(Shipwreck, SeededScenarios)
{
  for (unsigned seed = 0; seed < 10; ++seed)
  {
    SCOPED_TRACE(seed);
    std::mt19937 rng(seed);
    Scenario scenario;
    for (int robot = 0; robot < DEFAULT_ROBOT_COUNT; ++robot)
    {
      scenario.abysses[robot] = 1 + rng() % 1200;
      for (int segment = 1; segment <= 1200; ++segment)
      {
        if (rng() % 80 == 0)
        {
          scenario.debris.push_back({robot, segment});
        }
      }
    }
    if (seed % 3 != 0)
    {
      const int robot = rng() % DEFAULT_ROBOT_COUNT;
      const int segment = 1 + rng() % 1200;
      scenario.shipwreck = Location{robot, segment};
      if (scenario.abysses[robot] == segment)
      {
        ++scenario.abysses[robot];
      }
      const auto isShipwreck = [robot, segment](Location debris)
      {
        return debris.robot == robot && debris.segment == segment;
      };
      scenario.debris.erase(std::remove_if(scenario.debris.begin(), scenario.debris.end(), isShipwreck),
                            scenario.debris.end());
    }
    checkSearch(scenario);
  }
}
