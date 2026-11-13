#pragma once

#include "scenario.h"

#include <algorithm>
#include <array>
#include <random>
#include <iterator>
#include <tuple>
#include <vector>

namespace shipwreck_tests
{
  constexpr int CORE_RADIUS = 10'000;
  constexpr int OUTER_RADIUS = 50'000;
  constexpr int CORE_RADII[] = {300, 1'000, 3'000, 10'000, 20'000};
  constexpr int OUTER_RADII[] = {10'000, 25'000, 50'000, 80'000};
  constexpr int WRECK_SCENARIOS = 16;
  constexpr int MIN_DECOY_PATH_OFFSET = 3;
  constexpr int MIN_DECOY_LEAD = 10'000;
  constexpr int MAX_DECOY_LEAD = 20'000;
  constexpr int MIN_DECOY_DEPTH = 10'000;

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
    int robotCount = DEFAULT_ROBOT_COUNT;
    int pathLength = 250'000;
    double backgroundProbability = 0.001;
    DebrisStrength targetDebris{0.04, 0.003};
    std::array<DebrisStrength, 3> formerWreckDebris{{
        {0.01, 0.0005},
        {0.02, 0.001},
        {0.03, 0.002},
    }};
    int minWreckDepth = 25'000;
    int maxWreckDepth = 55'000;
    int minSiteDepth = 20'000;
    int maxSiteDepth = 60'000;
    bool singleApproachPath = false;
    bool variedShapes = false;
  };

  struct WreckSite
  {
    Location center;
    int coreDebrisPerPath;
    int outerDebrisPerPath;
    int coreRadius = CORE_RADIUS;
    int outerRadius = OUTER_RADIUS;
    std::vector<int> approachPaths{-1, 0, 1};
  };

  inline WreckSite makeSite(Location center, DebrisStrength debris)
  {
    return {center, static_cast<int>(CORE_RADIUS * debris.coreProbability),
            static_cast<int>(OUTER_RADIUS * debris.outerProbability)};
  }

  inline int clusterOffset(std::mt19937 &random, int radius)
  {
    const int first = random() % (radius + 1);
    const int second = random() % (radius + 1);
    return first - second;
  }

  inline Location wreckCenter(const ScenarioSettings &settings, int index)
  {
    const int robot = (12 + 37 * index) % settings.robotCount;
    const int depthRange = settings.maxWreckDepth - settings.minWreckDepth;
    return {robot, settings.minWreckDepth + index * depthRange / (WRECK_SCENARIOS - 1)};
  }

  inline Location randomSiteCenter(const ScenarioSettings &settings, std::mt19937 &random)
  {
    const int robot = random() % settings.robotCount;
    const int segment = settings.minSiteDepth + random() % (settings.maxSiteDepth - settings.minSiteDepth + 1);
    return {robot, segment};
  }

  inline Location decoyCenter(Location target, int robotCount, std::mt19937 &random)
  {
    const int pathOffset = MIN_DECOY_PATH_OFFSET + random() % (robotCount - 2 * MIN_DECOY_PATH_OFFSET + 1);
    const int lead = MIN_DECOY_LEAD + random() % (MAX_DECOY_LEAD - MIN_DECOY_LEAD + 1);
    const int robot = (target.robot + pathOffset) % robotCount;
    return {robot, std::max(MIN_DECOY_DEPTH, target.segment - lead)};
  }

  inline std::vector<WreckSite> chooseWreckSites(
      const ScenarioSettings &settings, std::optional<Location> targetCenter, std::mt19937 &random)
  {
    std::vector<WreckSite> sites;
    if (targetCenter)
    {
      sites.push_back(makeSite(*targetCenter, settings.targetDebris));
    }
    const auto &formerWrecks = settings.formerWreckDebris;
    for (std::size_t index = 0; index < formerWrecks.size(); ++index)
    {
      const bool isDecoy = targetCenter && index + 1 == formerWrecks.size();
      const Location center = isDecoy ? decoyCenter(*targetCenter, settings.robotCount, random) : randomSiteCenter(settings, random);
      sites.push_back(makeSite(center, formerWrecks[index]));
    }
    for (auto &site : sites)
    {
      if (settings.singleApproachPath)
      {
        site.approachPaths = {static_cast<int>(random() % 3) - 1};
      }
      if (settings.variedShapes)
      {
        site.coreRadius = CORE_RADII[random() % std::size(CORE_RADII)];
        site.outerRadius = OUTER_RADII[random() % std::size(OUTER_RADII)];
      }
    }
    return sites;
  }

  inline void addBackgroundDebris(
      Scenario &scenario, double probability, std::mt19937 &random)
  {
    const int robotCount = static_cast<int>(scenario.abysses.size());
    for (int robot = 0; robot < robotCount; ++robot)
    {
      const int length = scenario.abysses[robot] - 1;
      const int samples = static_cast<int>(length * probability);
      for (int sample = 0; sample < samples; ++sample)
      {
        scenario.debris.push_back({robot, 1 + static_cast<int>(random() % length)});
      }
    }
  }

  inline void addClusterDebris(
      Scenario &scenario, Location center, int radius, int samplesPerPath,
      const std::vector<int> &pathOffsets, std::mt19937 &random)
  {
    const int robotCount = static_cast<int>(scenario.abysses.size());
    for (int neighborOffset : pathOffsets)
    {
      const int robot = (center.robot + robotCount + neighborOffset) % robotCount;
      for (int sample = 0; sample < samplesPerPath; ++sample)
      {
        const int segment = center.segment + clusterOffset(random, radius);
        if (segment >= 1 && segment < scenario.abysses[robot])
        {
          scenario.debris.push_back({robot, segment});
        }
      }
    }
  }

  inline void addWreckDebris(Scenario &scenario, const WreckSite &site, std::mt19937 &random)
  {
    addClusterDebris(scenario, site.center, site.coreRadius, site.coreDebrisPerPath, {-1, 0, 1}, random);
    addClusterDebris(scenario, site.center, site.outerRadius, site.outerDebrisPerPath, site.approachPaths, random);
  }

  inline bool sameLocation(Location left, Location right)
  {
    return left.robot == right.robot && left.segment == right.segment;
  }

  inline bool locationBefore(Location left, Location right)
  {
    return std::tie(left.robot, left.segment) < std::tie(right.robot, right.segment);
  }

  inline void normalizeDebris(Scenario &scenario)
  {
    auto &debris = scenario.debris;
    std::sort(debris.begin(), debris.end(), locationBefore);
    debris.erase(std::unique(debris.begin(), debris.end(), sameLocation), debris.end());
    const auto isShipwreck = [&scenario](Location location)
    {
      return scenario.shipwreck && sameLocation(location, *scenario.shipwreck);
    };
    debris.erase(std::remove_if(debris.begin(), debris.end(), isShipwreck), debris.end());
  }

  inline Scenario makePerformanceScenario(
      const ScenarioSettings &settings, unsigned seed,
      std::optional<Location> targetCenter = std::nullopt)
  {
    std::mt19937 random(seed);
    Scenario scenario;
    scenario.abysses.assign(settings.robotCount, settings.pathLength + 1);
    const auto sites = chooseWreckSites(settings, targetCenter, random);
    std::size_t debrisCapacity = settings.robotCount *
                                 static_cast<std::size_t>(settings.pathLength * settings.backgroundProbability);
    for (const auto &site : sites)
    {
      debrisCapacity += 3 * site.coreDebrisPerPath + site.approachPaths.size() * site.outerDebrisPerPath;
    }
    scenario.debris.reserve(debrisCapacity);
    if (targetCenter)
    {
      const int segment = std::clamp(
          targetCenter->segment + clusterOffset(random, sites.front().coreRadius), 1, settings.pathLength);
      scenario.shipwreck = Location{targetCenter->robot, segment};
    }
    addBackgroundDebris(scenario, settings.backgroundProbability, random);
    for (const auto &site : sites)
    {
      addWreckDebris(scenario, site, random);
    }
    normalizeDebris(scenario);
    return scenario;
  }
}
