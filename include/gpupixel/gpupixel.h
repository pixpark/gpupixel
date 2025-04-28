/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

// core
#include "gpupixel/gpupixel_define.h"
// utils
#include "gpupixel/utils/math_toolbox.h"

// source
#include "gpupixel/source/source.h"
#include "gpupixel/source/source_image.h"
#include "gpupixel/source/source_raw_data.h"

// sink
#include "gpupixel/sink/sink.h"
#include "gpupixel/sink/sink_raw_data.h"
#include "gpupixel/sink/sink_render.h"
#if defined(GPUPIXEL_MAC) || defined(GPUPIXEL_IOS)
#include "gpupixel/sink/sink_view.h"
#endif
// face detect
#include "gpupixel/face_detector/face_detector.h"

// base filters
#include "gpupixel/filter/filter.h"
#include "gpupixel/filter/filter_group.h"

// face filters
#include "gpupixel/filter/beauty_face_filter.h"
#include "gpupixel/filter/blusher_filter.h"
#include "gpupixel/filter/face_makeup_filter.h"
#include "gpupixel/filter/face_reshape_filter.h"
#include "gpupixel/filter/lipstick_filter.h"

// general filters
#include "gpupixel/filter/bilateral_filter.h"
#include "gpupixel/filter/box_blur_filter.h"
#include "gpupixel/filter/box_high_pass_filter.h"
#include "gpupixel/filter/brightness_filter.h"
#include "gpupixel/filter/canny_edge_detection_filter.h"
#include "gpupixel/filter/color_invert_filter.h"
#include "gpupixel/filter/color_matrix_filter.h"
#include "gpupixel/filter/contrast_filter.h"
#include "gpupixel/filter/convolution3x3_filter.h"
#include "gpupixel/filter/crosshatch_filter.h"
#include "gpupixel/filter/directional_non_maximum_suppression_filter.h"
#include "gpupixel/filter/directional_sobel_edge_detection_filter.h"
#include "gpupixel/filter/emboss_filter.h"
#include "gpupixel/filter/exposure_filter.h"
#include "gpupixel/filter/gaussian_blur_filter.h"
#include "gpupixel/filter/gaussian_blur_mono_filter.h"
#include "gpupixel/filter/glass_sphere_filter.h"
#include "gpupixel/filter/grayscale_filter.h"
#include "gpupixel/filter/halftone_filter.h"
#include "gpupixel/filter/hsb_filter.h"
#include "gpupixel/filter/hue_filter.h"
#include "gpupixel/filter/ios_blur_filter.h"
#include "gpupixel/filter/luminance_range_filter.h"
#include "gpupixel/filter/nearby_sampling3x3_filter.h"
#include "gpupixel/filter/non_maximum_suppression_filter.h"
#include "gpupixel/filter/pixellation_filter.h"
#include "gpupixel/filter/posterize_filter.h"
#include "gpupixel/filter/rgb_filter.h"
#include "gpupixel/filter/saturation_filter.h"
#include "gpupixel/filter/single_component_gaussian_blur_filter.h"
#include "gpupixel/filter/single_component_gaussian_blur_mono_filter.h"
#include "gpupixel/filter/sketch_filter.h"
#include "gpupixel/filter/smooth_toon_filter.h"
#include "gpupixel/filter/sobel_edge_detection_filter.h"
#include "gpupixel/filter/sphere_refraction_filter.h"
#include "gpupixel/filter/toon_filter.h"
#include "gpupixel/filter/weak_pixel_inclusion_filter.h"
#include "gpupixel/filter/white_balance_filter.h"

namespace gpupixel {

/**
 * GPUPixel Utility Class: Provides resource path management functionality
 */
class GPUPIXEL_API GPUPixel {
 public:
  /**
   * Set resource root directory
   * @param root Root directory path
   */
  static void SetResourcePath(const std::string& path);
};

}  // namespace gpupixel
