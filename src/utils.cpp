#include "utils.h"

namespace utils {

cv::Mat enhance(const cv::Mat &pic) {
  static cv::Mat lab, result;
  static std::vector<cv::Mat> lab_planes(3);

  cv::cvtColor(pic, lab, cv::COLOR_BGR2Lab);
  cv::split(lab, lab_planes);

  static cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
  clahe->apply(lab_planes[0], lab_planes[0]);

  cv::merge(lab_planes, lab);
  cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);
  return result;
}

int calculateThreshold(const cv::Mat &eyeRoi, bool log) {
  if (eyeRoi.empty())
    return 0;
  double avg = cv::mean(eyeRoi)[0];
  int th_val = static_cast<int>(avg);
#ifdef DEBUG
  std::cout << "Threshold (avg): " << th_val << std::endl;
#endif
  return static_cast<int>(0.3 * th_val);
}

} // namespace utils
