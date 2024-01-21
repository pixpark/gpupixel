
/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "GPUPixelDef.h"

// Base
#include "Framebuffer.h"
#include "FramebufferCache.h"
#include "GLProgram.h"
#include "GPUPixelContext.h"

// Utils
#include "MathToolbox.h"
#include "Util.h"

// Source
#include "Source.h"
#include "SourceCamera.h"
#include "SourceImage.h"
#include "SourceRawDataInput.h"

// Target
#include "Target.h"
#include "TargetRawDataOutput.h"
#include "TargetView.h"
#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#include "GPUPixelTarget.h"
#include "GPUPixelView.h"
#include "ObjcTarget.h"
#endif

// Base Filters
#include "Filter.h"
#include "FilterGroup.h"

// Face Filters
#include "BeautyFaceFilter.h"
#include "FaceMakeupFilter.h"
#include "FaceReshapeFilter.h"

#include "BoxBlurFilter.h"
#include "BoxHighPassFilter.h"

// General Filters
#include "BilateralFilter.h"
#include "BrightnessFilter.h"
#include "CannyEdgeDetectionFilter.h"
#include "ColorInvertFilter.h"
#include "ColorMatrixFilter.h"
#include "ContrastFilter.h"
#include "Convolution3x3Filter.h"
#include "CrosshatchFilter.h"
#include "DirectionalNonMaximumSuppressionFilter.h"
#include "DirectionalSobelEdgeDetectionFilter.h"
#include "EmbossFilter.h"
#include "ExposureFilter.h"
#include "GaussianBlurFilter.h"
#include "GaussianBlurMonoFilter.h"
#include "GlassSphereFilter.h"
#include "GrayscaleFilter.h"
#include "HSBFilter.h"
#include "HalftoneFilter.h"
#include "HueFilter.h"
#include "IOSBlurFilter.h"
#include "LuminanceRangeFilter.h"
#include "NearbySampling3x3Filter.h"
#include "NonMaximumSuppressionFilter.h"
#include "PixellationFilter.h"
#include "PosterizeFilter.h"
#include "RGBFilter.h"
#include "SaturationFilter.h"
#include "SingleComponentGaussianBlurFilter.h"
#include "SingleComponentGaussianBlurMonoFilter.h"
#include "SketchFilter.h"
#include "SmoothToonFilter.h"
#include "SobelEdgeDetectionFilter.h"
#include "SphereRefractionFilter.h"
#include "ToonFilter.h"
#include "WeakPixelInclusionFilter.h"
#include "WhiteBalanceFilter.h"
