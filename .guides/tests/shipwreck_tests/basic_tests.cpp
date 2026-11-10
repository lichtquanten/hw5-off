#include "search_test_fixture.h"

#include <algorithm>
#include <random>

TEST_F(Shipwreck, NoReachablePaths)
{
    checkSearch(blockedScenario());
}

TEST_F(Shipwreck, EmptyUnequalPaths)
{
    Scenario scenario;
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
        scenario.abysses[robot] = 1 + (robot * 137) % 1200;
    checkSearch(scenario);
}

TEST_F(Shipwreck, EveryRobot)
{
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        SCOPED_TRACE(robot);
        Scenario scenario;
        scenario.abysses.fill(202);
        scenario.shipwreck = Location{robot, 1 + robot % 201};
        checkSearch(scenario);
    }
}

TEST_F(Shipwreck, DriveBoundaries)
{
    for (int segment : {1, MAX_SEGMENTS_PER_DRIVE - 1, MAX_SEGMENTS_PER_DRIVE,
                        MAX_SEGMENTS_PER_DRIVE + 1, 2 * MAX_SEGMENTS_PER_DRIVE,
                        2 * MAX_SEGMENTS_PER_DRIVE + 1})
    {
        SCOPED_TRACE(segment);
        Scenario scenario = blockedScenario();
        scenario.abysses[37] = segment + 2;
        scenario.shipwreck = Location{37, segment};
        checkSearch(scenario);
    }
}

TEST_F(Shipwreck, BeforeAbyss)
{
    for (int end : {1, 99, 100, 101, 299, 300, 301})
    {
        SCOPED_TRACE(end);
        Scenario scenario = blockedScenario();
        scenario.abysses[ROBOT_COUNT - 1] = end + 1;
        scenario.shipwreck = Location{ROBOT_COUNT - 1, end};
        checkSearch(scenario);
    }
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

TEST_F(Shipwreck, DebrisAdjacentToWreck)
{
    for (int segment : {2, 100, 101, 500, 501})
    {
        SCOPED_TRACE(segment);
        Scenario scenario = blockedScenario();
        scenario.abysses[51] = segment + 3;
        scenario.shipwreck = Location{51, segment};
        scenario.debris = {{51, segment + 1}, {51, segment - 1}};
        checkSearch(scenario);
    }
}

TEST_F(Shipwreck, DenseUnsortedDebris)
{
    Scenario scenario = blockedScenario();
    scenario.abysses[8] = 702;
    for (int segment = 700; segment >= 1; --segment)
        scenario.debris.push_back({8, segment});
    scenario.shipwreck = Location{8, 701};
    checkSearch(scenario);
    scenario.shipwreck.reset();
    checkSearch(scenario);
}

TEST_F(Shipwreck, TetherBoundary)
{
    Scenario scenario = blockedScenario();
    scenario.abysses[ROBOT_COUNT - 1] = TETHER_LENGTH + 1;
    scenario.debris = {{ROBOT_COUNT - 1, TETHER_LENGTH - 1}};
    scenario.shipwreck = Location{ROBOT_COUNT - 1, TETHER_LENGTH};
    checkSearch(scenario);
    scenario.shipwreck.reset();
    checkSearch(scenario);
}

TEST_F(Shipwreck, FullFleetNoWreck)
{
    Scenario scenario;
    scenario.abysses.fill(1002);
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
        for (int segment : {1, 99, 100, 101, 499, 500, 501, 1001})
            scenario.debris.push_back({robot, segment});
    checkSearch(scenario);
}

TEST_F(Shipwreck, IndependentSearches)
{
    Scenario scenario = blockedScenario();
    scenario.abysses[3] = 152;
    scenario.shipwreck = Location{3, 151};
    checkSearch(scenario);
    checkSearch(blockedScenario());
    scenario.shipwreck = Location{3, 1};
    checkSearch(scenario);
}

TEST_F(Shipwreck, SeededScenarios)
{
    for (unsigned seed = 0; seed < 80; ++seed)
    {
        SCOPED_TRACE(seed);
        std::mt19937 rng(seed);
        Scenario scenario;
        for (int robot = 0; robot < ROBOT_COUNT; ++robot)
        {
            scenario.abysses[robot] = 1 + rng() % 1200;
            for (int segment = 1; segment <= 1200; ++segment)
                if (rng() % 80 == 0)
                    scenario.debris.push_back({robot, segment});
        }
        if (seed % 3 != 0)
        {
            const int robot = rng() % ROBOT_COUNT;
            const int segment = 1 + rng() % 1200;
            scenario.shipwreck = Location{robot, segment};
            if (scenario.abysses[robot] == segment)
                ++scenario.abysses[robot];
            scenario.debris.erase(std::remove_if(scenario.debris.begin(), scenario.debris.end(),
                [robot, segment](Location debris) {
                    return debris.robot == robot && debris.segment == segment;
                }), scenario.debris.end());
        }
        std::shuffle(scenario.debris.begin(), scenario.debris.end(), rng);
        checkSearch(scenario);
    }
}
