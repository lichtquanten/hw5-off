/*
 * PROVIDED CODE - DO NOT EDIT
 */

#include "search.h"
#include "search_driver.h"
#include "scenario.h"
#include "simulated_mothership.h"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
  enum class Selection
  {
    Sequential,
    StopEarly,
    Finished,
    Invalid
  };

  struct TestPolicy
  {
    Selection selection = Selection::Sequential;
    bool keepDriving = true;
    int selectionCalls = 0;
    int continuationCalls = 0;
    std::vector<DriveStopReason> recorded;
  } policy;

  Scenario emptyScenario()
  {
    Scenario scenario;
    scenario.abysses.fill(1);
    return scenario;
  }

  Scenario twoPaths()
  {
    Scenario scenario = emptyScenario();
    scenario.abysses[0] = 251;
    scenario.abysses[1] = 151;
    return scenario;
  }

  template <typename Exception, typename Function>
  void expectThrows(Function function)
  {
    bool threw = false;
    try
    {
      function();
    }
    catch (const Exception &)
    {
      threw = true;
    }
    assert(threw);
  }
}

// Test policies replace the student implementation when building this executable.
Search::Search(Mothership &mothership) : mothership(mothership) {}

std::optional<int> Search::chooseNextRobot()
{
  ++policy.selectionCalls;
  if (policy.selection == Selection::StopEarly)
  {
    return std::nullopt;
  }
  if (policy.selection == Selection::Finished)
  {
    return 0;
  }
  if (policy.selection == Selection::Invalid)
  {
    return ROBOT_COUNT;
  }
  for (int robot = 0; robot < ROBOT_COUNT; ++robot)
  {
    if (mothership.getRobotPosition(robot) < mothership.getAbysses()[robot] - 1)
    {
      return robot;
    }
  }
  return std::nullopt;
}

void Search::recordDriveResult(int, const DriveResult &result)
{
  policy.recorded.push_back(result.reason);
}

bool Search::shouldKeepDriving(int, const DriveResult &)
{
  ++policy.continuationCalls;
  return policy.keepDriving;
}

int main()
{
  for (DecisionCadence cadence : {DecisionCadence::Student, DecisionCadence::EveryDrive})
  {
    policy = {};
    SimulatedMothership mothership(twoPaths());
    Search search(mothership);
    const auto run = runSearch(search, mothership, cadence);
    assert(!run.shipwreck);
    assert(run.drives == 5);
    assert(policy.recorded.size() == 5);
    assert(mothership.allPathsExplored());
    assert(mothership.getElapsedTicks() == 410);
    const bool student = cadence == DecisionCadence::Student;
    assert(run.selections == (student ? 3 : 6));
    assert(run.continuationChecks == (student ? 3 : 0));
    assert(policy.selectionCalls == run.selections);
    assert(policy.continuationCalls == run.continuationChecks);
  }

  policy = {};
  policy.keepDriving = false;
  {
    SimulatedMothership mothership(twoPaths());
    Search search(mothership);
    const auto run = runSearch(search, mothership, DecisionCadence::Student);
    assert(run.selections == 6);
    assert(run.continuationChecks == 3);
  }

  policy = {};
  {
    Scenario scenario = twoPaths();
    scenario.shipwreck = Location{0, 250};
    SimulatedMothership mothership(scenario);
    Search search(mothership);
    const auto found = search.findShipwreck();
    assert(found && found->robot == 0 && found->segment == 250);
    assert(policy.recorded.size() == 3);
    assert(policy.recorded.back() == DriveStopReason::ShipwreckFound);
  }

  policy = {};
  {
    Scenario scenario = emptyScenario();
    scenario.abysses[0] = 101;
    scenario.debris.push_back({0, 100});
    SimulatedMothership mothership(scenario);
    Search search(mothership);
    assert(!search.findShipwreck());
    assert(policy.recorded.size() == 1);
    assert(policy.recorded.back() == DriveStopReason::DebrisFound);
    assert(policy.continuationCalls == 0);
    expectThrows<std::logic_error>([&] { (void)mothership.driveActiveRobot(); });
  }

  policy = {};
  {
    SimulatedMothership mothership(emptyScenario());
    Search search(mothership);
    const auto run = runSearch(search, mothership, DecisionCadence::EveryDrive);
    assert(run.drives == 0 && run.selections == 1);
  }

  for (Selection selection : {Selection::StopEarly, Selection::Invalid})
  {
    policy = {};
    policy.selection = selection;
    SimulatedMothership mothership(twoPaths());
    Search search(mothership);
    expectThrows<std::exception>([&] { (void)search.findShipwreck(); });
    assert(policy.recorded.empty());
  }

  policy = {};
  policy.selection = Selection::Finished;
  {
    SimulatedMothership mothership(emptyScenario());
    Search search(mothership);
    expectThrows<std::runtime_error>([&] { (void)search.findShipwreck(); });
  }

  policy = {};
  {
    SimulatedMothership mothership(twoPaths());
    Search search(mothership);
    expectThrows<std::runtime_error>([&]
    {
      (void)runSearch(search, mothership, DecisionCadence::EveryDrive, 1);
    });
    assert(policy.recorded.size() == 1);
  }

  std::cout << "PASS: search drivers, cadence, completion, and invalid selections.\n";
}
