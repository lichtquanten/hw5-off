#pragma once

#include <gtest/gtest.h>

#include "scenario.h"
#include "search.h"
#include "simulated_mothership.h"

#include <exception>
#include <iostream>

class Shipwreck : public ::testing::Test
{
public:
  void checkSearch(const Scenario &scenario)
  {
    const bool reachable = scenario.shipwreck &&
                           scenario.shipwreck->segment < scenario.abysses[scenario.shipwreck->robot];
    ::testing::Message expected;
    if (reachable)
    {
      expected << "Expected: shipwreck at robot " << scenario.shipwreck->robot
               << ", segment " << scenario.shipwreck->segment;
    }
    else
    {
      expected << "Expected: no reachable shipwreck";
    }
    SCOPED_TRACE(expected.GetString());

    SimulatedMothership mothership(scenario);
    SearchResult actual{false, Location{}};
    try
    {
      Search search(mothership);
      actual = search.findShipwreck();
    }
    catch (const std::exception &error)
    {
      FAIL() << "Search stopped with error: " << error.what();
    }
    ++scenarioCount;
    totalTicks += mothership.getElapsedTicks();

    ::testing::Message returned;
    returned << "Returned: isFound = " << (actual.isFound ? "true" : "false");
    if (actual.isFound)
    {
      returned << ", robot " << actual.location.robot
               << ", segment " << actual.location.segment;
    }
    SCOPED_TRACE(returned.GetString());

    if (actual.isFound != reachable)
    {
      FAIL() << "The returned result does not match the expected outcome.";
    }
    if (!reachable)
    {
      EXPECT_TRUE(mothership.allPathsExplored())
          << "Search stopped while reachable paths remained.";
      return;
    }
    ASSERT_TRUE(mothership.getDiscoveredShipwreck().has_value())
        << "The search must discover the shipwreck before reporting it.";
    if (actual.location.robot != scenario.shipwreck->robot ||
        actual.location.segment != scenario.shipwreck->segment)
    {
      ADD_FAILURE() << "The search returned the wrong shipwreck location.";
    }
  }

protected:
  void TearDown() override
  {
    if (scenarioCount == 0)
    {
      return;
    }
    std::cout << "[ TICKS    ] scenarios=" << scenarioCount
              << " total=" << totalTicks << '\n';
  }

private:
  int scenarioCount = 0;
  long long totalTicks = 0;
};
