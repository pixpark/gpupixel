/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_camera.h"
#include "gpupixel_context.h"
#include "util.h"

namespace gpupixel {
SourceCamera::SourceCamera() {
#if defined(GPUPIXEL_IOS)
  video_data_output_sample_buffer_delegate_ =
      [[VideoDataOutputSampleBufferDelegate alloc] init];
  video_data_output_sample_buffer_delegate_.sourceCamera = this;

  horizontally_mirror_front_facing_camera_ = false;
  horizontally_mirror_rear_facing_camera_ = false;
#endif
}

SourceCamera::~SourceCamera() {
#if defined(GPUPIXEL_IOS)
  Stop();
  video_data_output_sample_buffer_delegate_ = 0;
#endif
}

std::shared_ptr<SourceCamera> SourceCamera::Create() {
  auto sourceCamera = std::shared_ptr<SourceCamera>(new SourceCamera());
#if defined(GPUPIXEL_IOS)
  if (!sourceCamera->Init()) {
    sourceCamera.reset();
  }
#endif
  return sourceCamera;
}

void SourceCamera::SetFrameData(
    int width,
    int height,
    const void* pixels,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  if (!framebuffer_ || (framebuffer_->GetWidth() != width ||
                        framebuffer_->GetHeight() != height)) {
    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(width, height, true);
  }
  this->SetFramebuffer(framebuffer_, outputRotation);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->GetFramebuffer()->GetTexture()));
#if defined(GPUPIXEL_IOS)
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
                        GL_UNSIGNED_BYTE, pixels));
#elif defined(GPUPIXEL_ANDROID)
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, pixels));
#endif
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

#if defined(GPUPIXEL_IOS)
bool SourceCamera::Init() {
  if (IsCameraExist(AVCaptureDevicePositionFront)) {
    return Init(AVCaptureSessionPreset640x480, AVCaptureDevicePositionFront);
  } else {
    return Init(AVCaptureSessionPreset640x480, AVCaptureDevicePositionBack);
  }
}

bool SourceCamera::Init(NSString* sessionPreset,
                        AVCaptureDevicePosition cameraPosition) {
  output_rotation_ = gpupixel::NoRotation;
  // internalRotation = gpupixel::NoRotation;
  capture_paused_ = NO;

  capture_session_ = [[AVCaptureSession alloc] init];
  capture_session_.sessionPreset = sessionPreset;

  // input
  AVCaptureDevice* device = 0;
  for (AVCaptureDevice* dev in
       [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo]) {
    if ([dev position] == cameraPosition) {
      device = dev;
      break;
    }
  }
  if (!device) {
    return false;
  }

  NSError* error = nil;
  capture_device_input_ =
      [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
  if ([capture_session_ canAddInput:capture_device_input_]) {
    [capture_session_ addInput:capture_device_input_];
  } else {
    return false;
  }

  // output
  capture_video_data_output_ = [[AVCaptureVideoDataOutput alloc] init];
  [capture_video_data_output_ setAlwaysDiscardsLateVideoFrames:YES];
  [capture_session_ addOutput:capture_video_data_output_];
  dispatch_queue_t queue =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
  [capture_video_data_output_
      setSampleBufferDelegate:video_data_output_sample_buffer_delegate_
                        queue:queue];
  capture_video_data_output_.videoSettings = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSNumber
                                       numberWithInt:kCVPixelFormatType_32BGRA],
                                   kCVPixelBufferPixelFormatTypeKey, nil];

  SetOutputImageOrientation(UIInterfaceOrientationPortrait);

  return true;
}

bool SourceCamera::IsCameraExist(AVCaptureDevicePosition cameraPosition) {
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for (AVCaptureDevice* device in devices) {
    if ([device position] == cameraPosition) {
      return true;
    }
  }
  return false;
}

void SourceCamera::Start() {
  if (![capture_session_ isRunning]) {
    video_data_output_sample_buffer_delegate_.sourceCamera = this;
    [capture_session_ startRunning];
  };
}

void SourceCamera::Stop() {
  if ([capture_session_ isRunning]) {
    video_data_output_sample_buffer_delegate_.sourceCamera = 0;
    [capture_session_ stopRunning];
  }
}

void SourceCamera::Pause() {
  capture_paused_ = true;
}

void SourceCamera::Resume() {
  capture_paused_ = false;
}

bool SourceCamera::IsRunning() {
  return [capture_session_ isRunning];
}

bool SourceCamera::Flip() {
  AVCaptureDevicePosition cameraPosition =
      [[capture_device_input_ device] position];
  if (cameraPosition == AVCaptureDevicePositionBack) {
    cameraPosition = AVCaptureDevicePositionFront;
  } else {
    cameraPosition = AVCaptureDevicePositionBack;
  }

  if (!IsCameraExist(cameraPosition)) {
    return false;
  }

  AVCaptureDevice* device = 0;
  for (AVCaptureDevice* dev in
       [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo]) {
    if ([dev position] == cameraPosition) {
      device = dev;
      break;
    }
  }
  if (!device) {
    return false;
  }

  NSError* error = nil;
  AVCaptureDeviceInput* newCaptureDeviceInput =
      [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
  if (!newCaptureDeviceInput) {
    return false;
  }

  [capture_session_ beginConfiguration];

  [capture_session_ removeInput:capture_device_input_];
  if ([capture_session_ canAddInput:newCaptureDeviceInput]) {
    [capture_session_ addInput:newCaptureDeviceInput];
    capture_device_input_ = newCaptureDeviceInput;
  } else {
    [capture_session_ addInput:capture_device_input_];
  }
  [capture_session_ commitConfiguration];

  UpdateOutputRotation();

  return true;
}

AVCaptureDevicePosition SourceCamera::GetCameraPosition() {
  return [[capture_device_input_ device] position];
}

void SourceCamera::SetOutputImageOrientation(
    UIInterfaceOrientation orientation) {
  output_image_orientation_ = orientation;
  UpdateOutputRotation();
}

void SourceCamera::SetHorizontallyMirrorFrontFacingCamera(bool newValue) {
  horizontally_mirror_front_facing_camera_ = newValue;
  UpdateOutputRotation();
}

void SourceCamera::SetHorizontallyMirrorRearFacingCamera(bool newValue) {
  horizontally_mirror_rear_facing_camera_ = newValue;
  UpdateOutputRotation();
}

void SourceCamera::UpdateOutputRotation() {
  if (GetCameraPosition() == AVCaptureDevicePositionBack) {
    if (horizontally_mirror_rear_facing_camera_) {
      switch (output_image_orientation_) {
        case UIInterfaceOrientationPortrait:
          output_rotation_ = gpupixel::RotateRightFlipVertical;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          output_rotation_ = gpupixel::Rotate180;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          output_rotation_ = gpupixel::FlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeRight:
          output_rotation_ = gpupixel::FlipVertical;
          break;
        default:
          output_rotation_ = gpupixel::NoRotation;
      }
    } else {
      switch (output_image_orientation_) {
        case UIInterfaceOrientationPortrait:
          output_rotation_ = gpupixel::RotateRight;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          output_rotation_ = gpupixel::RotateLeft;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          output_rotation_ = gpupixel::Rotate180;
          break;
        case UIInterfaceOrientationLandscapeRight:
          output_rotation_ = gpupixel::NoRotation;
          break;
        default:
          output_rotation_ = gpupixel::NoRotation;
      }
    }
  } else {
    if (horizontally_mirror_front_facing_camera_) {
      switch (output_image_orientation_) {
        case UIInterfaceOrientationPortrait:
          output_rotation_ = gpupixel::RotateRightFlipVertical;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          output_rotation_ = gpupixel::RotateRightFlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          output_rotation_ = gpupixel::FlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeRight:
          output_rotation_ = gpupixel::FlipVertical;
          break;
        default:
          output_rotation_ = gpupixel::NoRotation;
      }
    } else {
      switch (output_image_orientation_) {
        case UIInterfaceOrientationPortrait:
          output_rotation_ = gpupixel::RotateRight;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          output_rotation_ = gpupixel::RotateLeft;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          output_rotation_ = gpupixel::NoRotation;
          break;
        case UIInterfaceOrientationLandscapeRight:
          output_rotation_ = gpupixel::Rotate180;
          break;
        default:
          output_rotation_ = gpupixel::NoRotation;
      }
    }
  }
  video_data_output_sample_buffer_delegate_.rotation = output_rotation_;
}
#endif
}  // namespace gpupixel

#if defined(GPUPIXEL_IOS)
@implementation VideoDataOutputSampleBufferDelegate
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate
- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection {
  if (_sourceCamera) {
    gpupixel::GPUPixelContext::GetInstance()->RunSync([&] {
      CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
      CVPixelBufferLockBaseAddress(imageBuffer, 0);
      _sourceCamera->SetFrameData((int)CVPixelBufferGetWidth(imageBuffer),
                                  (int)CVPixelBufferGetHeight(imageBuffer),
                                  CVPixelBufferGetBaseAddress(imageBuffer),
                                  _rotation);
      CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
      _sourceCamera->DoRender();
    });
  }
}
@end
#endif
