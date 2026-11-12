#pragma once

#include "scenario.h"

#include <algorithm>
#include <array>
#include <random>
#include <tuple>
#include <utility>

namespace shipwreck_tests
{
constexpr int CORE_RADIUS = 10'000;
constexpr int OUTER_RADIUS = 50'000;
constexpr int MIN_SITE_DEPTH = 20'000;
constexpr int MAX_SITE_DEPTH = 60'000;

struct DebrisStrength
{
    double coreProbability;
    double outerProbability;

    DebrisStrength scaled(double factor) const
    {
        return {coreProbability * factor, outerProbability * factor};
    }
};

struct ScenarioSettings
{
    int pathLength = 250'000;
    double backgroundProbability = 0.001;
    DebrisStrength targetDebris{0.04, 0.003};
    std::array<DebrisStrength, 3> formerWreckDebris{{
        {0.01, 0.0005}, {0.02, 0.001}, {0.03, 0.002},
    }};
};

struct WreckSite
{
    Location center;
    DebrisStrength debris;
};

inline int clusterOffset(std::mt19937& random, int radius)
{
    const int first = random() % (radius + 1);
    const int second = random() % (radius + 1);
    return first - second;
}

inline std::vector<WreckSite> chooseWreckSites(
    const ScenarioSettings& settings, std::optional<Location> targetCenter, std::mt19937& random)
{
    std::vector<WreckSite> sites;
    if (targetCenter)
        sites.push_back({*targetCenter, settings.targetDebris});
    for (const auto& debris : settings.formerWreckDebris)
    {
        const int robot = random() % ROBOT_COUNT;
        const int segment = MIN_SITE_DEPTH + random() % (MAX_SITE_DEPTH - MIN_SITE_DEPTH + 1);
        sites.push_back({{robot, segment}, debris});
    }
    return sites;
}

inline void addBackgroundDebris(
    Scenario& scenario, double probability, std::mt19937& random)
{
    for (int robot = 0; robot < ROBOT_COUNT; ++robot)
    {
        const int length = scenario.abysses[robot] - 1;
        const int samples = static_cast<int>(length * probability);
        for (int sample = 0; sample < samples; ++sample)
            scenario.debris.push_back({robot, 1 + static_cast<int>(random() % length)});
    }
}

inline void addClusterDebris(
    Scenario& scenario, Location center, int radius, double peakProbability,
    std::mt19937& random)
{
    const int samplesPerPath = static_cast<int>(radius * peakProbability);
    for (int neighborOffset : {-1, 0, 1})
    {
        const int robot = (center.robot + ROBOT_COUNT + neighborOffset) % ROBOT_COUNT;
        for (int sample = 0; sample < samplesPerPath; ++sample)
        {
            const int segment = center.segment + clusterOffset(random, radius);
            if (segment >= 1 && segment < scenario.abysses[robot])
                scenario.debris.push_back({robot, segment});
        }
    }
}

inline void addWreckDebris(Scenario& scenario, const WreckSite& site, std::mt19937& random)
{
    addClusterDebris(scenario, site.center, CORE_RADIUS, site.debris.coreProbability, random);
    addClusterDebris(scenario, site.center, OUTER_RADIUS, site.debris.outerProbability, random);
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
    const ScenarioSettings& settings, unsigned seed,
    std::optional<Location> targetCenter = std::nullopt)
{
    std::mt19937 random(seed);
    Scenario scenario;
    scenario.abysses.fill(settings.pathLength + 1);
    const auto sites = chooseWreckSites(settings, targetCenter, random);
    std::size_t debrisCapacity = ROBOT_COUNT *
        static_cast<std::size_t>(settings.pathLength * settings.backgroundProbability);
    for (const auto& site : sites)
    {
        debrisCapacity += 3 * (
            static_cast<std::size_t>(CORE_RADIUS * site.debris.coreProbability) +
            static_cast<std::size_t>(OUTER_RADIUS * site.debris.outerProbability));
    }
    scenario.debris.reserve(debrisCapacity);
    if (targetCenter)
    {
        const int segment = std::clamp(
            targetCenter->segment + clusterOffset(random, CORE_RADIUS), 1, settings.pathLength);
        scenario.shipwreck = Location{targetCenter->robot, segment};
    }
    addBackgroundDebris(scenario, settings.backgroundProbability, random);
    for (const auto& site : sites)
        addWreckDebris(scenario, site, random);
    normalizeDebris(scenario, random);
    return scenario;
}
}
