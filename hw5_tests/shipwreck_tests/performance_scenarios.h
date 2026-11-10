#pragma once

#include "scenario.h"

#include <algorithm>
#include <random>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace shipwreck_tests
{
enum class DebrisPattern { Clustered, Misleading, WeakSignal, Sparse };
enum class PathLayout { Uniform, Mixed };
enum class WreckAvailability { Reachable, Unfindable };

struct DebrisDistribution
{
    int wreckClusterSamples = 240;
    int decoyClusterCount = 16;
    int decoyClusterSamples = 120;
};

inline DebrisDistribution debrisDistribution(DebrisPattern pattern)
{
    DebrisDistribution distribution;
    switch (pattern)
    {
    case DebrisPattern::Clustered:
        break;
    case DebrisPattern::Misleading:
        distribution.decoyClusterSamples = 600;
        break;
    case DebrisPattern::WeakSignal:
        distribution.wreckClusterSamples = 8;
        break;
    case DebrisPattern::Sparse:
        distribution.wreckClusterSamples = 4;
        distribution.decoyClusterCount = 2;
        distribution.decoyClusterSamples = 4;
        break;
    default:
        throw std::invalid_argument("Unknown debris pattern.");
    }
    return distribution;
}

inline void addDebrisCluster(Scenario& scenario, std::mt19937& random,
                            Location center, int sampleCount)
{
    for (int sample = 0; sample < sampleCount; ++sample)
    {
        const int neighborOffset = static_cast<int>(random() % 3) - 1;
        const int neighbor = (center.robot + ROBOT_COUNT + neighborOffset) % ROBOT_COUNT;
        const int segmentOffset = static_cast<int>(random() % (2 * SIGNAL_RANGE + 1)) - SIGNAL_RANGE;
        const int segment = center.segment + segmentOffset;
        if (segment >= 1 && segment < scenario.abysses[neighbor])
            scenario.debris.push_back({neighbor, segment});
    }
}

inline void normalizeDebris(Scenario& scenario, std::mt19937& random)
{
    auto& debris = scenario.debris;
    debris.erase(std::remove_if(debris.begin(), debris.end(), [&](Location location)
    {
        return scenario.shipwreck && location.robot == scenario.shipwreck->robot &&
               location.segment == scenario.shipwreck->segment;
    }), debris.end());
    std::sort(debris.begin(), debris.end(), [](Location left, Location right)
    {
        return std::tie(left.robot, left.segment) < std::tie(right.robot, right.segment);
    });
    debris.erase(std::unique(debris.begin(), debris.end(), [](Location left, Location right)
    {
        return left.robot == right.robot && left.segment == right.segment;
    }), debris.end());
    for (std::size_t remaining = debris.size(); remaining > 1; --remaining)
        std::swap(debris[remaining - 1], debris[random() % remaining]);
}

inline Scenario makePerformanceScenario(
    unsigned seed, int maxPathLength, int backgroundPerRobot, DebrisPattern pattern,
    PathLayout layout = PathLayout::Uniform,
    WreckAvailability wreckAvailability = WreckAvailability::Reachable)
{
    if (maxPathLength < 1 || maxPathLength > TETHER_LENGTH || backgroundPerRobot < 0)
        throw std::invalid_argument("Invalid performance scenario dimensions.");
    std::mt19937 random(seed);
    const auto pick = [&random](int count) { return static_cast<int>(random() % count); };
    const DebrisDistribution distribution = debrisDistribution(pattern);
    Scenario scenario;
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        int length = maxPathLength;
        if (layout == PathLayout::Mixed)
        {
            if (robot % 7 == 0)
                length = 0;
            else if (robot % 7 == 1)
                length = 1 + pick(std::max(1, maxPathLength / 100));
            else
                length = maxPathLength / 4 + pick(3 * maxPathLength / 4 + 1);
        }
        scenario.abysses[robot] = length + 1;
    }

    const int wreckRobot = pick(ROBOT_COUNT);
    if (scenario.abysses[wreckRobot] == 1)
        scenario.abysses[wreckRobot] = maxPathLength + 1;
    const int wreckSegment = 1 + pick(scenario.abysses[wreckRobot] - 1);
    scenario.shipwreck = Location{wreckRobot, wreckSegment};

    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        const int length = scenario.abysses[robot] - 1;
        if (length == 0)
            continue;
        for (int sample = 0; sample < backgroundPerRobot; ++sample)
            scenario.debris.push_back({robot, 1 + pick(length)});
    }

    addDebrisCluster(scenario, random, *scenario.shipwreck, distribution.wreckClusterSamples);
    for (int cluster = 0; cluster < distribution.decoyClusterCount; ++cluster)
    {
        const int robot = pick(ROBOT_COUNT);
        const int length = scenario.abysses[robot] - 1;
        if (length > 0)
            addDebrisCluster(scenario, random, {robot, 1 + pick(length)}, distribution.decoyClusterSamples);
    }

    if (wreckAvailability == WreckAvailability::Unfindable)
    {
        if (seed % 2 == 0 || wreckSegment == TETHER_LENGTH)
            scenario.shipwreck.reset();
        else
        {
            scenario.abysses[wreckRobot] = wreckSegment;
            scenario.shipwreck = Location{wreckRobot, wreckSegment + 1};
        }
    }
    normalizeDebris(scenario, random);
    return scenario;
}
}
