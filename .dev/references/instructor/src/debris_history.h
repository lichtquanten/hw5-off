#pragma once

#include <cstddef>
#include <vector>

class DebrisHistory
{
public:
    void recordDiscovery(int segment);
    int countInRange(int lower, int upper) const;
    void discardBefore(int segment);

private:
    std::vector<int> positions;
    std::size_t firstRetained = 0;
    int discardedBefore = 0;
};
