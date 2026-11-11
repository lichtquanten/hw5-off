/* EXAMPLE CODE
 * This class suggests a robot to next explore with. The approach
 * here to look at every robot; check if the robot has reached an abyss; if not,
 * check if the robot has positive signal. Then, take all of the robots with
 * positive signal and pick a random robot to recommend.
 *
 * This is not a great approach, conceptually! I'll leave it to you to think about why not.
 * This is also rather slow, computationally, and could be made more efficient,
 * even with the same not great conceptual approach.
 *
 * You should consider this an example of creating a class of your own, whether
 * for selecting a next robot or whatever else!
 */

/* BEGIN EXAMPLE - ADD YOUR OWN CLASS

#include "robot_selector.h"

#include <vector>

RobotSelector::RobotSelector(const Mothership &ship, const SeafloorSurvey &survey)
    : ship(ship), survey(survey) // EXAMPLE - INITIALIZE DATA MEMBERS
{
}

std::optional<int> RobotSelector::choose()
{
  std::vector<int> reachable;
  std::vector<int> positiveSignal;

  for (int robot = 0; robot < ROBOT_COUNT; ++robot)
  {
    int position = ship.getRobotPosition(robot);
    if (position >= ship.getAbysses()[robot] - 1)
    {
      continue;
    }

    reachable.push_back(robot);
    if (survey.getSignalStrength({robot, position}) > 0)
    {
      positiveSignal.push_back(robot);
    }
  }

  const auto &candidates = positiveSignal.empty() ? reachable : positiveSignal;
  if (candidates.empty())
  {
    return std::nullopt;
  }

  std::uniform_int_distribution<int> pick(0, static_cast<int>(candidates.size()) - 1);
  return candidates[pick(generator)];
}

END EXAMPLE - IMPLEMENT YOUR OWN CLASS */
