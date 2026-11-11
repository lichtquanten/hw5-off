#pragma once

#include <array>
#include <optional>
#include <set>

#include "ahead_debris.h"
#include "debris_history.h"
#include "mothership.h"

class Search
{
public:
    explicit Search(Mothership& mothership);
    std::optional<Location> findShipwreck();

private:
    struct PriorityComponents
    {
        double currentSignalStrength = 0;
        double aheadNeighborScore = 0;
        double unexploredNeighborhoodFraction = 0;
        double progressMultiplier = 0;
        double weightedScore() const;
    };

    struct RobotPriority
    {
        double score;
        int robot;
        bool operator<(const RobotPriority& other) const;
    };

    Mothership& mothership;
    std::array<DebrisHistory, ROBOT_COUNT> debrisHistory;
    AheadDebris aheadDebris;
    std::array<PriorityComponents, ROBOT_COUNT> priorityComponents{};
    std::set<RobotPriority> rankedRobots;

    int pathEnd(int robot) const;
    bool hasRemainingPath(int robot) const;
    int currentSignalStrength(int robot) const;
    double unexploredNeighborhoodFraction(int robot) const;
    double progressMultiplier(int robot) const;
    PriorityComponents measurePriority(int robot);
    void setPriority(int robot, PriorityComponents components);
    void refreshPriority(int robot);
    void refreshAffectedPriorities(int robot, int previousPosition, bool discoveredDebris);
    void recordDiscovery(int robot);
    void discardPastDiscoveries(int path);
};
