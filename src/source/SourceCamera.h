/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Source.h"

#if defined(GPUPIXEL_IOS)
#import <AVFoundation/AVFoundation.h>

@class VideoDataOutputSampleBufferDelegate;
#endif

NS_GPUPIXEL_BEGIN

class SourceCamera : public Source {
 public:
  SourceCamera();
  virtual ~SourceCamera();

  static std::shared_ptr<SourceCamera> create();

  void setFrameData(int width,
                    int height,
                    const void* pixels,
                    RotationMode outputRotation = RotationMode::NoRotation);
#if defined(GPUPIXEL_IOS)
  bool init();
  bool init(NSString* sessionPreset, AVCaptureDevicePosition cameraPosition);
  static bool isCameraExist(AVCaptureDevicePosition cameraPosition);
  void start();
  void stop();
  void pause();
  void resume();
  bool isRunning();
  bool flip();

  AVCaptureDevicePosition getCameraPosition();
  void setOutputImageOrientation(UIInterfaceOrientation orientation);
  void setHorizontallyMirrorFrontFacingCamera(bool newValue);
  void setHorizontallyMirrorRearFacingCamera(bool newValue);
#endif

 private:
#if defined(GPUPIXEL_IOS)
  VideoDataOutputSampleBufferDelegate* _videoDataOutputSampleBufferDelegate;
  AVCaptureSession* _captureSession;
  BOOL _capturePaused;
  GPUPixel::RotationMode _outputRotation;
  // GPUPixel::RotationMode internalRotation;
  AVCaptureDeviceInput* _captureDeviceInput;
  AVCaptureVideoDataOutput* _captureVideoDataOutput;
  /// This determines the rotation applied to the output image, based on the
  /// source material
  UIInterfaceOrientation _outputImageOrientation;
  /// These properties determine whether or not the two camera orientations
  /// should be mirrored. By default, both are NO.
  bool _horizontallyMirrorFrontFacingCamera,
      _horizontallyMirrorRearFacingCamera;
  void _updateOutputRotation();
#endif
  std::shared_ptr<Framebuffer> _framebuffer;
};

NS_GPUPIXEL_END

#if defined(GPUPIXEL_IOS)
@interface VideoDataOutputSampleBufferDelegate
    : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>
@property(nonatomic) GPUPixel::SourceCamera* sourceCamera;
@property(nonatomic) GPUPixel::RotationMode rotation;
@end
#endif
