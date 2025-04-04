/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "source.h"

#if defined(GPUPIXEL_IOS)
#import <AVFoundation/AVFoundation.h>

@class VideoDataOutputSampleBufferDelegate;
#endif

namespace gpupixel {
class GPUPIXEL_API SourceCamera : public Source {
 public:
  SourceCamera();
  virtual ~SourceCamera();

  static std::shared_ptr<SourceCamera> Create();

  void SetFrameData(int width,
                    int height,
                    const void* pixels,
                    RotationMode outputRotation = RotationMode::NoRotation);
#if defined(GPUPIXEL_IOS)
  bool Init();
  bool Init(NSString* sessionPreset, AVCaptureDevicePosition cameraPosition);
  static bool IsCameraExist(AVCaptureDevicePosition cameraPosition);
  void Start();
  void Stop();
  void Pause();
  void Resume();
  bool IsRunning();
  bool Flip();

  AVCaptureDevicePosition GetCameraPosition();
  void SetOutputImageOrientation(UIInterfaceOrientation orientation);
  void SetHorizontallyMirrorFrontFacingCamera(bool newValue);
  void SetHorizontallyMirrorRearFacingCamera(bool newValue);
#endif

 private:
#if defined(GPUPIXEL_IOS)
  VideoDataOutputSampleBufferDelegate*
      video_data_output_sample_buffer_delegate_;
  AVCaptureSession* capture_session_;
  BOOL capture_paused_;
  gpupixel::RotationMode output_rotation_;
  // gpupixel::RotationMode internalRotation;
  AVCaptureDeviceInput* capture_device_input_;
  AVCaptureVideoDataOutput* capture_video_data_output_;
  /// This determines the rotation applied to the output image, based on the
  /// source material
  UIInterfaceOrientation output_image_orientation_;
  /// These properties determine whether or not the two camera orientations
  /// should be mirrored. By default, both are NO.
  bool horizontally_mirror_front_facing_camera_,
      horizontally_mirror_rear_facing_camera_;
  void UpdateOutputRotation();
#endif
  std::shared_ptr<GPUPixelFramebuffer> framebuffer_;
};

}  // namespace gpupixel

#if defined(GPUPIXEL_IOS)
@interface VideoDataOutputSampleBufferDelegate
    : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
@property(nonatomic) gpupixel::SourceCamera* sourceCamera;
@property(nonatomic) gpupixel::RotationMode rotation;
@end
#endif
