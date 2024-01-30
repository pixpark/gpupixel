
/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

// base
#include "framebuffer.h"
#include "framebuffer_cache.h"
#include "gl_program.h"
#include "gpupixel_context.h"

// utils
#include "math_toolbox.h"
#include "util.h"

// source
#include "source.h"
#include "source_camera.h"
#include "source_image.h"
#include "source_raw_data_input.h"

// target
#include "target.h"
#include "target_raw_data_output.h"
#include "target_view.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#include "gpupixel_target.h"
#include "gpupixel_view.h"
#include "objc_target.h"
#endif

// base filters
#include "filter.h"
#include "filter_group.h"

// face filters
#include "beauty_face_filter.h"
#include "face_makeup_filter.h"
#include "face_reshape_filter.h"

#include "box_blur_filter.h"
#include "box_high_pass_filter.h"

// general filters
#include "bilateral_filter.h"
#include "brightness_filter.h"
#include "canny_edge_detection_filter.h"
#include "color_invert_filter.h"
#include "color_matrix_filter.h"
#include "contrast_filter.h"
#include "convolution3x3_filter.h"
#include "crosshatch_filter.h"
#include "directional_non_maximum_suppression_filter.h"
#include "directional_sobel_edge_detection_filter.h"
#include "emboss_filter.h"
#include "exposure_filter.h"
#include "gaussian_blur_filter.h"
#include "gaussian_blur_mono_filter.h"
#include "glass_sphere_filter.h"
#include "grayscale_filter.h"
#include "hsb_filter.h"
#include "halftone_filter.h"
#include "hue_filter.h"
#include "ios_blur_filter.h"
#include "luminance_range_filter.h"
#include "nearby_sampling3x3_filter.h"
#include "non_maximum_suppression_filter.h"
#include "pixellation_filter.h"
#include "posterize_filter.h"
#include "rgb_filter.h"
#include "saturation_filter.h"
#include "single_component_gaussian_blur_filter.h"
#include "single_component_gaussian_blur_mono_filter.h"
#include "sketch_filter.h"
#include "smooth_toon_filter.h"
#include "sobel_edge_detection_filter.h"
#include "sphere_refraction_filter.h"
#include "toon_filter.h"
#include "weak_pixel_inclusion_filter.h"
#include "white_balance_filter.h"
