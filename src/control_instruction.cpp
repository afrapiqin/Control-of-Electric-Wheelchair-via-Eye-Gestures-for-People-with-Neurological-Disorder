#include "control_instruction.h"

ControlInstruction::ControlInstruction()
    : activeState(NONE), instruction(RESUME), speed(SLOW), lastSpeed(SLOW) {}

void ControlInstruction::recordEyeGesture(const Position &state) {
  pupilLocation.push_back(state);
  if (pupilLocation.size() > MAX_HISTORY) {
    pupilLocation.erase(pupilLocation.begin());
  }
}

void ControlInstruction::processStates() {
  stateChange = false;
  // Get last 5 states if available
  std::map<Position, int> stateCounts;
  int count = 0;
  std::queue<Position> tempQueue;
  // Iterate from the back for the last MAX_HISTORY states
  for (auto it = pupilLocation.rbegin();
       it != pupilLocation.rend() && tempQueue.size() < 5; ++it) {
    tempQueue.push(*it);
  }

  while (!tempQueue.empty() && count < 5) {
    Position state = tempQueue.front();
    stateCounts[state]++;
    tempQueue.pop();
    count++;
  }

  // Find majority state
  Position majorityState = activeState;
  int maxCount = 0;

  for (const auto &pair : stateCounts) {
    if (pair.second > maxCount) {
      maxCount = pair.second;
      majorityState = pair.first;
    }
  }

  // Update active state if majority is different
  if (majorityState != activeState) {
    activeState = majorityState;
    stateChange = true;

    switch (activeState) {
    case TOP:
      instruction = CHANGE_SPEED;
      switch (speed) {
      case STOP:
        speed = STOP;
        break;
      case SLOW:
        speed = MEDIUM;
        break;
      case MEDIUM:
        speed = FAST;
        break;
      case FAST:
        speed = SLOW;
        break;
      }
      break;
    case BOTTOM:
      if (speed != STOP) {
        instruction = REVERSE;
      }
      break;
    case MIDDLE:
      if (speed != STOP) {
        instruction = FORWARD;
      }
      break;
    case LEFT:
      if (speed != STOP) {
        instruction = LEFTWARD;
      }
      break;
    case RIGHT:
      if (speed != STOP) {
        instruction = RIGHTWARD;
      }
      break;
    case NONE:
      if (speed != STOP) {
        instruction = HALT;
        lastSpeed = speed;
        speed = STOP;
      } else {
        instruction = RESUME;
        speed = lastSpeed;
      }
      break;
    }
  }
}

Speed ControlInstruction::getSpeed() const { return speed; }

Instruction ControlInstruction::getInstruction() const { return instruction; }

bool ControlInstruction::getStateChange() const { return stateChange; }
