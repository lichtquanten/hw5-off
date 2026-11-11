# Overview

You must find a shipwreck on the ocean floor!

# Search

You are looking for a shipwreck. The ship sank to the ocean floor a long time ago. You’re on a mothership out in the ocean. You have N underwater robots to use for your search.

Each robot roves along the sea floor, following a straight line. Each robot travels radially from the mothership in a different direction.

You can drive one robot at a time. You do this by driving the robot forward for 1 segment (a unit of distance) in 1 tick (a unit of time). You can switch from one robot to another. This switch takes 5 ticks.

After driving a segment, you get a report on if the robot found:

- nothing,
- debris, or
- the shipwreck.

Each robot is attached to a tether of length T segments. No robot can travel more than T segments. T is large, on the order of billions of segments.

Robots may encounter abysses. Abysses are deep openings in the ground. When a robot reaches an abyss, it can go no further. You are given a list of positions for the abysses at the start.

The shipwreck will be in one segment and reachable by one robot. As the ship originally sank, debris scattered from the ship about the ocean floor. You expect that the ship will be surrounded by debris. Since much time has passed since the sinking, though, debris will have been swept up and carried to areas away from the shipwreck.

You can reason about debris using a measure called signal strength. Signal strength predicts the likelihood of finding the shipwreck near to a robot. Debris found near a robot adds to signal strength. That is, more debris found nearby indicates a higher likelihood of finding the shipwreck. Nearby is considered:

  - within M segments of a robot, along its own path
  - within M segments of a robot, along the paths of the robots to its left and right

Each nearby segment with debris found, whether along a robot’s own path or its neighbors’ paths, contributes equally to signal strength.

Your goal is to find the shipwreck, or to report that the shipwreck is not findable.

You should try to find the shipwreck as quickly as possible, if it is findable. That said, this assignment makes use of heuristics, rather than mathematical formulas defining an optimal approach. You can use your judgment and intuition to come up with and justify heuristics. There is no single right solution to this problem.

# Guiding Your Problem Solving

You have two key questions to consider:

1. What is your search heuristic?

2. How do you store, access, and update the data needed for this search heuristic?

A heuristic is "an algorithmic approach that utilizes human expert knowledge to build a function for decision-making."

# Data Structures

Consider the data structures you have learned thus far in the course:

  - Linked list Stack
  - Queue
  - ArrayLists
  - Hash table
  - Heap
  - Trees
  - AVL tree
  - Splay tree

Consider how these data structures can help you.

# Running Your Code

To run a quick check for correctness:

```bash
make smoke-test
```

To run the full test suite, including correctness and performance tests:
```bash
make test
```

The full test suite may take up to 5 minutes or so to run.

The full test suite compares your solution to two reference solutions: a round-robin solution and an instructor solution. The round-robin solution advances each robot one after the other, without considering debris, until finding the shipwreck. We do not share how the instructor solution works - only its performance. The instructor solution uses defensible search heuristics and makes effective use of data strutures. That said, you can outperform the instructor solution! It is one of many reasonable solutions, and there is no optimal solution to this problem.

# Evaluation

The assignment will be graded on correctness of finding the shipwreck, or of reporting that the shipwreck is not findable.

The assignment will also be graded on search efficiency, as measured in ticks; runtime cost, i.e. how long in seconds it takes for the program to run; and memory cost, i.e. how many bytes of memory the program uses at its peak.

In improving your solution's performance, you will typically want to think at a conceptual level about your approach, rather than thinking about your code on a line-by-line basis. Coming up with an approach that is efficient as far as ticks, runtime, and memory is something that can be done on pen and paper.
