/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_macros.h"

namespace gpupixel {
class GPUPIXEL_API BoxDifferenceFilter : public Filter {
 public:
  static std::shared_ptr<BoxDifferenceFilter> create();
  ~BoxDifferenceFilter();
  bool init();
  bool doRender(bool updateSinks = true) override;

  //
  void setDelta(float delta);

 protected:
  BoxDifferenceFilter();
  float delta_;
  GLuint filterTexCoordAttribute_;
  GLuint filterTexCoordAttribute2_;
};

}
