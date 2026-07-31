# Circular Object Tracker

**Donovan Crowley**
> donocrowley16@gmail.com

## Project Description
* Real-time computer vision pipeline built with C++ and OpenCV

---

## Architecture & State Machine
* Core engine operates on a multi-state machine architecture controlled by keyboard input:

1. **SEARCH (State 0):** Uses the Hough Circle Transform algorithm to detect spherical objects in grayscale video frames

2. **TRAIN (State 1):** Dynamically adjusts HSV (Hue, Saturation, Value) threshold ranges by evaluating internal versus external color pixels relative to the target bounds

3. **TRACK (State 2):** Calculates image moments on thresholding frames to compute continuous centroid trajectories and draw tracing lines across the video frames

4. **READ (State 3)** Samples fixed regional perimeter values over the frame to compute the average HSV color metrics on command

# Installation
* On A Mac:
**Dependencies:**
`brew install opencv cmake`

**Configure Executable**
`mkdir build`
`cd build`
`cmake ..`
`make`