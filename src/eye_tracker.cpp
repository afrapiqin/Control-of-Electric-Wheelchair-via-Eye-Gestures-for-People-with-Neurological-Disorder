#include "eye_tracker.h"
#include <dlib/opencv.h>

EyeTracker::EyeTracker() {}

bool EyeTracker::initialize() {
  if (!face_cascade.load("res/haarcascade_frontalface_default.xml")) {
    return false;
  }
  try {
    dlib::deserialize("res/shape_predictor_5_face_landmarks.dat") >> predictor;
  } catch (dlib::serialization_error &e) {
    return false;
  }
  return true;
}

cv::Rect EyeTracker::detectFace(const cv::Mat &frame) {
  std::vector<cv::Rect> faces;
  face_cascade.detectMultiScale(frame, faces, 1.3, 5);
  if (!faces.empty()) {
    cv::Rect face = faces[0];
#ifdef DEBUG
    cv::Mat temp = frame.clone();
    cv::rectangle(temp, face, cv::Scalar(255, 0, 0), 1);
    cv::imwrite("Test.png", temp);
#endif
    return face;
  }
  return cv::Rect(0, 0, 0, 0);
}

cv::Rect EyeTracker::detectEye(const cv::Mat &frame) {
  cv::Rect faceRect = detectFace(frame);
  if (faceRect.width == 0)
    return faceRect;

  int top_margin, bottom_margin, left_margin, right_margin;
  dlib::cv_image<unsigned char> dlibImg(frame);
  dlib::rectangle dlibRect(faceRect.x, faceRect.y, faceRect.x + faceRect.width,
                           faceRect.y + faceRect.height);

  dlib::full_object_detection shape = predictor(dlibImg, dlibRect);
  if (shape.num_parts() < 4) {
    return cv::Rect(0, 0, 0, 0);
  }

  dlib::point p2 = shape.part(2);
  dlib::point p3 = shape.part(3);
  int eye_length = static_cast<int>((p3.x() - p2.x()) / 4.0);

  top_margin = p2.y() - eye_length * 2;
  bottom_margin = p3.y() + eye_length * 1.5;
  left_margin = p2.x();
  right_margin = p3.x();

  cv::Rect roi(left_margin, top_margin, right_margin - left_margin,
               bottom_margin - top_margin);
  if (roi.x < 0)
    roi.x = 0;
  if (roi.y < 0)
    roi.y = 0;
  if (roi.x + roi.width > frame.cols)
    roi.width = frame.cols - roi.x;
  if (roi.y + roi.height > frame.rows)
    roi.height = frame.rows - roi.y;

  if (roi.width <= 0 || roi.height <= 0)
    return cv::Rect(0, 0, 0, 0);

  // trim Lacrimal Caruncle
  int trimWidth = static_cast<int>(frame(roi).cols * 0.15); // 10% of width
  cv::Rect temp(trimWidth, 0, frame(roi).cols - trimWidth, frame(roi).rows);
  eye_roi = frame(roi)(temp).clone(); // Clone to avoid reference issues
  return roi;
}

cv::Mat EyeTracker::getEyeROI() const { return eye_roi; }