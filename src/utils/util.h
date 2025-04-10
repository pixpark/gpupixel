/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <string>
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
#define rotationSwapsSize(rotation)                   \
  ((rotation) == gpupixel::RotateLeft ||              \
   (rotation) == gpupixel::RotateRight ||             \
   (rotation) == gpupixel::RotateRightFlipVertical || \
   (rotation) == gpupixel::RotateRightFlipHorizontal)

class GPUPIXEL_API Util {
 public:
  static std::string StringFormat(const char* fmt, ...);
  static void Log(const std::string& tag, std::string format, ...);
  static int64_t NowTimeMs();

  static std::string GetResourcePath(std::string name = "");
  static void SetResourceRoot(std::string root);
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
  static std::string GetResourcePath(std::string bundle_name,
                                     std::string file_name,
                                     std::string type);
#endif
#if defined(GPUPIXEL_ANDROID)
  static std::string GetResourcePathJni(std::string name);
  static std::string GetDefaultResourcePathJni(std::string name);
#endif

 private:
  static std::string resource_root_path_;
};
}  // namespace gpupixel
