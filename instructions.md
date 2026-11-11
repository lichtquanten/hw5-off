# Overview

You must find a shipwreck on the ocean floor!

<image>

A ship sank to the ocean floor a long time ago. You on a mothership, with 100 underwater robots to use for the search.

As the ship sank, it scattered debris along the seafloor. The debris can be used to aid the search.

Your goal is to find the shipwreck, or to report that the shipwreck is not findable.

## Robots

The robots start evenly spaced in a circle around the mothership. Each robot moves in a straight line, radially out from the mothership.

Each robot's path is divided into segments. Each segment takes 1 tick (a unit of time) to drive.

The shipwreck occupies one segment and is reachable by at most one robot.

You can drive one robot at a time. When instructed to drive, a robot drives forward for up to 100 segments, stopping sooner if it encounters debris or the shipwreck.

After driving, the robot will report finding:
- nothing,
- debris, or
- the shipwreck.

After driving a robot, you can choose to drive the same robot again, or switch to another robot. Switching to another robot takes 10 ticks.

Each robot is attached to the mothership by a tether of length 10 million segments. Driving beyond the tether length causes failure.

## Abysses

Robots may encounter abysses. Abysses are deep openings in the ground. Driving into an abyss causes failure. You are given a list of positions for the abysses at the start.

## Debris and Signal Strength

You expect that the shipwreck will be surrounded by debris. Since much time has passed since the sinking, though, debris will have been swept up and carried to areas away from the shipwreck.

You can reason about debris using a measure called signal strength. Signal strength predicts the likelihood of finding the shipwreck near a robot. Debris found nearby a robot adds to signal strength.

For signal strength, “nearby” means:
- within 500 segments of a robot, along its own path
- within 500 segments of a robot, along the paths of the robots immediately to its left and right

Each nearby segment of debris has been found contributes equally to signal strength, whether it is along a robot's own path or its neighbors' paths.

# Key Info

| Item | Value |
| --- | --- |
| Number of robots | 100 |
| Speed | 1 segment/tick |
| Maximum distance per drive | 100 segments |
| Cost to switch robots | 10 ticks |
| Tether length | 10 million segments |
| Signal range | Within 500 segments, along the robot's own path and its two neighboring paths |

# Evaluation

Your goal is to find the shipwreck, or to report that the shipwreck is not findable, while making defensible decisions as far as search heuristic and effective use of data structures.

The assignment will be evaluated on whether your solution finds the shipwreck or correctly reports that it is not findable.

The assignment will also be evaluated on:

- search efficiency, as measured in ticks
- runtime cost, i.e. how long in seconds it takes for the program to run
- memory cost, i.e. how many bytes of memory the program uses at its peak.

Your solution will be evaluated against a fix threshold for each of those criteria, as seen with `make test`. That said, there is no single optimal approach to this problem.

# Developing Your Solution

You should try to find the shipwreck as quickly as possible, if it is findable. That said, this assignment makes use of heuristics, rather than mathematical formulas defining an optimal approach. A heuristic is a strategy intended to solve a problem efficiently, without guaranteeing the most efficient approach. You can use your judgment and intuition to come up with and justify heuristics.

You have two key questions to consider:

1. What is your search heuristic?

2. How do you store, access, and update the data needed for this search heuristic?

When improving your solution's performance, you will usually want to think about your overall approach, rather than going through your code line by line.

## Data Structures

Consider the data structures you have learned thus far in the course:

- Linked list
- Stack
- Queue
- ArrayLists
- Heap
- Hash table
- Trees
- AVL tree
- Splay tree

Look at `src/examples/using_data_structures.cpp`. You will find examples of how you can use each of these data structures should any seem useful in the assignment.

# Building Off of the Code

## Starter Code

- `src/search.{h,cpp}`: Provides the `Search` class, which performs the search in `findShipwreck()` and uses `Mothership` to control the robots. [EDIT]
- `src/mothership.h`: Provides the `Mothership` class to switch robots, drive them, and report what they find. Also provides robot and abyss positions, and constants for robot count, tether length, drive limit, switching cost, and signal range. [DO NOT EDIT]
- `src/seafloor_survey.{h,cpp}`: Provides the `SeafloorSurvey` class for storing debris discoveries and computing signal strength. [OPTIONAL, EDIT]

# Editing Code

Implement your search in `findShipwreck()`. Return `Location{robot, segment}` if the shipwreck is found, or `std::nullopt` if it is not findable.

You can add any data members or member methods to the `Search` class. You will likely want to add more classes and files.

You do not need to update the Makefile when adding files to `src`. The Makefile is configured already to find `.ccp` and `.h` files in `src/`.

You do not need to write your own `main()`. `make smoke-test` and `make test` take care of running your `Search` class against search scenarios.

## Toy Code

Pieces of the starter code are marked as "TOY". These toy approaches are basic. You should replace the toy code with code that supports your search heuristic and makes effective use of data structures.

`SeafloorSurvey`, in `src/seafloor_survey.{h,cpp}`, offers an example of recording found debris outside the `Search` class. You can use this class or not, and you can make any changes to the class, as well.

## Running Tests

To run a quick check for correctness:

```bash
make smoke-test
```

To run the full test suite, including correctness and performance tests:

```bash
make test
```

The full test suite may take up to 5 minutes or so to run.

The full test suite compares your solution to two reference solutions: a round-robin solution and an instructor solution. The round-robin solution advances each robot one after the other, without considering debris, until finding the shipwreck. We do not share how the instructor solution works - only its performance. The instructor solution uses defensible search heuristics and makes effective use of data structures. That said, you can outperform the instructor solution! It is one of many reasonable solutions, and there is no optimal solution to this problem.
