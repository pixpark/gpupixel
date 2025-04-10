/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>

#include "gpupixel/source/source.h"
namespace gpupixel {
class GPUPIXEL_API SourceImage : public Source {
 public:
  static std::shared_ptr<SourceImage> Create(const std::string name);

  static std::shared_ptr<SourceImage> CreateFromBuffer(
      int width,
      int height,
      int channel_count,
      const unsigned char* pixels);

  ~SourceImage() {};

  const unsigned char* GetRgbaImageBuffer() const;
  int GetWidth() const;
  int GetHeight() const;

  void Render();

  void Init(int width,
            int height,
            int channel_count,
            const unsigned char* pixels);

#if defined(GPUPIXEL_ANDROID)
  static std::shared_ptr<SourceImage> CreateImageForAndroid(std::string name);
#endif
  std::vector<unsigned char> image_bytes_;

 private:
  SourceImage() {}
};

}  // namespace gpupixel
