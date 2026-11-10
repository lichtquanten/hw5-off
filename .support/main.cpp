/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "simulated_mothership.h"
#include "scenario.h"
#include "search.h"
#include "search_driver.h"

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

    const auto initializationStart = std::chrono::steady_clock::now();
    Search search(mothership);
    const auto initializationTime = std::chrono::steady_clock::now() - initializationStart;

    if (benchmark)
    {
      const SearchRun run = runSearch(search, mothership, DecisionCadence::EveryDrive,
                                      1'000'000);
      verifySearchResult(mothership, run.shipwreck);

      const double initializationNs =
          std::chrono::duration<double, std::nano>(initializationTime).count();
      const double selectionNs = static_cast<double>(run.selectionTime.count());
      const double recordingNs = static_cast<double>(run.recordingTime.count());
      std::cout << "PASS: benchmark completed every reachable path.\n"
                << "Drives: " << run.drives << '\n'
                << "Selections: " << run.selections << '\n'
                << std::fixed << std::setprecision(2)
                << "Initialization (us): " << initializationNs / 1'000 << '\n'
                << "Selection (ns/call): " << selectionNs / run.selections << '\n'
                << "Recording (ns/call): " << recordingNs / run.drives << '\n'
                << "Update + selection (ns/drive): "
                << (selectionNs + recordingNs) / run.drives << '\n'
                << "Student time including initialization (ms): "
                << (initializationNs + selectionNs + recordingNs) / 1'000'000 << '\n';
      return 0;
    }

    const auto result = search.findShipwreck();
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

    std::cout << "Ticks: " << mothership.getElapsedTicks() << '\n';
    return 0;
  }
  catch (const std::exception &error)
  {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
