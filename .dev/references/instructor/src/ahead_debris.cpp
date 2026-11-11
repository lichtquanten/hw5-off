#include "ahead_debris.h"

#include <algorithm>
#include <limits>
#include <utility>

#include "robot_neighbors.h"

namespace
{
constexpr std::size_t QUEUE_REBUILD_FACTOR = 4;
constexpr int TRAVEL_REWARD_PER_DISCOVERY = 10'000;
}

AheadDebris::AheadDebris(const Mothership& mothership) : mothership(mothership) {}

void AheadDebris::PathBuckets::recordDiscovery(int bucketEnd)
{
    if (bucketEnd <= discardedThrough)
        return;
    if (entries.empty() || entries.back().end != bucketEnd)
        entries.push_back({bucketEnd, 0});
    ++entries.back().discoveries;
}

int AheadDebris::PathBuckets::discoveriesInBucket(int bucketEnd) const
{
    const auto bucket = std::lower_bound(entries.begin() + firstRetained, entries.end(), bucketEnd,
        [](const Bucket& bucket, int end) { return bucket.end < end; });
    return bucket != entries.end() && bucket->end == bucketEnd ? bucket->discoveries : 0;
}

void AheadDebris::PathBuckets::discardThrough(int segment)
{
    if (segment <= discardedThrough)
        return;
    discardedThrough = segment;
    firstRetained = std::upper_bound(entries.begin() + firstRetained, entries.end(), segment,
        [](int end, const Bucket& bucket) { return end < bucket.end; }) - entries.begin();
    if (firstRetained > entries.size() / 2)
    {
        std::vector<Bucket>(entries.begin() + firstRetained, entries.end()).swap(entries);
        firstRetained = 0;
    }
}

bool AheadDebris::BucketCandidate::operator<(const BucketCandidate& other) const
{
    if (rewardMinusEnd != other.rewardMinusEnd)
        return rewardMinusEnd < other.rewardMinusEnd;
    return bucketEnd > other.bucketEnd;
}

int AheadDebris::pathEnd(int robot) const
{
    return mothership.getAbysses()[robot] - 1;
}

bool AheadDebris::isUnsearchedBucket(int robot, int bucketEnd) const
{
    return bucketEnd - BUCKET_LENGTH < pathEnd(robot) &&
           mothership.getRobotPosition(robot) < std::min(bucketEnd, pathEnd(robot));
}

int AheadDebris::neighboringDiscoveries(int robot, int bucketEnd) const
{
    const auto neighbors = neighborsOf(robot);
    return buckets[neighbors[0]].discoveriesInBucket(bucketEnd) +
           buckets[neighbors[1]].discoveriesInBucket(bucketEnd);
}

AheadDebris::BucketCandidate AheadDebris::makeCandidate(int robot, int bucketEnd) const
{
    const int count = neighboringDiscoveries(robot, bucketEnd);
    return {static_cast<long long>(TRAVEL_REWARD_PER_DISCOVERY) * count - std::min(bucketEnd, pathEnd(robot)),
            bucketEnd, count};
}

void AheadDebris::recordDiscovery(int sourceRobot, int segment)
{
    const int bucketEnd = ((segment - 1) / BUCKET_LENGTH + 1) * BUCKET_LENGTH;
    buckets[sourceRobot].recordDiscovery(bucketEnd);
    for (int neighbor : neighborsOf(sourceRobot))
        if (isUnsearchedBucket(neighbor, bucketEnd))
            candidates[neighbor].push(makeCandidate(neighbor, bucketEnd));
}

void AheadDebris::rebuildCandidates(int robot)
{
    const auto neighbors = neighborsOf(robot);
    std::vector<BucketCandidate> current;
    current.reserve(buckets[neighbors[0]].size() + buckets[neighbors[1]].size());
    for (int path : neighbors)
    {
        const auto& source = buckets[path];
        for (auto bucket = source.entries.begin() + source.firstRetained; bucket != source.entries.end(); ++bucket)
        {
            if (!isUnsearchedBucket(robot, bucket->end))
                continue;
            if (path == neighbors[1] && buckets[neighbors[0]].discoveriesInBucket(bucket->end))
                continue;
            current.push_back(makeCandidate(robot, bucket->end));
        }
    }
    candidates[robot] = CandidateQueue(std::less<BucketCandidate>(), std::move(current));
}

double AheadDebris::bestNeighborBucketScore(int robot)
{
    auto& queue = candidates[robot];
    const auto neighbors = neighborsOf(robot);
    const auto retained = buckets[neighbors[0]].size() + buckets[neighbors[1]].size();
    if (queue.size() > QUEUE_REBUILD_FACTOR * (retained + 1))
        rebuildCandidates(robot);
    while (!queue.empty())
    {
        const auto& bucket = queue.top();
        if (!isUnsearchedBucket(robot, bucket.bucketEnd) ||
            bucket.discoveries != neighboringDiscoveries(robot, bucket.bucketEnd))
        {
            queue.pop();
            continue;
        }
        const auto reward = bucket.rewardMinusEnd + mothership.getRobotPosition(robot);
        return static_cast<double>(std::max(0LL, reward)) / TRAVEL_REWARD_PER_DISCOVERY;
    }
    return 0;
}

void AheadDebris::discardPassedBuckets(int sourceRobot)
{
    int passedByNeighbors = std::numeric_limits<int>::max();
    for (int neighbor : neighborsOf(sourceRobot))
        if (mothership.getRobotPosition(neighbor) < pathEnd(neighbor))
            passedByNeighbors = std::min(passedByNeighbors, mothership.getRobotPosition(neighbor));
    buckets[sourceRobot].discardThrough(passedByNeighbors);
}

void AheadDebris::discardRobotCandidates(int robot)
{
    candidates[robot] = CandidateQueue();
}
