#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <queue>
#include <vector>

#include "debris_history.h"
#include "mothership.h"

class Search
{
public:
    explicit Search(Mothership& mothership);
    std::optional<Location> findShipwreck();

private:
    struct Candidate
    {
        long long priority;
        int robot;
        std::uint64_t version;
        bool operator>(const Candidate& other) const;
    };

    struct RobotSchedule
    {
        long long priority = 0;
        std::uint64_t version = 0;
        bool hasQueuedCandidate = false;
    };

    using CandidateQueue = std::priority_queue<Candidate, std::vector<Candidate>, std::greater<Candidate>>;

    Mothership& mothership;
    std::array<DebrisHistory, ROBOT_COUNT> debris;
    std::array<RobotSchedule, ROBOT_COUNT> schedules{};
    CandidateQueue candidates;

    bool hasReachableSegments(int robot) const;
    long long calculatePriority(int robot) const;
    std::optional<int> takeNextRobot();
    void updatePriority(int robot);
    void rebuildQueue();
    void recordDebris(int robot);
    std::optional<long long> earliestRelevantSegment(int path) const;
    void discardObsoleteDebris(int observer);
};
