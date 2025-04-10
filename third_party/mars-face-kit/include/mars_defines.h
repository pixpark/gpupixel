//
//  mars-face-kit
//
//  Created by PixPark on 2024/11/18.
//
#pragma once

#include <string>
#include <vector>
#include <memory>
// version
#define MARS_FACE_SDK_VERSION "v0.0.2"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
//define something for Windows (64-bit only)
#define MARS_FACE_WIN
#else
//define something for Windows (32-bit only)
#define MARS_FACE_WIN
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#define MARS_FACE_IOS
#elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#define MARS_FACE_IOS
#elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#define MARS_FACE_IOS
#elif TARGET_OS_MAC
// Other kinds of Apple platforms
#define MARS_FACE_MAC
#else
#   error "Unknown Apple platform"
#endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#define MARS_FACE_ANDROID
#elif __linux__
// linux
#define MARS_FACE_LINUX
#elif __emscripten__
#define MARS_FACE_LINUX
#elif __unix__ // all unices not caught above
// Unix
#define MARS_FACE_LINUX
#elif defined(_POSIX_VERSION)
// POSIX
#   error "Unknown Platform"
#define MARS_FACE_POSIX
#else
#   error "Unknown Platform"
#endif

#ifndef MARS_FACE_API
  #ifdef MARS_FACE_WIN
    #define MARS_FACE_API __declspec(dllexport)
  #else
    #define MARS_FACE_API
 #endif
#endif

namespace mars_face_kit {
struct Point2D {
    float x;
    float y;
};

struct Point3D {
    float x;
    float y;
    float z;
    float visibility;  // landmark score
    float presence;     // landmark in frame score
};

struct Rect {
    float left;
    float top;
    float right;
    float bottom;
};

struct FaceDetectionInfo {
    Rect rect;
    std::vector<Point2D> landmarks;
    std::vector<Point3D> landmarks3d;
    
    float visibilities;
    int face_id;
    /// Confidence score for the input frame to be a face.
    float score;
    /// Pitch angle, up -, down +. See
    float pitch;
    /// Roll angle, left -, right +. See
    float roll;
    /// Yaw angle, left -, right +. See
    float yaw;
    
    float angle;
    
    uint32_t face_action;
};

enum PixelFormat {
    RGBA = 0,
    RGB = 1,
    BGR = 2,
    GRAY = 3,
    BGRA = 4,
    YCrCb = 5,
    YUV_NV21 = 6,
    YUV_NV12 = 7,
    YUV_I420 = 8,
};

enum RotateType {
    CLOCKWISE_ROTATE_0 = 0,
    CLOCKWISE_ROTATE_90 = 1,
    CLOCKWISE_ROTATE_180 = 2,
    CLOCKWISE_ROTATE_270 = 3,
};

typedef struct {
    uint8_t *data;
    PixelFormat pixel_format;
    int width;
    int height;
    int width_step;
    double timestamp;
    RotateType rotate_type;
} MarsImage;

struct Embedding {
    std::vector<float> float_embedding;
    std::string quantized_embedding;
};

struct ClassifierInfo {
    int id_;
    float score_;
};

}
