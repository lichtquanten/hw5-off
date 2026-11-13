#include "scenario.h"

#include <stdexcept>

void validateRobotIndex(int robotIndex, int robotCount)
{
  if (robotIndex < 0 || robotIndex >= robotCount)
  {
    throw std::out_of_range("Robot index is outside the valid range.");
  }
}
