#include "search.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "robot_neighbors.h"

double Search::PriorityComponents::weightedScore() const
{
    return (currentSignalStrength + aheadNeighborScore + unexploredNeighborhoodFraction) * progressMultiplier;
}

bool Search::RobotPriority::operator<(const RobotPriority& other) const
{
    if (score != other.score)
        return score > other.score;
    return robot < other.robot;
}

Search::Search(Mothership& mothership) : mothership(mothership), aheadDebris(mothership)
{
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
        refreshPriority(robot);
}

int Search::pathEnd(int robot) const
{
    return mothership.getAbysses()[robot] - 1;
}

bool Search::hasRemainingPath(int robot) const
{
    return mothership.getRobotPosition(robot) < pathEnd(robot);
}

int Search::currentSignalStrength(int robot) const
{
    const int position = mothership.getRobotPosition(robot);
    int signal = 0;
    for (int path : robotAndNeighbors(robot))
        signal += debrisHistory[path].countInRange(position - SIGNAL_RANGE, position + SIGNAL_RANGE);
    return signal;
}

double Search::unexploredNeighborhoodFraction(int robot) const
{
    const int first = mothership.getRobotPosition(robot) + 1;
    const int last = std::min(pathEnd(robot), first + SIGNAL_RANGE - 1);
    int available = 0;
    int examined = 0;
    for (int path : robotAndNeighbors(robot))
    {
        const int end = std::min(last, pathEnd(path));
        if (end < first)
            continue;
        available += end - first + 1;
        examined += std::max(0, std::min(end, mothership.getRobotPosition(path)) - first + 1);
    }
    return static_cast<double>(available - examined) / available;
}

double Search::progressMultiplier(int robot) const
{
    return std::sqrt(static_cast<double>(SIGNAL_RANGE) /
                     (SIGNAL_RANGE + mothership.getRobotPosition(robot)));
}

Search::PriorityComponents Search::measurePriority(int robot)
{
    return {static_cast<double>(currentSignalStrength(robot)), aheadDebris.bestNeighborBucketScore(robot),
            unexploredNeighborhoodFraction(robot), progressMultiplier(robot)};
}

void Search::setPriority(int robot, PriorityComponents components)
{
    auto entry = rankedRobots.extract({priorityComponents[robot].weightedScore(), robot});
    priorityComponents[robot] = components;
    if (!hasRemainingPath(robot))
    {
        aheadDebris.discardRobotCandidates(robot);
        return;
    }
    if (entry.empty())
        rankedRobots.insert({components.weightedScore(), robot});
    else
    {
        entry.value().score = components.weightedScore();
        rankedRobots.insert(std::move(entry));
    }
}

void Search::refreshPriority(int robot)
{
    setPriority(robot, hasRemainingPath(robot) ? measurePriority(robot) : PriorityComponents{});
}

void Search::refreshAffectedPriorities(int robot, int previousPosition, bool discoveredDebris)
{
    refreshPriority(robot);
    for (int neighbor : neighborsOf(robot))
    {
        if (discoveredDebris)
            refreshPriority(neighbor);
        else if (hasRemainingPath(neighbor))
        {
            const int frontier = mothership.getRobotPosition(neighbor);
            const int observedEnd = std::min({frontier + SIGNAL_RANGE, pathEnd(neighbor), pathEnd(robot)});
            if (previousPosition >= observedEnd || mothership.getRobotPosition(robot) <= frontier)
                continue;
            auto components = priorityComponents[neighbor];
            components.unexploredNeighborhoodFraction = unexploredNeighborhoodFraction(neighbor);
            setPriority(neighbor, components);
        }
    }
}

void Search::recordDiscovery(int robot)
{
    const int segment = mothership.getRobotPosition(robot);
    debrisHistory[robot].recordDiscovery(segment);
    aheadDebris.recordDiscovery(robot, segment);
}

void Search::discardPastDiscoveries(int path)
{
    int earliestSignal = std::numeric_limits<int>::max();
    for (int observer : robotAndNeighbors(path))
        if (hasRemainingPath(observer))
            earliestSignal = std::min(earliestSignal, mothership.getRobotPosition(observer) - SIGNAL_RANGE);
    debrisHistory[path].discardBefore(earliestSignal);
}

std::optional<Location> Search::findShipwreck()
{
    while (!rankedRobots.empty())
    {
        const int robot = rankedRobots.begin()->robot;
        mothership.switchToRobot(robot);
        const int previousPosition = mothership.getRobotPosition(robot);
        int moved = 0;
        bool discoveredDebris = false;
        while (hasRemainingPath(robot) && moved < SIGNAL_RANGE)
        {
            const DriveResult result = mothership.driveActiveRobot();
            moved += result.segmentsMoved;
            if (result.reason == DriveStopReason::ShipwreckFound)
                return Location{robot, mothership.getRobotPosition(robot)};
            if (result.reason == DriveStopReason::DebrisFound)
            {
                recordDiscovery(robot);
                discoveredDebris = true;
                break;
            }
        }
        for (int path : robotAndNeighbors(robot))
        {
            discardPastDiscoveries(path);
            aheadDebris.discardPassedBuckets(path);
        }
        refreshAffectedPriorities(robot, previousPosition, discoveredDebris);
    }
    return std::nullopt;
}
