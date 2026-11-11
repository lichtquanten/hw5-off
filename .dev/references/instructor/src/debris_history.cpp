#include "debris_history.h"

#include <algorithm>

void DebrisHistory::recordDiscovery(int segment)
{
    positions.push_back(segment);
}

int DebrisHistory::countInRange(int lower, int upper) const
{
    const auto first = std::lower_bound(positions.begin() + firstRetained, positions.end(), lower);
    return std::upper_bound(first, positions.end(), upper) - first;
}

void DebrisHistory::discardBefore(int segment)
{
    if (segment <= discardedBefore)
        return;
    discardedBefore = segment;
    firstRetained = std::lower_bound(positions.begin() + firstRetained, positions.end(), segment)
                    - positions.begin();
    if (firstRetained > positions.size() / 2)
    {
        std::vector<int>(positions.begin() + firstRetained, positions.end()).swap(positions);
        firstRetained = 0;
    }
}
