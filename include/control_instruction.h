#pragma once
#include "pupil_tracker.h"

enum Instruction {
  FORWARD,
  REVERSE,
  LEFTWARD,
  RIGHTWARD,
  CHANGE_SPEED,
  HALT,
  RESUME
};
enum Speed { STOP, SLOW, MEDIUM, FAST };

class ControlInstruction {
public:
  ControlInstruction();
  void recordEyeGesture(const Position &pupilPosition);
  void processStates();
  Speed getSpeed() const;
  Instruction getInstruction() const;
  bool getStateChange() const;

private:
  Position activeState;
  Instruction instruction;
  bool stateChange;
  Speed speed;
  Speed lastSpeed;

  std::vector<Position> pupilLocation;
  static const int MAX_HISTORY = 40;
};