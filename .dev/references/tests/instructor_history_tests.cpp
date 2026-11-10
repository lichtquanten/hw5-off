#include <gtest/gtest.h>
#include "debris_history.h"

#include <algorithm>
#include <random>
#include <vector>

TEST(DebrisHistory, InclusiveBoundaries)
{
    DebrisHistory history;
    for (int segment : {1, 499, 500, 501, 1000})
        history.recordDiscovery(segment);
    EXPECT_EQ(history.count(500, 500), 1);
    EXPECT_EQ(history.count(0, 500), 3);
    history.discardBefore(500);
    EXPECT_EQ(history.size(), 3u);
    EXPECT_EQ(history.count(500, 1000), 3);
    EXPECT_EQ(history.count(0, 499), 0);
}

TEST(DebrisHistory, ReclaimsStorageAndAcceptsLaterDiscoveries)
{
    DebrisHistory history;
    for (int segment = 1; segment <= 10000; ++segment)
        history.recordDiscovery(segment);
    const auto capacity = history.capacity();
    history.discardBefore(9900);
    EXPECT_EQ(history.size(), 101u);
    EXPECT_LT(history.capacity(), capacity);
    history.recordDiscovery(10001);
    EXPECT_EQ(history.count(9900, 10001), 102);
    history.discardAll();
    EXPECT_EQ(history.size(), 0u);
    EXPECT_EQ(history.capacity(), 0u);
}

TEST(DebrisHistory, DiscoveriesBehindRetiredWindowStayDiscarded)
{
    DebrisHistory history;
    history.recordDiscovery(10);
    history.discardBefore(100);
    history.recordDiscovery(50);
    history.recordDiscovery(100);
    history.recordDiscovery(150);
    history.discardBefore(90);
    EXPECT_EQ(history.size(), 2u);
    EXPECT_EQ(history.count(0, 200), 2);
}

TEST(DebrisHistory, InterleavedQueriesAndRetirementMatchFullHistory)
{
    std::mt19937 random(237);
    DebrisHistory history;
    std::vector<int> full;
    long long cutoff = 0;
    int position = 0;
    for (int step = 0; step < 20000; ++step)
    {
        position += 1 + random() % 5;
        full.push_back(position);
        history.recordDiscovery(position);
        if (random() % 4 == 0)
        {
            cutoff += random() % 15;
            history.discardBefore(cutoff);
        }
        const long long lower = cutoff + random() % 100;
        const long long upper = lower + random() % 1000;
        const auto first = std::lower_bound(full.begin(), full.end(), lower);
        const auto last = std::upper_bound(first, full.end(), upper);
        EXPECT_EQ(history.count(lower, upper), last - first) << "step " << step;
    }
}
