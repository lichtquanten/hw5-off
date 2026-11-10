#pragma once

#include <gtest/gtest.h>

#include "search.h"
#include "scenario.h"
#include "simulated_mothership.h"

#include <iostream>

namespace
{
Scenario blockedScenario()
{
    Scenario scenario;
    scenario.abysses.fill(1);
    return scenario;
}

class Shipwreck : public ::testing::Test
{
protected:
    int scenarioCount = 0;
    long long totalTicks = 0;

    void TearDown() override
    {
        if (scenarioCount == 0)
            return;
        std::cout << "[ TICKS    ] scenarios=" << scenarioCount
                  << " total=" << totalTicks << '\n';
    }

    void checkSearch(const Scenario& scenario)
    {
        const bool reachable = scenario.shipwreck &&
            scenario.shipwreck->segment < scenario.abysses[scenario.shipwreck->robot];
        SimulatedMothership mothership(scenario);
        Search search(mothership);
        const auto actual = search.findShipwreck();
        ++scenarioCount;
        totalTicks += mothership.getElapsedTicks();
        const auto discovered = mothership.getDiscoveredShipwreck();

        ASSERT_EQ(actual.has_value(), reachable);
        ASSERT_EQ(discovered.has_value(), reachable);
        if (reachable)
        {
            EXPECT_EQ(actual->robot, scenario.shipwreck->robot);
            EXPECT_EQ(actual->segment, scenario.shipwreck->segment);
            EXPECT_EQ(discovered->robot, scenario.shipwreck->robot);
            EXPECT_EQ(discovered->segment, scenario.shipwreck->segment);
        }
        else
        {
            EXPECT_TRUE(mothership.allPathsExplored());
        }
    }
};
}
