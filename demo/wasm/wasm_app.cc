#include "wasm_app.h"
#include <iostream>
#include <memory>
#include <vector>
#include "gpupixel/gpupixel.h"

using namespace gpupixel;

/**
 * Global variables for managing GPUPixel components
 * These are kept in an anonymous namespace to avoid external linkage
 */
namespace {
std::shared_ptr<SourceRawData> source_raw_data_;  // Raw data source component
std::shared_ptr<SinkRender> sink_render_canvas_;  // Canvas rendering sink
std::shared_ptr<BeautyFaceFilter>
    beauty_filter_;  // Beauty filter for face processing

bool g_is_initialized = false;  // Global initialization flag
}  // namespace

/**
 * Initializes the GPUPixel system with the specified resource path
 * @param resource_path Path to GPUPixel resources
 * @return 0 on success, 1 if already initialized
 */
int Init(const char* resource_path) {
  std::cout << "Init: Starting GPUPixel initialization, resource path: "
            << resource_path << std::endl;

  if (g_is_initialized) {
    std::cout << "Init: Already initialized, skipping initialization"
              << std::endl;
    return 1;
  }

  GPUPixel::SetResourceRoot(resource_path);
  std::cout << "Init: Resource root directory set" << std::endl;

  source_raw_data_ = SourceRawData::Create();
  std::cout << "Init: SourceRawData created" << std::endl;

  sink_render_canvas_ = SinkRender::Create();
  sink_render_canvas_->SetFillMode(SinkRender::FillMode::PreserveAspectRatio);
  sink_render_canvas_->SetMirror(true);
  std::cout << "Init: SinkRender created" << std::endl;

  beauty_filter_ = BeautyFaceFilter::Create();
  std::cout << "Init: BeautyFaceFilter created" << std::endl;

  source_raw_data_->AddSink(beauty_filter_);
  beauty_filter_->AddSink(sink_render_canvas_);
  std::cout << "Init: Processing pipeline configured" << std::endl;

  g_is_initialized = true;
  std::cout << "Init: Initialization completed successfully" << std::endl;
  return 0;
}

/**
 * Cleans up and destroys all GPUPixel components
 */
void Destroy() {
  std::cout << "Destroy: Starting resource cleanup" << std::endl;

  source_raw_data_.reset();
  std::cout << "Destroy: SourceRawData released" << std::endl;

  sink_render_canvas_.reset();
  std::cout << "Destroy: SinkRender released" << std::endl;

  beauty_filter_.reset();
  std::cout << "Destroy: BeautyFaceFilter released" << std::endl;

  g_is_initialized = false;
  std::cout << "Destroy: Resource cleanup completed" << std::endl;
}

/**
 * Updates beauty parameters for face processing
 * @param smoothing Smoothing intensity (0-10)
 * @param whitening Whitening intensity (0-10)
 */
void SetBeautyParams(float smoothing, float whitening) {
  std::cout << "SetBeautyParams: Setting beauty parameters - Smoothing: "
            << smoothing << ", Whitening: " << whitening << std::endl;

  if (beauty_filter_) {
    float blurAlpha = smoothing / 10.0f;
    float whiteValue = whitening / 20.0f;

    beauty_filter_->SetBlurAlpha(blurAlpha);
    beauty_filter_->SetWhite(whiteValue);

    std::cout << "SetBeautyParams: Applied values - Blur Alpha: " << blurAlpha
              << ", White Value: " << whiteValue << std::endl;
  } else {
    std::cout << "SetBeautyParams: Warning - BeautyFaceFilter not initialized"
              << std::endl;
  }
}

/**
 * Processes a single frame of RGBA image data
 * @param rgba_data Pointer to RGBA image data
 * @param width Image width in pixels
 * @param height Image height in pixels
 */
void ProcessImage(const uint8_t* rgba_data, int width, int height) {
  if (!g_is_initialized) {
    std::cout << "ProcessImage: Error - System not initialized" << std::endl;
    return;
  }

  if (!rgba_data) {
    std::cout << "ProcessImage: Error - Image data is null" << std::endl;
    return;
  }

  if (width <= 0 || height <= 0) {
    std::cout << "ProcessImage: Error - Invalid image dimensions (width: "
              << width << ", height: " << height << ")" << std::endl;
    return;
  }

  // Frame processing logs are disabled to avoid excessive output
  // std::cout << "ProcessImage: Processing frame (width: " << width << ",
  // height: " << height << ")" << std::endl;

  sink_render_canvas_->SetRenderSize(width, height);

  source_raw_data_->ProcessData(rgba_data, width, height, width * 4,
                                GPUPIXEL_FRAME_TYPE_RGBA);
}
