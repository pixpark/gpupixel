/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_camera.h"
#include "gpupixel_context.h"
#include "util.h"
USING_NS_GPUPIXEL

SourceCamera::SourceCamera() {
#if defined(GPUPIXEL_IOS)
  _videoDataOutputSampleBufferDelegate =
      [[VideoDataOutputSampleBufferDelegate alloc] init];
  _videoDataOutputSampleBufferDelegate.sourceCamera = this;

  _horizontallyMirrorFrontFacingCamera = false;
  _horizontallyMirrorRearFacingCamera = false;
#endif
}

SourceCamera::~SourceCamera() {
#if defined(GPUPIXEL_IOS)
  stop();
  _videoDataOutputSampleBufferDelegate = 0;
#endif
}

std::shared_ptr<SourceCamera> SourceCamera::create() {
  auto sourceCamera = std::shared_ptr<SourceCamera>(new SourceCamera());
#if defined(GPUPIXEL_IOS)
  if (!sourceCamera->init()) {
    sourceCamera.reset();
  }
#endif
  return sourceCamera;
}

void SourceCamera::setFrameData(
    int width,
    int height,
    const void* pixels,
    RotationMode outputRotation /* = RotationMode::NoRotation*/) {
  if (!_framebuffer || (_framebuffer->getWidth() != width ||
                        _framebuffer->getHeight() != height)) {
    _framebuffer =
        GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
            width, height, true);
  }
  if(_face_detector) {
      _face_detector->Detect(static_cast<const uint8_t *>(pixels), width, height,
                             GPUPIXEL_MODE_FMT_VIDEO,
                             GPUPIXEL_FRAME_TYPE_RGBA8888);
  }
  this->setFramebuffer(_framebuffer, outputRotation);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture()));
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
bool SourceCamera::init() {
  if (isCameraExist(AVCaptureDevicePositionFront)) {
    return init(AVCaptureSessionPreset640x480, AVCaptureDevicePositionFront);
  } else {
    return init(AVCaptureSessionPreset640x480, AVCaptureDevicePositionBack);
  }
}

bool SourceCamera::init(NSString* sessionPreset,
                        AVCaptureDevicePosition cameraPosition) {
  _outputRotation = gpupixel::NoRotation;
  // internalRotation = gpupixel::NoRotation;
  _capturePaused = NO;

  _captureSession = [[AVCaptureSession alloc] init];
  _captureSession.sessionPreset = sessionPreset;

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
  _captureDeviceInput =
      [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
  if ([_captureSession canAddInput:_captureDeviceInput]) {
    [_captureSession addInput:_captureDeviceInput];
  } else {
    return false;
  }

  // output
  _captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
  [_captureVideoDataOutput setAlwaysDiscardsLateVideoFrames:YES];
  [_captureSession addOutput:_captureVideoDataOutput];
  dispatch_queue_t queue =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
  [_captureVideoDataOutput
      setSampleBufferDelegate:_videoDataOutputSampleBufferDelegate
                        queue:queue];
  _captureVideoDataOutput.videoSettings = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSNumber
                                       numberWithInt:kCVPixelFormatType_32BGRA],
                                   kCVPixelBufferPixelFormatTypeKey, nil];

  setOutputImageOrientation(UIInterfaceOrientationPortrait);

  return true;
}

bool SourceCamera::isCameraExist(AVCaptureDevicePosition cameraPosition) {
  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  for (AVCaptureDevice* device in devices) {
    if ([device position] == cameraPosition) {
      return true;
    }
  }
  return false;
}

void SourceCamera::start() {
  if (![_captureSession isRunning]) {
    _videoDataOutputSampleBufferDelegate.sourceCamera = this;
    [_captureSession startRunning];
  };
}

void SourceCamera::stop() {
  if ([_captureSession isRunning]) {
    _videoDataOutputSampleBufferDelegate.sourceCamera = 0;
    [_captureSession stopRunning];
  }
}

void SourceCamera::pause() {
  _capturePaused = true;
}

void SourceCamera::resume() {
  _capturePaused = false;
}

bool SourceCamera::isRunning() {
  return [_captureSession isRunning];
}

bool SourceCamera::flip() {
  AVCaptureDevicePosition cameraPosition =
      [[_captureDeviceInput device] position];
  if (cameraPosition == AVCaptureDevicePositionBack) {
    cameraPosition = AVCaptureDevicePositionFront;
  } else {
    cameraPosition = AVCaptureDevicePositionBack;
  }

  if (!isCameraExist(cameraPosition)) {
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

  [_captureSession beginConfiguration];

  [_captureSession removeInput:_captureDeviceInput];
  if ([_captureSession canAddInput:newCaptureDeviceInput]) {
    [_captureSession addInput:newCaptureDeviceInput];
    _captureDeviceInput = newCaptureDeviceInput;
  } else {
    [_captureSession addInput:_captureDeviceInput];
  }
  [_captureSession commitConfiguration];

  _updateOutputRotation();

  return true;
}

AVCaptureDevicePosition SourceCamera::getCameraPosition() {
  return [[_captureDeviceInput device] position];
}

void SourceCamera::setOutputImageOrientation(
    UIInterfaceOrientation orientation) {
  _outputImageOrientation = orientation;
  _updateOutputRotation();
}

void SourceCamera::setHorizontallyMirrorFrontFacingCamera(bool newValue) {
  _horizontallyMirrorFrontFacingCamera = newValue;
  _updateOutputRotation();
}

void SourceCamera::setHorizontallyMirrorRearFacingCamera(bool newValue) {
  _horizontallyMirrorRearFacingCamera = newValue;
  _updateOutputRotation();
}

void SourceCamera::_updateOutputRotation() {
  if (getCameraPosition() == AVCaptureDevicePositionBack) {
    if (_horizontallyMirrorRearFacingCamera) {
      switch (_outputImageOrientation) {
        case UIInterfaceOrientationPortrait:
          _outputRotation = gpupixel::RotateRightFlipVertical;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          _outputRotation = gpupixel::Rotate180;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          _outputRotation = gpupixel::FlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeRight:
          _outputRotation = gpupixel::FlipVertical;
          break;
        default:
          _outputRotation = gpupixel::NoRotation;
      }
    } else {
      switch (_outputImageOrientation) {
        case UIInterfaceOrientationPortrait:
          _outputRotation = gpupixel::RotateRight;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          _outputRotation = gpupixel::RotateLeft;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          _outputRotation = gpupixel::Rotate180;
          break;
        case UIInterfaceOrientationLandscapeRight:
          _outputRotation = gpupixel::NoRotation;
          break;
        default:
          _outputRotation = gpupixel::NoRotation;
      }
    }
  } else {
    if (_horizontallyMirrorFrontFacingCamera) {
      switch (_outputImageOrientation) {
        case UIInterfaceOrientationPortrait:
          _outputRotation = gpupixel::RotateRightFlipVertical;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          _outputRotation = gpupixel::RotateRightFlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          _outputRotation = gpupixel::FlipHorizontal;
          break;
        case UIInterfaceOrientationLandscapeRight:
          _outputRotation = gpupixel::FlipVertical;
          break;
        default:
          _outputRotation = gpupixel::NoRotation;
      }
    } else {
      switch (_outputImageOrientation) {
        case UIInterfaceOrientationPortrait:
          _outputRotation = gpupixel::RotateRight;
          break;
        case UIInterfaceOrientationPortraitUpsideDown:
          _outputRotation = gpupixel::RotateLeft;
          break;
        case UIInterfaceOrientationLandscapeLeft:
          _outputRotation = gpupixel::NoRotation;
          break;
        case UIInterfaceOrientationLandscapeRight:
          _outputRotation = gpupixel::Rotate180;
          break;
        default:
          _outputRotation = gpupixel::NoRotation;
      }
    }
  }
  _videoDataOutputSampleBufferDelegate.rotation = _outputRotation;
}
#endif

#if defined(GPUPIXEL_IOS)
@implementation VideoDataOutputSampleBufferDelegate
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate
- (void)captureOutput:(AVCaptureOutput*)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection*)connection {
  if (_sourceCamera) {
    GPUPixelContext::getInstance()->runSync([&] {
      CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
      CVPixelBufferLockBaseAddress(imageBuffer, 0);
      _sourceCamera->setFrameData((int)CVPixelBufferGetWidth(imageBuffer),
                                  (int)CVPixelBufferGetHeight(imageBuffer),
                                  CVPixelBufferGetBaseAddress(imageBuffer),
                                  _rotation);
      CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
      _sourceCamera->proceed();
    });
  }
}
@end
#endif
