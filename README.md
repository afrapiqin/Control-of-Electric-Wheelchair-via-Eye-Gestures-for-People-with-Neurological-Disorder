# Control of Electric Wheelchair via Eye Gestures
This project presents a video-processing-based control system for an electric wheelchair, designed for individuals with tetraplegia or severe mobility impairments. The system classifies different eye gestures to generate movement instructions for the wheelchair, providing an alternative to traditional joystick-based control.

Full Paper: [IEEE](https://ieeexplore.ieee.org/document/9530744)

## Methodology
The system captures real-time video using a camera module attached to a Raspberry Pi. It processes the frames to detect and classify eye gestures, which are then mapped to movement commands for the wheelchair.

### Eye Gesture Detection Process:
1. **Face Detection**: Utilizes OpenCV's Haar Cascade classifier to identify the face region.
2. **Right Eye Detection**: Dlib's 5-point facial detector extracts the right eye region.
3. **Iris Detection**: Image enhancement and morphological operations are applied to locate the iris.
4. **Eye Gesture Classification**: Based on iris centroid position, the system determines movement directions:
   - Left: Move left
   - Right: Move right
   - Up: Adjust speed
   - Down: Reverse
   - Middle: Move forward
   - Closed eyes: Start/stop wheelchair movement

## Hardware Requirements
- Raspberry Pi with a camera module
- DC motor control interface
- Adjustable focus lens (3.6mm, 60° field of view)

## Performance Evaluation
The system was tested under different lighting conditions, achieving the following detection accuracy:
- **Bad lighting:** 73.5%
- **Moderate lighting:** 92.83%
- **Good lighting:** 99.83%

## Acknowledgment
This project is supported by the Advanced Rehabilitation Engineering in Diagnostic and Monitoring (AREDiM) Research Group and Universiti Teknologi MARA.