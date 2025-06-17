/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <string>
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN
#define rotationSwapsSize(rotation)                   \
  ((rotation) == gpupixel::RotateLeft ||              \
   (rotation) == gpupixel::RotateRight ||             \
   (rotation) == gpupixel::RotateRightFlipVertical || \
   (rotation) == gpupixel::RotateRightFlipHorizontal)

class Util {
 public:
  static std::string str_format(const char* fmt, ...);
  static void Log(const std::string& tag, const std::string& format, ...);
  static int64_t nowTimeMs();

  static std::string getResourcePath(std::string name);
    
  /// 将视频图像帧流数据转成RGBA图像数据
  /// - Parameters:
  ///   - inputData: 输入数据
  ///   - width: 图像的宽度
  ///   - height: 图像的高度
  ///   - stride: 视频图像帧流数据的步长
  static uint8_t* createImageBuffer(const uint8_t* inputData, int width, int height, int stride);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  static std::string getResourcePath(std::string bundle_name,
                                     std::string file_name,
                                     std::string type);
#endif
#if defined(GPUPIXEL_ANDROID)
  static std::string getResourcePathJni(std::string name);
#endif
};
NS_GPUPIXEL_END
