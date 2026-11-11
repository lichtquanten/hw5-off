/* -------------------- EXAMPLE STRUCT -------------------- */

/*
struct Robot {
    int id;
    double score;
};
*/


/* -------------------- VECTOR / ARRAYLIST -------------------- */

/*
#include <vector>
#include <iostream>

// Numbers
std::vector<int> values{10, 20, 30};
values.push_back(40);
values[0] = 15;
int value = values.at(1);         // Bounds-checked access.

for (int value : values) {
    std::cout << value << '\n';
}

values.erase(values.begin());    // Remove the first element.
values.pop_back();               // Remove the last element.

// Structs
std::vector<Robot> robots{{1, 10.0}, {2, 20.0}};
robots.push_back(Robot{3, 30.0});
robots[0].score = 15.0;

for (const Robot& robot : robots) {
    std::cout << robot.id << ' ' << robot.score << '\n';
}
*/


/* -------------------- DOUBLY LINKED LIST -------------------- */

/*
#include <list>
#include <algorithm>
#include <iostream>

std::list<int> values{10, 20, 30};
values.push_front(5);
values.push_back(40);

auto it = std::find(values.begin(), values.end(), 20);
if (it != values.end()) {
    values.insert(it, 15);        // Insert before 20.
    values.erase(it);            // Remove 20.
}

for (int value : values) {
    std::cout << value << '\n';
}

values.pop_front();
values.pop_back();
*/


/* -------------------- STACK -------------------- */

/*
#include <stack>
#include <iostream>

std::stack<int> values;
values.push(10);
values.push(20);
values.push(30);

while (!values.empty()) {
    int value = values.top();     // 30, then 20, then 10.
    values.pop();                // No return value.
    std::cout << value << '\n';
}

std::stack<Robot> robots;
robots.push(Robot{1, 10.0});
Robot robot = robots.top();
robots.pop();
*/


/* -------------------- QUEUE -------------------- */

/*
#include <queue>
#include <iostream>

std::queue<int> values;
values.push(10);
values.push(20);
values.push(30);

while (!values.empty()) {
    int value = values.front();   // 10, then 20, then 30.
    values.pop();                // No return value.
    std::cout << value << '\n';
}

std::queue<Robot> robots;
robots.push(Robot{1, 10.0});
Robot robot = robots.front();
robots.pop();
*/


/* -------------------- HEAP / PRIORITY QUEUE -------------------- */

/*
#include <queue>
#include <vector>
#include <iostream>

// Numbers: max-heap
std::priority_queue<int> max_heap;
max_heap.push(10);
max_heap.push(20);
max_heap.push(30);

while (!max_heap.empty()) {
    std::cout << max_heap.top() << '\n'; // 30, then 20, then 10.
    max_heap.pop();              // No return value.
}

// Structs: highest score first
struct CompareByScore {
    bool operator()(const Robot& a, const Robot& b) const {
        return a.score < b.score;
    }
};

std::priority_queue<Robot, std::vector<Robot>, CompareByScore> robots;
robots.push(Robot{1, 10.0});
robots.push(Robot{2, 20.0});
Robot robot = robots.top();       // Robot 2.
robots.pop();
*/


/* -------------------- MAP (UNORDERED) -------------------- */

/*
#include <unordered_map>
#include <iostream>

std::unordered_map<int, Robot> robots; // ID -> Robot.
robots[1] = Robot{1, 10.0};        // Insert or replace.
robots[2] = Robot{2, 20.0};
robots.at(1).score = 15.0;         // Throws if the key is missing.

auto it = robots.find(2);
if (it != robots.end()) {
    std::cout << it->second.score << '\n';
}

for (const auto& [id, robot] : robots) { // Unspecified order.
    std::cout << id << ' ' << robot.score << '\n';
}

robots.erase(2);
// Reading robots[id] also inserts an entry if id is missing.
*/


/* -------------------- MAP (ORDERED) -------------------- */

/*
#include <map>
#include <iostream>

std::map<int, Robot> robots;       // ID -> Robot.
robots[1] = Robot{1, 10.0};        // Insert or replace.
robots[2] = Robot{2, 20.0};
robots.at(1).score = 15.0;         // Throws if the key is missing.

auto it = robots.find(2);
if (it != robots.end()) {
    std::cout << it->second.score << '\n';
}

for (const auto& [id, robot] : robots) { // Ascending ID order.
    std::cout << id << ' ' << robot.score << '\n';
}

robots.erase(2);
// Reading robots[id] also inserts an entry if id is missing.
*/


/* -------------------- SET (UNORDERED) -------------------- */

/*
#include <unordered_set>
#include <iostream>

std::unordered_set<int> values{10, 20, 30};
values.insert(40);
values.insert(10);                // Duplicate ignored.

bool found = values.find(20) != values.end();

for (int value : values) {        // Unspecified order.
    std::cout << value << '\n';
}

values.erase(20);
*/


/* -------------------- SET (ORDERED) -------------------- */

/*
#include <set>
#include <iostream>

// Numbers
std::set<int> values{10, 20, 30};
values.insert(40);
bool found = values.find(20) != values.end();

auto it = values.lower_bound(25); // First value >= 25.
if (it != values.end()) {
    std::cout << *it << '\n';     // 30.
}

for (int value : values) {        // Ascending order.
    std::cout << value << '\n';
}
values.erase(20);

// Structs: ordered by ID
struct CompareById {
    bool operator()(const Robot& a, const Robot& b) const {
        return a.id < b.id;
    }
};

std::set<Robot, CompareById> robots;
robots.insert(Robot{1, 10.0});
robots.insert(Robot{2, 20.0});
auto robot_it = robots.find(Robot{2, 0.0}); // Score is ignored.

// IDs determine uniqueness. Erase and reinsert to change an element.
*/


/* -------------------- AVL TREE -------------------- */

/*
#include "trees.h"
#include <iostream>

// Set
AVLSet<int> values{10, 20, 30};
values.insert(40);
bool found = values.find(20) != values.end();

for (int value : values) {        // Ascending order.
    std::cout << value << '\n';
}
values.erase(20);

// Map
AVLMap<int, Robot> robots;
robots[1] = Robot{1, 10.0};
robots[2] = Robot{2, 20.0};
robots.at(1).score = 15.0;

for (const auto& [id, robot] : robots) {
    std::cout << id << ' ' << robot.score << '\n';
}
robots.erase(2);
*/


/* -------------------- SPLAY TREE -------------------- */

/*
#include "trees.h"
#include <iostream>

// Set
SplaySet<int> values{10, 20, 30};
values.insert(40);
bool found = values.find(20) != values.end();

for (int value : values) {        // Ascending order.
    std::cout << value << '\n';
}
values.erase(20);

// Map
SplayMap<int, Robot> robots;
robots[1] = Robot{1, 10.0};
robots[2] = Robot{2, 20.0};
robots.at(1).score = 15.0;

for (const auto& [id, robot] : robots) {
    std::cout << id << ' ' << robot.score << '\n';
}
robots.erase(2);
*/
