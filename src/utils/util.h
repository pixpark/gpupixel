/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <cassert>
#include <string>
#include "gpupixel/gpupixel_define.h"
#include "utils/filesystem.h"
namespace gpupixel {
#define rotationSwapsSize(rotation)                   \
  ((rotation) == gpupixel::RotateLeft ||              \
   (rotation) == gpupixel::RotateRight ||             \
   (rotation) == gpupixel::RotateRightFlipVertical || \
   (rotation) == gpupixel::RotateRightFlipHorizontal)

class GPUPIXEL_API Util {
 public:
  static std::string StringFormat(const char* fmt, ...);
  static int64_t NowTimeMs();

  static void SetResourcePath(const fs::path& path);
  static fs::path GetResourcePath();

  static bool IsAppleAppActive();

 private:
  static fs::path resource_root_path_;
};
}  // namespace gpupixel
