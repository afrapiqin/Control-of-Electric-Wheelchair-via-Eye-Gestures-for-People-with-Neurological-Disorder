#pragma once
#include <opencv2/opencv.hpp>

namespace utils
{
cv::Mat enhance(const cv::Mat &pic);
int calculateThreshold(const cv::Mat &eyeRoi, bool log = false);
}
