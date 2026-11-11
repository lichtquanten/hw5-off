#include "search_test_fixture.h"

#include <algorithm>
#include <limits>
#include <random>
#include <stdexcept>

namespace
{
void checkRequestedDriveDistances()
{
    Scenario scenario;
    SimulatedMothership mothership(scenario);
    Mothership& controls = mothership;
    int position = 0;
    for (int distance : {1, 37, 99, MAX_SEGMENTS_PER_DRIVE})
    {
        const auto result = controls.driveActiveRobot(distance);
        position += distance;
        EXPECT_EQ(result.segmentsMoved, distance);
        EXPECT_EQ(result.reason, DriveStopReason::DriveLimitReached);
        EXPECT_EQ(controls.getRobotPosition(0), position);
        EXPECT_EQ(controls.getElapsedTicks(), position);
    }

    for (int distance : {-1, 0, MAX_SEGMENTS_PER_DRIVE + 1,
                         std::numeric_limits<int>::max()})
    {
        EXPECT_THROW((void)controls.driveActiveRobot(distance), std::invalid_argument);
        EXPECT_EQ(controls.getRobotPosition(0), position);
        EXPECT_EQ(controls.getElapsedTicks(), position);
    }

    const auto result = controls.driveActiveRobot();
    EXPECT_EQ(result.segmentsMoved, MAX_SEGMENTS_PER_DRIVE);
    EXPECT_EQ(controls.getRobotPosition(0), position + MAX_SEGMENTS_PER_DRIVE);
    EXPECT_EQ(controls.getElapsedTicks(), position + MAX_SEGMENTS_PER_DRIVE);
}

void checkAbyssDrives()
{
    Scenario scenario;
    scenario.abysses[0] = 75;

    // A request that reaches or crosses the abyss must fail, rather than clamp.
    for (int distance : {75, MAX_SEGMENTS_PER_DRIVE})
    {
        SimulatedMothership mothership(scenario);
        EXPECT_THROW((void)mothership.driveActiveRobot(distance), std::runtime_error);
        EXPECT_FALSE(mothership.getDiscoveredShipwreck());
    }

    // Safety is relative to the robot's current position.
    SimulatedMothership safe(scenario);
    EXPECT_EQ(safe.driveActiveRobot(50).segmentsMoved, 50);
    const auto lastSafeDrive = safe.driveActiveRobot(24);
    EXPECT_EQ(lastSafeDrive.segmentsMoved, 24);
    EXPECT_EQ(lastSafeDrive.reason, DriveStopReason::PathLimitReached);
    EXPECT_EQ(safe.getRobotPosition(0), 74);
    EXPECT_EQ(safe.getElapsedTicks(), 74);
    EXPECT_THROW((void)safe.driveActiveRobot(1), std::runtime_error);

    // A wreck before the abyss stops even a request that extends beyond it.
    scenario.shipwreck = Location{0, 50};
    for (int distance : {74, MAX_SEGMENTS_PER_DRIVE})
    {
        SimulatedMothership mothership(scenario);
        const auto result = mothership.driveActiveRobot(distance);
        EXPECT_EQ(result.reason, DriveStopReason::ShipwreckFound);
        EXPECT_EQ(result.segmentsMoved, 50);
        EXPECT_EQ(mothership.getRobotPosition(0), 50);
        EXPECT_EQ(mothership.getElapsedTicks(), 50);
        ASSERT_TRUE(mothership.getDiscoveredShipwreck());
        EXPECT_EQ(mothership.getDiscoveredShipwreck()->segment, 50);
    }

    // Debris also stops early, but cannot protect a subsequent unsafe drive.
    scenario.shipwreck.reset();
    scenario.debris = {{0, 50}};
    SimulatedMothership debris(scenario);
    const auto discovery = debris.driveActiveRobot();
    EXPECT_EQ(discovery.reason, DriveStopReason::DebrisFound);
    EXPECT_EQ(discovery.segmentsMoved, 50);
    EXPECT_THROW((void)debris.driveActiveRobot(25), std::runtime_error);

    // Discoveries on or beyond the abyss cannot rescue an unsafe request.
    for (int debrisSegment : {75, 80})
    {
        scenario.debris = {{0, debrisSegment}};
        scenario.shipwreck = Location{0, 90};
        SimulatedMothership mothership(scenario);
        EXPECT_THROW((void)mothership.driveActiveRobot(), std::runtime_error);
        EXPECT_FALSE(mothership.getDiscoveredShipwreck());
    }

    scenario.abysses[0] = 1;
    SimulatedMothership blocked(scenario);
    EXPECT_THROW((void)blocked.driveActiveRobot(1), std::runtime_error);
}

void checkTetherDrives()
{
    for (int discovery : {0, 1, 2})
    {
        Scenario scenario;
        if (discovery == 1)
            scenario.debris = {{0, TETHER_LENGTH}};
        if (discovery == 2)
            scenario.shipwreck = Location{0, TETHER_LENGTH};
        SimulatedMothership mothership(scenario);
        while (mothership.getRobotPosition(0) < TETHER_LENGTH - 1)
        {
            const int remaining = TETHER_LENGTH - 1 - mothership.getRobotPosition(0);
            (void)mothership.driveActiveRobot(std::min(MAX_SEGMENTS_PER_DRIVE, remaining));
        }

        if (discovery == 0)
        {
            EXPECT_THROW((void)mothership.driveActiveRobot(2), std::runtime_error);
            const auto result = mothership.driveActiveRobot(1);
            EXPECT_EQ(result.reason, DriveStopReason::PathLimitReached);
            EXPECT_EQ(result.segmentsMoved, 1);
        }
        else
        {
            const auto result = mothership.driveActiveRobot();
            EXPECT_EQ(result.reason, discovery == 1 ? DriveStopReason::DebrisFound
                                                   : DriveStopReason::ShipwreckFound);
            EXPECT_EQ(result.segmentsMoved, 1);
        }

        EXPECT_EQ(mothership.getRobotPosition(0), TETHER_LENGTH);
        EXPECT_EQ(mothership.getElapsedTicks(), TETHER_LENGTH);
        EXPECT_THROW((void)mothership.driveActiveRobot(1), std::runtime_error);
    }
}
}

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
    checkRequestedDriveDistances();
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
    checkAbyssDrives();
    for (int end : {1, 99, 100, 101, 299, 300, 301})
    {
        SCOPED_TRACE(end);
        Scenario scenario = blockedScenario();
        scenario.abysses[ROBOT_COUNT - 1] = end + 1;
        scenario.shipwreck = Location{ROBOT_COUNT - 1, end};
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
    checkTetherDrives();
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
