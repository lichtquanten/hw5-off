#pragma once

#include <array>
#include <cstddef>
#include <queue>
#include <vector>

#include "mothership.h"

class AheadDebris
{
public:
    explicit AheadDebris(const Mothership& mothership);
    void recordDiscovery(int sourceRobot, int segment);
    double bestNeighborBucketScore(int robot);
    void discardPassedBuckets(int sourceRobot);
    void discardRobotCandidates(int robot);

private:
    static constexpr int BUCKET_LENGTH = 2 * SIGNAL_RANGE;

    struct Bucket
    {
        int end;
        int discoveries;
    };

    struct PathBuckets
    {
        std::vector<Bucket> entries;
        std::size_t firstRetained = 0;
        int discardedThrough = 0;
        void recordDiscovery(int bucketEnd);
        int discoveriesInBucket(int bucketEnd) const;
        void discardThrough(int segment);
        std::size_t size() const { return entries.size() - firstRetained; }
    };

    struct BucketCandidate
    {
        long long rewardMinusEnd;
        int bucketEnd;
        int discoveries;
        bool operator<(const BucketCandidate& other) const;
    };

    using CandidateQueue = std::priority_queue<BucketCandidate>;

    const Mothership& mothership;
    std::array<PathBuckets, ROBOT_COUNT> buckets;
    std::array<CandidateQueue, ROBOT_COUNT> candidates;

    int pathEnd(int robot) const;
    bool isUnsearchedBucket(int robot, int bucketEnd) const;
    int neighboringDiscoveries(int robot, int bucketEnd) const;
    BucketCandidate makeCandidate(int robot, int bucketEnd) const;
    void rebuildCandidates(int robot);
};
