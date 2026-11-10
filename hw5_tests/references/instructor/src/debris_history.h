#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

class DebrisHistory
{
public:
    void recordDiscovery(int segment)
    {
        if (segment < minimumRetainedSegment)
            return;
        assert(positions.empty() || positions.back() < segment);
        positions.push_back(segment);
    }

    int count(long long lower, long long upper) const
    {
        const auto retainedBegin = positions.begin() + firstRetained;
        const auto left = std::lower_bound(retainedBegin, positions.end(), lower);
        const auto right = std::upper_bound(left, positions.end(), upper);
        return static_cast<int>(right - left);
    }

    void discardBefore(long long minimumSegment)
    {
        if (minimumSegment <= minimumRetainedSegment)
            return;
        minimumRetainedSegment = minimumSegment;
        const auto retainedBegin = positions.begin() + firstRetained;
        const auto firstKept = std::lower_bound(retainedBegin, positions.end(), minimumSegment);
        firstRetained = firstKept - positions.begin();
        if (firstRetained > positions.size() / 2)
            compactRetiredPrefix();
    }

    void discardAll()
    {
        discardBefore(std::numeric_limits<long long>::max());
    }

    std::size_t size() const { return positions.size() - firstRetained; }
    std::size_t capacity() const { return positions.capacity(); }

private:
    std::vector<int> positions;
    std::size_t firstRetained = 0;
    long long minimumRetainedSegment = std::numeric_limits<long long>::lowest();

    void compactRetiredPrefix()
    {
        std::vector<int> retained(positions.begin() + firstRetained, positions.end());
        positions.swap(retained);
        firstRetained = 0;
    }
};
