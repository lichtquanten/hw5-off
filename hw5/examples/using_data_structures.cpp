/* -------------------- STRUCTS -------------------- */

/*
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>

struct Movie {
    std::string title;
    int year;


    bool operator<(const Movie& other) const {
        // (Less than operator needed by: priority queue, ordered sets, ordered maps)
        if (year != other.year) {
            return year < other.year;
        }
        return title < other.title;
    }

    bool operator==(const Movie& other) const {
        // (Equality operator needed by: unordered set, unordered map)
        return title == other.title && year == other.year;
    }
};

struct MovieHash {
    std::size_t operator()(const Movie& movie) const {
        // (Hash operator needed by: unordered set, unordered map)
        return std::hash<std::string>()(movie.title) ^ std::hash<int>()(movie.year);
    }
};

struct Team {
    std::string name;
    int wins;
    int draws;
    int losses;

    int points() const {
        return 3 * wins + draws;
    }

    bool operator<(const Team& other) const {
        // (Less than operator needed by: priority queue, ordered sets, ordered maps)
        if (points() != other.points()) {
            return points() < other.points();
        }
        return name < other.name;
    }
};

Movie movie{"Mother", 2009};
std::cout << movie.title << '\n';                // Mother
std::cout << movie.year << '\n';                 // 2009

movie.title = "Parasite";
movie.year = 2019;
std::cout << movie.title << '\n';                // Parasite
std::cout << movie.year << '\n';                 // 2019
*/

/* -------------------- VECTOR -------------------- */

/*
#include <iostream>
#include <vector>

std::vector<Movie> movies;
movies.push_back(Movie{"Snowpiercer", 2013});
movies.push_back(Movie{"Parasite", 2019});
movies.insert(movies.begin(), Movie{"Mother", 2009});

std::cout << movies.front().title << '\n';      // Mother
std::cout << movies.back().title << '\n';       // Parasite
std::cout << movies[1].title << '\n';           // Snowpiercer
std::cout << movies.at(1).title << '\n';        // Snowpiercer (does bound checking)

for (Movie& movie : movies) {
    if (movie.title == "Parasite") {
        movie = Movie{"Okja", 2017};
        break;
    }
}

movies.erase(movies.begin());
movies.pop_back();

std::cout << movies.size() << '\n';              // 1
for (const Movie& movie : movies) {
    std::cout << movie.title << '\n';
} // Prints Snowpiercer

std::cout << movies.empty() << '\n'; // 0 (i.e. false)
*/

/* -------------------- LINKED LIST -------------------- */

/*
#include <iostream>
#include <list>

std::list<Movie> movies;
movies.push_back(Movie{"Snowpiercer", 2013});
movies.push_back(Movie{"Parasite", 2019});
movies.push_front(Movie{"Mother", 2009});

std::cout << movies.front().title << '\n';        // Mother
std::cout << movies.back().title << '\n';         // Parasite

for (auto it = movies.begin(); it != movies.end(); ++it) {
    if (it->title == "Parasite") {
        *it = Movie{"Okja", 2017}; // Replace "Parasite" with "Okja"
        auto added = movies.insert(it, Movie{"The Host", 2006}); // Add "The Host" before "Okja"
        break;
    }
}

movies.pop_front();
movies.pop_back();

std::cout << movies.size() << '\n';              // 2
for (const Movie& movie : movies) {
    std::cout << movie.title << '\n';
} // Prints Snowpiercer, The Host

std::cout << movies.empty() << '\n'; // 0 (i.e. false)
*/

/* -------------------- STACK -------------------- */

/*
#include <iostream>
#include <stack>

std::stack<Movie> movies;
movies.push(Movie{"Snowpiercer", 2013});
movies.push(Movie{"Parasite", 2019});
movies.push(Movie{"Mother", 2009});
std::cout << movies.size() << '\n';              // 3

while (!movies.empty()) {
    Movie movie = movies.top();
    movies.pop();
    std::cout << movie.title << '\n';
} // Prints Mother, Parasite, Snowpiercer
*/

/* -------------------- QUEUE -------------------- */

/*
#include <iostream>
#include <queue>

std::queue<Movie> movies;
movies.push(Movie{"Snowpiercer", 2013});
movies.push(Movie{"Parasite", 2019});
movies.push(Movie{"Mother", 2009});
std::cout << movies.size() << '\n';              // 3
std::cout << movies.back().title << '\n';        // Mother

while (!movies.empty()) {
    Movie movie = movies.front();
    movies.pop();
    std::cout << movie.title << '\n';
} // Prints Snowpiercer, Parasite, Mother
*/

/* -------------------- PRIORITY QUEUE / HEAP -------------------- */

/*
#include <iostream>
#include <queue>

std::priority_queue<Team> teams;
teams.push(Team{"Lazio", 3, 1, 1});
teams.push(Team{"Milan", 4, 0, 1});
teams.push(Team{"Roma", 2, 2, 1});
std::cout << teams.size() << '\n';               // 3

while (!teams.empty()) {
    Team team = teams.top();
    teams.pop();
    std::cout << team.name << ' ' << team.points() << '\n';
} // Prints Milan 12, Lazio 10, Roma 8
*/

/* -------------------- MAPS (UNORDERED / AVL / SPLAY) -------------------- */

/*
#include <iostream>
#include <string>
#include <unordered_map>
#include "trees.h"

std::unordered_map<std::string, Movie> favorites;
// AVLMap<std::string, Movie> favorites;
// SplayMap<std::string, Movie> favorites;

favorites.insert({"Alice", Movie{"Parasite", 2019}});
favorites["Bob"] = Movie{"Okja", 2017};
favorites["Carol"] = Movie{"Mother", 2009};

std::cout << favorites.at("Alice").title << '\n';  // Parasite

auto it = favorites.find("Bob");
if (it != favorites.end()) {
    it->second = Movie{"The Host", 2006};
}
std::cout << favorites.at("Bob").title << '\n';    // The Host

for (const auto& entry : favorites) {
    std::cout << entry.first << ' ' << entry.second.title << '\n';
} // Prints Alice Parasite, Bob The Host, Carol Mother (for AVL/Splay;
  //                                                    in any order for
  //                                                    unordered_map)

std::cout << favorites.begin()->first << '\n';     // Alice (for AVL/Splay; any item for unordered_map)
std::cout << favorites.rbegin()->first << '\n';    // Carol (AVL/Splay only)

favorites.erase("Carol");
std::cout << favorites.size() << '\n';             // 2
*/

/* -------------------- SETS (UNORDERED / AVL / SPLAY) -------------------- */

/*
#include <iostream>
#include <unordered_set>
#include "trees.h"

std::unordered_set<Movie, MovieHash> watched;
// AVLSet<Movie> watched;
// SplaySet<Movie> watched;

watched.insert(Movie{"Parasite", 2019});
watched.insert(Movie{"Okja", 2017});
watched.insert(Movie{"The Host", 2006});
watched.insert(Movie{"Parasite", 2019});
std::cout << watched.size() << '\n';                      // 3
std::cout << watched.count(Movie{"Okja", 2017}) << '\n';  // 1

for (const Movie& movie : watched) {
    std::cout << movie.title << '\n';
} // Prints The Host, Okja, Parasite (for AVL/Splay, ordered per < operator;
 //                                   in any order for unordered_set)

std::cout << watched.begin()->title << '\n';    // The Host (for AVL/Splay; any item for unordered_set)
std::cout << watched.rbegin()->title << '\n';   // Parasite (for AVL/Splay only)

watched.erase(Movie{"Okja", 2017});
std::cout << watched.count(Movie{"Okja", 2017}) << '\n';  // 0
std::cout << watched.size() << '\n';                      // 2
*/

/* -------------------- ORDERED SETS: UPDATING AN ELEMENT -------------------- */

/*
#include <iostream>
#include "trees.h"

AVLSet<Team> standings;
// SplaySet<Team> standings;

Team lazio{"Lazio", 3, 1, 1};
Team milan{"Milan", 4, 0, 1};
standings.insert(lazio);
standings.insert(milan);

for (const Team& team : standings) {
    std::cout << team.name << ' ' << team.points() << '\n';
} // Prints Lazio 10, Milan 12

std::cout << standings.rbegin()->name << '\n';   // Milan

// Remove, update, insert (no updates in-place)
standings.erase(lazio);
++lazio.wins;
standings.insert(lazio);
std::cout << standings.rbegin()->name << '\n';   // Lazio
*/
