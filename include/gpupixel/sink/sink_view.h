/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/sink/sink.h"
#include <memory>

namespace gpupixel {

class GPUPIXEL_API SinkView : public Sink {
 public:
  static std::shared_ptr<SinkView> Create(void* parent_view);
 
  virtual ~SinkView();

  virtual void Render() override;

  virtual void SetInputFramebuffer(
      std::shared_ptr<GPUPixelFramebuffer> framebuffer,
      RotationMode rotation_mode = NoRotation,
      int texIdx = 0) override;

  virtual bool IsReady() const override;

  virtual void ResetAndClean() override;
 private:
  SinkView(void* parent_view);

  void* render_view_;
};

}  // namespace gpupixel
