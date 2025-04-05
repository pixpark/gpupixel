# Face Landmarks

## Overview
GPUPixel use Mars Face detect face landmarks, Mars Face SDK  a high-performance face detection and facial landmark detection solution based on MNN (Mobile Neural Network) framework. It provides accurate face detection and returns 106 basic facial landmarks, with an extension to 111 landmarks for more detailed facial feature analysis.

## Features
- High-performance face detection based on MNN framework
- 106 basic facial landmarks detection
- Extended 111 facial landmarks
- Cross-platform support
- Easy-to-use API interface

## Basic Facial Landmarks
The SDK detects 106 basic facial landmarks that cover key facial features including:
- Eyes contours
- Eyebrows
- Nose
- Mouth outline
- Face contour

## Extended Landmarks
Based on the 106 basic landmarks, the SDK calculates 5 additional landmarks (107-111) using specific facial feature points:

- Point 106: Midpoint between landmarks 102 and 98
- Point 107: Midpoint between landmarks 35 and 65
- Point 108: Midpoint between landmarks 70 and 40
- Point 109: Midpoint between landmarks 5 and 80
- Point 110: Midpoint between landmarks 81 and 27

These additional points provide more detailed facial feature information for advanced applications.

## Face Landmark Dot

![](../../image/landmark-dot.jpg)

## Integration Guide

### Initialization
```cpp
#include "face_detector.h"

// Create face detector instance
std::shared_ptr<FaceDetector> faceDetector = std::make_shared<FaceDetector>();
```

### Face Detection and Landmark Extraction
```cpp
// Perform detection on image data and get landmarks directly
std::vector<float> landmarks = faceDetector->Detect(
    buffer,                     // image data pointer (const unsigned char*)
    width,                      // image width (int)
    height,                     // image height (int)
    int stride,                 //
    GPUPIXEL_MODE_FMT_PICTURE,  // image format (GPUPIXEL_MODE_FMT)
    GPUPIXEL_FRAME_TYPE_RGBA    // frame type (GPUPIXEL_FRAME_TYPE)
);

// Check if landmarks were detected
if (!landmarks.empty()) {
    // Apply landmarks to filters
    lipstickFilter->SetFaceLandmarks(landmarks);
    blusherFilter->SetFaceLandmarks(landmarks);
    reshapeFilter->SetFaceLandmarks(landmarks);
}
```

### Image Format Support
The detector supports various input formats:

**Format Parameter (GPUPIXEL_MODE_FMT)**:
- `GPUPIXEL_MODE_FMT_VIDEO`: For video source
- `GPUPIXEL_MODE_FMT_PICTURE`: For still images

**Frame Type Parameter (GPUPIXEL_FRAME_TYPE)**:
- `GPUPIXEL_FRAME_TYPE_YUVI420`: YUV I420 format
- `GPUPIXEL_FRAME_TYPE_RGBA`: RGBA format
- `GPUPIXEL_FRAME_TYPE_BGRA`: BGRA format

## Performance Considerations
- The detector is optimized for both real-time video and still image processing
- Landmark coordinates are normalized to [0,1] range
- Detection results are returned directly as a vector of float values

## API Reference

### FaceDetector Class
```cpp
class FaceDetector {
public:
    FaceDetector();
    ~FaceDetector();
    
    // Perform face detection on image data and return landmarks
    std::vector<float> Detect(
        const uint8_t* data,         // image data pointer
        int width,                   // image width
        int height,                  // image height
        GPUPIXEL_MODE_FMT fmt,       // image format
        GPUPIXEL_FRAME_TYPE type     // frame type
    );
};
```

### Landmark Result Format
The returned landmarks vector contains normalized (x,y) coordinates for all detected points, arranged sequentially:
- For each landmark point, two consecutive float values represent its (x,y) coordinates
- The coordinates are normalized to the image dimensions (range 0.0 to 1.0)
- If no face is detected, an empty vector is returned