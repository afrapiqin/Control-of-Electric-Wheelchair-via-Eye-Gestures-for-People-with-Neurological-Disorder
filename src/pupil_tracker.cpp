#include "pupil_tracker.h"
#include "utils.h"

PupilTracker::PupilTracker() : pupil(-1, -1), width(0), height(0) {}

void PupilTracker::detectPupil(const cv::Mat &eyeRoi) {
  // Apply Gaussian blur to reduce noise
  cv::Mat blurred_eye;
  cv::GaussianBlur(eyeRoi, blurred_eye, cv::Size(7, 7), 0);

  // Binary thresholding
  cv::Mat binary_eye;
  int threshold_value = utils::calculateThreshold(eyeRoi, false);
  cv::threshold(blurred_eye, binary_eye, threshold_value, 255,
                cv::THRESH_BINARY);

  // Morphological operations to clean up the image
  // Erode and dilate to remove small noise
  cv::erode(binary_eye, binary_eye, cv::Mat(), cv::Point(-1, -1), 2);
  cv::dilate(binary_eye, binary_eye, cv::Mat(), cv::Point(-1, -1), 2);

  // Apply median blur to smooth edges
  cv::medianBlur(binary_eye, binary_eye, 5);

  // Close operation to fill gaps
  cv::Mat elliptical_kernel =
      cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
  cv::morphologyEx(binary_eye, binary_eye, cv::MORPH_CLOSE, elliptical_kernel);

  // Invert the binary image to get white pupil on black background
  cv::Mat invertedEye;
  cv::bitwise_not(binary_eye, invertedEye);

  // Calculate image moments to find pupil center
  cv::Moments moments = cv::moments(invertedEye, true);

  if (moments.m00 != 0) {
    pupil.first = static_cast<int>(moments.m10 / moments.m00);
    pupil.second = static_cast<int>(moments.m01 / moments.m00);
  } else {
    pupil.first = -1;
    pupil.second = -1;
  }
#ifdef DEBUG
  cv::imwrite("Test2.png", eye_bin);
#endif

  // Get image dimensions
  width = eyeRoi.cols;
  height = eyeRoi.rows;

  // Define region centers
  regionCenters = {{Position::TOP, {width / 2, height / 4}},
                   {Position::BOTTOM, {width / 2, 3 * height / 4}},
                   {Position::RIGHT, {width / 4, height / 2}},
                   {Position::LEFT, {3 * width / 4, height / 2}},
                   {Position::MIDDLE, {width / 2, height / 2}}};
}

// Function to compute Euclidean distance
double euclideanDistance(int x1, int y1, int x2, int y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void PupilTracker::detectLocation(const cv::Mat &frame) {
  // Compute distances and assign inverse weights
  std::map<Position, double> weights;
  double totalWeight = 0.0;

  if (pupil.first == -1 && pupil.second == -1) {
    pupil_position = Position::NONE;
  } else {

    for (const auto &region : regionCenters) {
      double distance = euclideanDistance(
          pupil.first, pupil.second, region.second.first, region.second.second);
      double weight =
          (distance > 0) ? 1.0 / distance : 1.0; // Avoid division by zero
      weights[region.first] = weight;
      totalWeight += weight;
    }

    // Normalize weights
    for (auto &weight : weights) {
      weight.second /= totalWeight;
    }

    // Determine the best classification (region with the highest weight)
    double maxWeight = 0.0;

    for (const auto &weight : weights) {
      // std::cout << "Weight for " << weight.first << ": " << weight.second
      //           << std::endl;
      if (weight.second > maxWeight) {
        maxWeight = weight.second;
        pupil_position = weight.first;
      }
    }
  }
}

Position PupilTracker::getPosition() const { return pupil_position; }