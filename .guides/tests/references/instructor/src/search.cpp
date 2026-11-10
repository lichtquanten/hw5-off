#include "search.h"

#include <algorithm>
#include <utility>

namespace
{
constexpr int MAX_SIGNAL_BONUS_COUNT = 32;
constexpr int DISTANCE_BONUS_PER_DEBRIS = SIGNAL_RANGE / 4;
constexpr int EXPLORATION_BATCH_LENGTH = SIGNAL_RANGE;
constexpr std::size_t MAX_QUEUE_ENTRIES = 4 * ROBOT_COUNT;

std::array<int, 2> neighborsOf(int robot)
{
    return {(robot + ROBOT_COUNT - 1) % ROBOT_COUNT, (robot + 1) % ROBOT_COUNT};
}

std::array<int, 3> robotAndNeighbors(int robot)
{
    const auto neighbors = neighborsOf(robot);
    return {neighbors[0], robot, neighbors[1]};
}
}

bool Search::Candidate::operator>(const Candidate& other) const
{
    if (priority != other.priority)
        return priority > other.priority;
    return robot > other.robot;
}

Search::Search(Mothership& mothership) : mothership(mothership)
{
    rebuildQueue();
}

bool Search::hasReachableSegments(int robot) const
{
    return mothership.getRobotPosition(robot) < mothership.getAbysses()[robot] - 1;
}

long long Search::calculatePriority(int robot) const
{
    const long long position = mothership.getRobotPosition(robot);
    int signal = 0;
    for (int path : robotAndNeighbors(robot))
        signal += debris[path].count(position - SIGNAL_RANGE, position + SIGNAL_RANGE);

    const int signalBonus = std::min(signal, MAX_SIGNAL_BONUS_COUNT);
    return position - static_cast<long long>(signalBonus) * DISTANCE_BONUS_PER_DEBRIS;
}

std::optional<int> Search::takeNextRobot()
{
    while (!candidates.empty())
    {
        const Candidate next = candidates.top();
        candidates.pop();
        auto& schedule = schedules[next.robot];
        if (next.version != schedule.version || !hasReachableSegments(next.robot))
            continue;
        schedule.hasQueuedCandidate = false;
        return next.robot;
    }
    return std::nullopt;
}

void Search::updatePriority(int robot)
{
    auto& schedule = schedules[robot];
    if (!hasReachableSegments(robot))
    {
        if (schedule.hasQueuedCandidate)
            ++schedule.version;
        schedule.hasQueuedCandidate = false;
        return;
    }
    const long long priority = calculatePriority(robot);
    if (schedule.hasQueuedCandidate && schedule.priority == priority)
        return;

    ++schedule.version;
    schedule.priority = priority;
    schedule.hasQueuedCandidate = true;
    candidates.push({priority, robot, schedule.version});
}

void Search::rebuildQueue()
{
    std::vector<Candidate> current;
    current.reserve(ROBOT_COUNT);
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        auto& schedule = schedules[robot];
        schedule.hasQueuedCandidate = hasReachableSegments(robot);
        if (schedule.hasQueuedCandidate)
        {
            schedule.priority = calculatePriority(robot);
            current.push_back({schedule.priority, robot, schedule.version});
        }
    }
    candidates = CandidateQueue(std::greater<Candidate>(), std::move(current));
}

void Search::recordDebris(int robot)
{
    const int segment = mothership.getRobotPosition(robot);
    debris[robot].recordDiscovery(segment);
    for (int neighbor : neighborsOf(robot))
    {
        const long long center = mothership.getRobotPosition(neighbor);
        if (segment >= center - SIGNAL_RANGE && segment <= center + SIGNAL_RANGE)
            updatePriority(neighbor);
    }
}

std::optional<long long> Search::earliestRelevantSegment(int path) const
{
    const auto& abysses = mothership.getAbysses();
    std::optional<long long> earliest;
    for (int observer : robotAndNeighbors(path))
    {
        const int position = mothership.getRobotPosition(observer);
        if (position >= abysses[observer] - 1)
            continue;
        const long long lower = static_cast<long long>(position) - SIGNAL_RANGE;
        if (!earliest || lower < *earliest)
            earliest = lower;
    }
    return earliest;
}

void Search::discardObsoleteDebris(int observer)
{
    for (int path : robotAndNeighbors(observer))
    {
        if (debris[path].size() == 0)
            continue;
        const auto earliest = earliestRelevantSegment(path);
        if (earliest)
            debris[path].discardBefore(*earliest);
        else
            debris[path].discardAll();
    }
}

std::optional<Location> Search::findShipwreck()
{
    while (const auto next = takeNextRobot())
    {
        const int robot = *next;
        mothership.switchToRobot(robot);
        int moved = 0;
        while (hasReachableSegments(robot) && moved < EXPLORATION_BATCH_LENGTH)
        {
            const DriveResult result = mothership.driveActiveRobot();
            moved += result.segmentsMoved;
            if (result.reason == DriveStopReason::ShipwreckFound)
                return Location{robot, mothership.getRobotPosition(robot)};
            if (result.reason == DriveStopReason::DebrisFound)
            {
                recordDebris(robot);
                break;
            }
        }
        discardObsoleteDebris(robot);
        updatePriority(robot);
        if (candidates.size() > MAX_QUEUE_ENTRIES)
            rebuildQueue();
    }
    return std::nullopt;
}
