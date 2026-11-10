#include <gtest/gtest.h>
#include "performance_scenarios.h"

#include <stdexcept>

using shipwreck_tests::DebrisPattern;
using shipwreck_tests::PathLayout;
using shipwreck_tests::WreckAvailability;
using shipwreck_tests::makePerformanceScenario;

TEST(PerformanceScenarios, RejectsInvalidDimensions)
{
    EXPECT_THROW(makePerformanceScenario(1, 0, 0, DebrisPattern::Sparse), std::invalid_argument);
    EXPECT_THROW(makePerformanceScenario(1, TETHER_LENGTH + 1, 0, DebrisPattern::Sparse), std::invalid_argument);
    EXPECT_THROW(makePerformanceScenario(1, 100, -1, DebrisPattern::Sparse), std::invalid_argument);
}

TEST(PerformanceScenarios, TinyPathsRemainValid)
{
    for (unsigned seed = 0; seed < 16; ++seed)
    {
        SCOPED_TRACE(seed);
        for (auto layout : {PathLayout::Uniform, PathLayout::Mixed})
        {
            for (auto availability : {WreckAvailability::Reachable, WreckAvailability::Unfindable})
            {
                const auto scenario = makePerformanceScenario(
                    seed, 1, 0, static_cast<DebrisPattern>(seed % 4), layout, availability);
                EXPECT_NO_THROW(validateScenario(scenario));
            }
        }
    }
}
