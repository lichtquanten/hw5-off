/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "simulated_mothership.h"
#include "scenario.h"
#include "search.h"

#include <chrono>
#include <exception>
#include <iomanip>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

namespace
{

  Scenario makeDemoScenario()
  {
    Scenario scenario;

    scenario.abysses.fill(1'001);

    scenario.debris = {
        {0, 80},
        {0, 220},
        {1, 260},
        {ROBOT_COUNT - 1, 280},
    };
    scenario.shipwreck = Location{0, 300};

    return scenario;
  }

  Scenario makeBenchmarkScenario()
  {
    Scenario scenario;
    scenario.abysses.fill(30'001);
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
      for (int segment = 50; segment < 30'000; segment += 100)
      {
        scenario.debris.push_back({robot, segment});
      }
    }
    return scenario;
  }

  void verifySearchResult(const SimulatedMothership &mothership,
                          std::optional<Location> result)
  {
    const auto discovered = mothership.getDiscoveredShipwreck();

    if (result)
    {
      if (!discovered || result->robot != discovered->robot ||
          result->segment != discovered->segment)
      {
        throw std::runtime_error(
            "The returned location was not a discovered shipwreck.");
      }
      return;
    }

    if (discovered)
    {
      throw std::runtime_error(
          "The search found the wreck but returned nullopt.");
    }

    if (!mothership.allPathsExplored())
    {
      throw std::runtime_error(
          "The search stopped before exploring every reachable path.");
    }
  }

}

int main(int argc, char **argv)
{
  try
  {
    const bool benchmark = argc == 2 && std::string(argv[1]) == "--benchmark";
    if (argc > 1 && !benchmark)
    {
      throw std::invalid_argument("Usage: shipwreck [--benchmark]");
    }
    const Scenario scenario = benchmark ? makeBenchmarkScenario() : makeDemoScenario();
    SimulatedMothership mothership(scenario);

    const auto searchStart = std::chrono::steady_clock::now();
    Search search(mothership);
    const auto result = search.findShipwreck();
    const auto searchTime = std::chrono::steady_clock::now() - searchStart;
    verifySearchResult(mothership, result);

    if (result)
    {
      std::cout << "PASS: found shipwreck at robot " << result->robot
                << ", segment " << result->segment << '\n';
    }
    else
    {
      std::cout << "PASS: no reachable shipwreck.\n";
    }

    std::cout << "Ticks: " << mothership.getElapsedTicks() << '\n'
              << std::fixed << std::setprecision(3)
              << "Runtime including initialization (ms): "
              << std::chrono::duration<double, std::milli>(searchTime).count() << '\n';
    return 0;
  }
  catch (const std::exception &error)
  {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
