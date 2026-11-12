/* -------------------- STRUCTS -------------------- */

/*
#include <string>

struct Movie {
    std::string title;
    int year;
};

struct Task {
    std::string name;
    int priority;

    bool operator<(const Task& other) const { // Needed by priority queue
        return priority < other.priority;
    }
};

struct Contact {
    std::string name;
    std::string phone;
};
*/

/* -------------------- VECTOR -------------------- */

/*
#include <vector>
#include <iostream>

std::vector<Movie> movies;
movies.push_back(Movie{"Snowpiercer", 2013});
movies.push_back(Movie{"Parasite", 2019});

movies[1] = Movie{"Okja", 2017};
Movie second = movies.at(1);
Movie first = movies.front();
Movie last = movies.back();

for (const Movie& movie : movies) {
    std::cout << movie.title << " (" << movie.year << ")\n";
} // Prints Snowpiercer, Okja, Mother

movies.pop_back();
movies.erase(movies.begin());
*/

/* -------------------- LINKED LIST -------------------- */

/*
#include <list>
#include <algorithm>
#include <iostream>

std::list<Movie> watchlist;
watchlist.push_back(Movie{"Parasite", 2019});
watchlist.push_front(Movie{"Snowpiercer", 2013});

auto it = std::find_if(watchlist.begin(), watchlist.end(), [](const Movie& movie) {
    return movie.title == "Parasite";
});
if (it != watchlist.end()) {
    watchlist.insert(it, Movie{"Mother", 2009});   // Inserts before Parasite.
    watchlist.erase(it);                          // Removes Parasite.
}

for (const Movie& movie : watchlist) {
    std::cout << movie.title << " (" << movie.year << ")\n";
} // Prints Snowpiercer, Mother

watchlist.pop_front();
watchlist.pop_back();
*/

/* -------------------- STACK -------------------- */

/*
#include <stack>
#include <iostream>

std::stack<Movie> recently_watched;
recently_watched.push(Movie{"Snowpiercer", 2013});
recently_watched.push(Movie{"Parasite", 2019});
recently_watched.push(Movie{"Mother", 2009});

while (!recently_watched.empty()) {
    Movie movie = recently_watched.top();
    recently_watched.pop();
    std::cout << movie.title << " (" << movie.year << ")\n";
} // Prints Mother, Parasite, Snowpiercer
*/

/* -------------------- QUEUE -------------------- */

/*
#include <queue>
#include <iostream>

std::queue<Movie> watch_next;
watch_next.push(Movie{"Snowpiercer", 2013});
watch_next.push(Movie{"Parasite", 2019});
watch_next.push(Movie{"Mother", 2009});

while (!watch_next.empty()) {
    Movie movie = watch_next.front();
    watch_next.pop();
    std::cout << movie.title << " (" << movie.year << ")\n";
} // Prints Snowpiercer, Parasite, Mother
*/

/* -------------------- PRIORITY QUEUE / HEAP -------------------- */

/*
#include <queue>
#include <iostream>

std::priority_queue<Task> todo;
todo.push(Task{"email", 2});
todo.push(Task{"taxes", 3});
todo.push(Task{"laundry", 1});

while (!todo.empty()) {
    Task task = todo.top();
    todo.pop();
    std::cout << task.name << ' ' << task.priority << '\n';
} // Prints taxes, email, then laundry
*/

/* -------------------- MAP / TREES -------------------- */

/*
#include <unordered_map>
#include "trees.h"
#include <iostream>

std::unordered_map<std::string, Contact> contacts;   // Hash table
// AVLMap<std::string, Contact> contacts;            // AVL tree
// SplayMap<std::string, Contact> contacts;          // Splay tree

Contact alice{"Alice", "323-3232"};
Contact bob{"Bob", "434-4343"};
contacts[alice.name] = alice;
contacts[bob.name] = bob;
contacts.at("Alice").phone = "212-2121";

auto it = contacts.find("Bob");
if (it != contacts.end()) {
    std::cout << it->second.phone << '\n';
}

for (const auto& [name, contact] : contacts) {
    std::cout << name << ' ' << contact.phone << '\n';
}

contacts.erase("Bob");
*/

/* -------------------- SET / TREES -------------------- */

/*
#include <unordered_set>
#include "trees.h"
#include <iostream>

std::unordered_set<std::string> usernames;   // Hash table
// AVLSet<std::string> usernames;            // AVL tree
// SplaySet<std::string> usernames;          // Splay tree

usernames.insert("alice");
usernames.insert("bob");
usernames.insert("alice");   // No change

bool taken = usernames.find("bob") != usernames.end();

for (const std::string& name : usernames) {
    std::cout << name << '\n';
}

usernames.erase("bob");
*/
