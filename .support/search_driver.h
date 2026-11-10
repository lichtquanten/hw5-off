/*
 * PROVIDED CODE - DO NOT EDIT
 */

#pragma once

#include <chrono>
#include <optional>

#include "mothership.h"

class Search;

enum class DecisionCadence
{
  Student,
  EveryDrive
};

struct SearchRun
{
  std::optional<Location> shipwreck;
  long long drives = 0;
  long long selections = 0;
  long long continuationChecks = 0;
  std::chrono::nanoseconds selectionTime{};
  std::chrono::nanoseconds recordingTime{};
  std::chrono::nanoseconds continuationTime{};
};

SearchRun runSearch(Search &search, Mothership &mothership,
                    DecisionCadence cadence,
                    long long driveLimit = 1LL * ROBOT_COUNT * TETHER_LENGTH);
