#pragma once
#include <opencv2/opencv.hpp>

enum Position { TOP, BOTTOM, MIDDLE, LEFT, RIGHT, NONE };

class PupilTracker {
public:
  PupilTracker();
  void reset();
  void detectPupil(const cv::Mat &eyeRoi);
  void detectLocation(const cv::Mat &frame);
  Position getPosition() const;

private:
  std::pair<int, int> pupil;
  int width;
  int height;
  Position pupil_position;

  std::map<Position, std::pair<int, int>> regionCenters;
};
