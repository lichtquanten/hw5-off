#pragma once

#include <vector>

#include "mothership.h"
#include "trees.h"

class Search
{
public:
  explicit Search(Mothership &mothership);
  SearchResult findShipwreck();

private:
  struct Rank
  {
    int signal;
    int position;
    int robot;
    bool operator<(const Rank &other) const;
  };

  Mothership &mothership;
  int robotCount;
  std::vector<int> ends;
  std::vector<int> positions;
  std::vector<int> signals;
  std::vector<std::vector<int>> debris;
  AVLSet<Rank> waiting;

  int leftOf(int robot) const;
  int rightOf(int robot) const;
  bool hasRemainingPath(int robot) const;
  Rank rank(int robot) const;
  int measureSignal(int robot) const;
  void refreshWaitingSignal(int robot);
};
