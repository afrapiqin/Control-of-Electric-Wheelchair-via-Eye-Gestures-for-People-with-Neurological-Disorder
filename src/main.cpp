#include <iostream>
#include <mutex>
#include <thread>

#include "control_instruction.h"
#include "eye_tracker.h"
#include "pupil_tracker.h"
#include "utils.h"

// Enable OpenCV optimization flags
#if defined(__AVX2__)
cv::setUseOptimized(true);
cv::setNumThreads(4); // Adjust based on CPU cores
#endif

std::mutex frameMutex;
cv::Mat latestFrame;

void captureThread(cv::VideoCapture &cap) {
  while (true) {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty())
      continue;

    std::lock_guard<std::mutex> lock(frameMutex);
    frame.copyTo(latestFrame);
  }
}

int main() {
  std::cout << "Launching..." << std::endl;
  PupilTracker pupilTracker;
  EyeTracker eyeTracker;
  ControlInstruction controlInstruction;

  if (!eyeTracker.initialize()) {
    std::cerr << "Failed to initialize eye tracker" << std::endl;
    return -1;
  }

  cv::VideoCapture cap(1);
  if (!cap.isOpened()) {
    std::cerr << "Error opening video stream" << std::endl;
    return -1;
  }

  std::thread capture(captureThread, std::ref(cap));
  capture.detach();

  while (true) {
    try {
      cv::Mat currentFrame;
      {
        std::lock_guard<std::mutex> lock(frameMutex);
        if (latestFrame.empty())
          continue;
        currentFrame = latestFrame.clone();
      }

      // Preprocess image
      cv::Mat enhancedFrame = utils::enhance(currentFrame);
      cv::Mat grayFrame;
      cv::cvtColor(enhancedFrame, grayFrame, cv::COLOR_BGR2GRAY);

      // Detect eye region
      cv::Rect eyeRegion = eyeTracker.detectEye(grayFrame);
      if (eyeRegion.width == 0) {
        continue;
      }

      // Detect pupil location
      pupilTracker.detectPupil(eyeTracker.getEyeROI());
      pupilTracker.detectLocation(grayFrame);

      // Translate control instruction
      controlInstruction.recordEyeGesture(pupilTracker.getPosition());
      controlInstruction.processStates();

      if (controlInstruction.getStateChange()) {
        switch (controlInstruction.getInstruction()) {
        case FORWARD:
          std::cout << "Moving Forward" << std::endl;
          break;
        case REVERSE:
          std::cout << "Reversing" << std::endl;
          break;
        case LEFTWARD:
          std::cout << "Turning Left" << std::endl;
          break;
        case RIGHTWARD:
          std::cout << "Turning Right" << std::endl;
          break;
        case RESUME:
        case HALT:
        case CHANGE_SPEED:
          std::cout << "Speed change to: " << controlInstruction.getSpeed()
                    << std::endl;
          break;
        }
      }
    } catch (const std::exception &e) {
      std::cerr << "Frame processing error: " << e.what() << std::endl;
      continue;
    }
  }

  cap.release();
  cv::destroyAllWindows();

  return 0;
}
