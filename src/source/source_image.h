/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "source.h"

NS_GPUPIXEL_BEGIN

class SourceImage : public Source {
 public:
  SourceImage() {}
  ~SourceImage() {};

    void init(int width,
              int height,
              int channel_count,
              const unsigned char* pixels);
  static std::shared_ptr<SourceImage> create(const std::string name);

  static std::shared_ptr<SourceImage> create_from_memory(int width,
                                            int height,
                                            int channel_count,
                                            const unsigned char* pixels);
  void Render();
 private:
#if defined(GPUPIXEL_ANDROID)
    static std::shared_ptr<SourceImage> createImageForAndroid(std::string name);
#endif
  std::vector<unsigned char> image_bytes;
};

NS_GPUPIXEL_END
