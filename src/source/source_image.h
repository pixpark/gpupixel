/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <string>

#include "source.h"

namespace gpupixel {
class GPUPIXEL_API SourceImage : public Source {
 public:
  SourceImage() {}
  ~SourceImage() {};

  void Init(int width,
            int height,
            int channel_count,
            const unsigned char* pixels);
  static std::shared_ptr<SourceImage> Create(const std::string name);

  static std::shared_ptr<SourceImage> CreateFromBuffer(
      int width,
      int height,
      int channel_count,
      const unsigned char* pixels);

  const unsigned char* GetRgbaImageBuffer() const;
  int GetWidth() const;
  int GetHeight() const;

  void Render();

 private:
#if defined(GPUPIXEL_ANDROID)
  static std::shared_ptr<SourceImage> CreateImageForAndroid(std::string name);
#endif
  std::vector<unsigned char> image_bytes_;
};

}  // namespace gpupixel
