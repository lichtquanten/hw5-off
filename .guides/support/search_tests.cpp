/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "search.h"
#include "scenario.h"
#include "simulated_mothership.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace
{
  Scenario emptyScenario()
  {
    Scenario scenario;
    scenario.abysses.fill(1);
    return scenario;
  }

  void checkSearch(const Scenario &scenario, bool expectShipwreck)
  {
    SimulatedMothership mothership(scenario);
    Search search(mothership);
    const auto result = search.findShipwreck();
    const auto discovered = mothership.getDiscoveredShipwreck();

    assert(result.has_value() == expectShipwreck);
    if (result)
    {
      assert(discovered);
      assert(result->robot == discovered->robot);
      assert(result->segment == discovered->segment);
    }
    else
    {
      assert(!discovered);
      assert(mothership.allPathsExplored());
    }
  }
}

int main()
{
  checkSearch(emptyScenario(), false);

  Scenario scenario = emptyScenario();
  scenario.abysses[0] = 251;
  scenario.abysses[1] = 151;
  checkSearch(scenario, false);

  scenario.shipwreck = Location{1, 150};
  checkSearch(scenario, true);

  scenario = emptyScenario();
  scenario.abysses[0] = 101;
  scenario.debris = {{0, 100}};
  checkSearch(scenario, false);

  scenario = emptyScenario();
  scenario.abysses[ROBOT_COUNT - 1] = 301;
  scenario.debris = {{ROBOT_COUNT - 1, 80}, {ROBOT_COUNT - 1, 220}};
  scenario.shipwreck = Location{ROBOT_COUNT - 1, 300};
  checkSearch(scenario, true);

  scenario.shipwreck = Location{ROBOT_COUNT - 1, 301};
  scenario.abysses[ROBOT_COUNT - 1] = 302;
  scenario.debris.push_back({ROBOT_COUNT - 1, 300});
  checkSearch(scenario, true);

  scenario = emptyScenario();
  scenario.abysses[0] = TETHER_LENGTH + 1;
  checkSearch(scenario, false);

  SimulatedMothership blocked(emptyScenario());
  bool rejected = false;
  try
  {
    (void)blocked.driveActiveRobot();
  }
  catch (const std::logic_error &)
  {
    rejected = true;
  }
  assert(rejected);

  std::cout << "PASS: search correctness, path limits, and debris stops.\n";
}
