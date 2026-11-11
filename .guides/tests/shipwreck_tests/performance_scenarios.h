#pragma once

#include "scenario.h"

#include <algorithm>
#include <random>
#include <tuple>
#include <utility>

namespace shipwreck_tests
{
enum class DebrisPattern { Clustered, Misleading, WeakSignal, Sparse };
enum class PathLayout { Uniform, Mixed };
enum class WreckAvailability { Reachable, Unfindable };

struct DebrisDistribution
{
    int wreckDensityDivisor = 10;
    int decoyClusterCount = 3;
    int decoyDensityDivisor = 16;
};

inline DebrisDistribution debrisDistribution(DebrisPattern pattern)
{
    DebrisDistribution distribution;
    switch (pattern)
    {
    case DebrisPattern::Clustered:
        break;
    case DebrisPattern::Misleading:
        distribution.decoyDensityDivisor = 5;
        break;
    case DebrisPattern::WeakSignal:
        distribution.wreckDensityDivisor = 25;
        break;
    case DebrisPattern::Sparse:
        distribution.wreckDensityDivisor = 60;
        distribution.decoyClusterCount = 2;
        distribution.decoyDensityDivisor = 50;
        break;
    }
    return distribution;
}

inline void addDebrisCluster(Scenario& scenario, std::mt19937& random,
                            Location center, int radius, int sampleCount)
{
    for (int sample = 0; sample < sampleCount; ++sample)
    {
        const int neighborOffset = static_cast<int>(random() % 3) - 1;
        const int neighbor = (center.robot + ROBOT_COUNT + neighborOffset) % ROBOT_COUNT;
        const int segmentOffset = static_cast<int>(random() % (radius + 1)) -
                                  static_cast<int>(random() % (radius + 1));
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
    unsigned seed, int backgroundPerRobot, DebrisPattern pattern,
    PathLayout layout = PathLayout::Uniform,
    WreckAvailability wreckAvailability = WreckAvailability::Reachable,
    int pathLength = TETHER_LENGTH,
    std::optional<Location> wreckLocation = std::nullopt)
{
    std::mt19937 random(seed);
    const auto pick = [&random](int count) { return static_cast<int>(random() % count); };
    const DebrisDistribution distribution = debrisDistribution(pattern);
    Scenario scenario;
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        int length = pathLength;
        if (layout == PathLayout::Mixed)
        {
            if (robot % 7 == 0)
                length = 0;
            else if (robot % 7 == 1)
                length = 1 + pick(std::max(1, pathLength / 100));
            else
                length = pathLength / 4 + pick(3 * pathLength / 4 + 1);
        }
        scenario.abysses[robot] = length + 1;
    }

    const int wreckRobot = wreckLocation ? wreckLocation->robot : pick(ROBOT_COUNT);
    if (scenario.abysses[wreckRobot] == 1)
        scenario.abysses[wreckRobot] = pathLength + 1;
    const int wreckSegment = wreckLocation ? wreckLocation->segment :
                            1 + pick(scenario.abysses[wreckRobot] - 1);
    scenario.abysses[wreckRobot] = std::max(scenario.abysses[wreckRobot], wreckSegment + 1);
    scenario.shipwreck = Location{wreckRobot, wreckSegment};

    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        const int length = scenario.abysses[robot] - 1;
        if (length == 0)
            continue;
        for (int sample = 0; sample < backgroundPerRobot; ++sample)
            scenario.debris.push_back({robot, 1 + pick(length)});
    }

    const int wreckRadius = std::max(SIGNAL_RANGE, 3 * pathLength / 4);
    const int wreckSamples = std::min(50'000, wreckRadius / 10) * 10 /
                             distribution.wreckDensityDivisor;
    addDebrisCluster(scenario, random, *scenario.shipwreck, wreckRadius, wreckSamples);
    for (int cluster = 0; cluster < distribution.decoyClusterCount; ++cluster)
    {
        const int robot = pick(ROBOT_COUNT);
        const int length = scenario.abysses[robot] - 1;
        if (length > 0)
        {
            const int radius = std::max(SIGNAL_RANGE, pathLength / 10);
            const int samples = std::min(50'000, radius / 5) * 5 /
                                distribution.decoyDensityDivisor;
            addDebrisCluster(scenario, random, {robot, 1 + pick(length)}, radius, samples);
        }
    }

    if (wreckAvailability == WreckAvailability::Unfindable)
        scenario.shipwreck.reset();
    normalizeDebris(scenario, random);
    return scenario;
}
}
