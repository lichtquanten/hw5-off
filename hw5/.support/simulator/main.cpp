#include "simulated_mothership.h"
#include "scenario.h"
#include "search.h"

#include <exception>
#include <iostream>
#include <stdexcept>

namespace
{

  Scenario makeDemoScenario()
  {
    Scenario scenario;

    scenario.abysses.assign(DEFAULT_ROBOT_COUNT, 1'001);

    scenario.debris = {
        {0, 80},
        {0, 220},
        {1, 260},
        {DEFAULT_ROBOT_COUNT - 1, 280},
    };
    scenario.shipwreck = Location{0, 300};

    return scenario;
  }

}

int main()
{
  try
  {
    const Scenario scenario = makeDemoScenario();
    SimulatedMothership mothership(scenario);

    Search search(mothership);
    const auto result = search.findShipwreck();
    const auto discovered = mothership.getDiscoveredShipwreck();

    if (!result.isFound || !discovered ||
        result.location.robot != discovered->robot ||
        result.location.segment != discovered->segment)
    {
      throw std::runtime_error(
          "The search did not return a discovered shipwreck.");
    }

    std::cout << "PASS: found shipwreck at robot " << result.location.robot
              << ", segment " << result.location.segment << '\n';

    return 0;
  }
  catch (const std::exception &error)
  {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
