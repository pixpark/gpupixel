/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel/filter/color_matrix_filter.h"
#include "gpupixel/gpupixel_define.h"

namespace gpupixel {
class GPUPIXEL_API HSBFilter : public ColorMatrixFilter {
 public:
  static std::shared_ptr<HSBFilter> Create();
  bool Init();

  /** Reset the filter to have no transformations.
   */
  void reset();

  /** Add a hue rotation to the filter.
   The hue rotation is in the range [-360, 360] with 0 being no-change.
   Note that this adjustment is additive, so use the reset method if you need
   to.
   */
  void rotateHue(float h);

  /** Add a saturation adjustment to the filter.
   The saturation adjustment is in the range [0.0, 2.0] with 1.0 being
   no-change. Note that this adjustment is additive, so use the reset method if
   you need to.
   */
  void adjustSaturation(float s);

  /** Add a brightness adjustment to the filter.
   The brightness adjustment is in the range [0.0, 2.0] with 1.0 being
   no-change. Note that this adjustment is additive, so use the reset method if
   you need to.
   */
  void adjustBrightness(float b);

 protected:
  HSBFilter() {};

 private:
  Matrix4 brightness_matrix_;
  Matrix4 saturation_matrix_;
};

}  // namespace gpupixel
