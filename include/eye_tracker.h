#pragma once
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <opencv2/opencv.hpp>

class EyeTracker {
public:
  EyeTracker();
  bool initialize();
  cv::Rect detectEye(const cv::Mat &frame);
  cv::Mat getEyeROI() const;

private:
  cv::Rect detectFace(const cv::Mat &frame);
  cv::CascadeClassifier face_cascade;
  dlib::shape_predictor predictor;
  cv::Mat eye_roi;
};
