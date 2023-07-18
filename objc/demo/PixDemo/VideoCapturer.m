/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoCapturer.h"
 
@implementation VCVideoCapturerParam

- (instancetype)init {
  self = [super init];
  if (self) {
    _devicePosition = AVCaptureDevicePositionFront;
    _sessionPreset = AVCaptureSessionPreset1280x720;
    _frameRate = 15;
    _videoOrientation = AVCaptureVideoOrientationPortrait;
#if TARGET_OS_IPHONE
    switch ([UIDevice currentDevice].orientation) {
      case UIDeviceOrientationPortrait:
      case UIDeviceOrientationPortraitUpsideDown:
        _videoOrientation = AVCaptureVideoOrientationPortrait;
        break;
      case UIDeviceOrientationLandscapeRight:
        _videoOrientation = AVCaptureVideoOrientationLandscapeRight;
        break;
      case UIDeviceOrientationLandscapeLeft:
        _videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
        break;
      default:
        break;
    }
#elif TARGET_OS_MAC
    _videoOrientation = AVCaptureVideoOrientationPortrait;
#endif
  }
  return self;
}

@end

@interface VideoCapturer () <AVCaptureVideoDataOutputSampleBufferDelegate>

/** 采集会话 */
@property(nonatomic, strong) AVCaptureSession *captureSession;
/** 采集输入设备 也就是摄像头 */
@property(nonatomic, strong) AVCaptureDeviceInput *captureDeviceInput;
/** 采集输出 */
@property(nonatomic, strong) AVCaptureVideoDataOutput *captureVideoDataOutput;
/** 抓图输出 */
@property(nonatomic, strong) AVCaptureStillImageOutput *captureStillImageOutput;
/** 预览图层，把这个图层加在View上就能播放 */
@property(nonatomic, strong) AVCaptureVideoPreviewLayer *videoPreviewLayer;
/** 输出连接 */
@property(nonatomic, strong) AVCaptureConnection *captureConnection;
/** 是否已经在采集 */
@property(nonatomic, assign) BOOL isCapturing;

@end

@implementation VideoCapturer

- (void)dealloc {
}

- (instancetype)initWithCaptureParam:(VCVideoCapturerParam *)param
                               error:(NSError *__autoreleasing _Nullable *_Nullable)error;
{
  if (self = [super init]) {
    NSError *errorMessage = nil;
    self.captureParam = param;

    /****************** 设置输入设备 ************************/
    // 获取所有摄像头
    NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
#if TARGET_OS_IPHONE
    // 获取当前方向的摄像头
    NSArray *captureDeviceArray = [cameras
        filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"position == %d",
                                                                     _captureParam.devicePosition]];
#elif TARGET_OS_MAC
    NSArray *captureDeviceArray = cameras;
#endif
    if (captureDeviceArray.count == 0) {
      errorMessage = [self p_errorWithDomain:@"MAVideoCapture::Get Camera Faild!"];
      return nil;
    }
    // 转化为输入设备
    AVCaptureDevice *camera = captureDeviceArray.firstObject;
    self.captureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:camera
                                                                    error:&errorMessage];
    if (errorMessage) {
      errorMessage = [self p_errorWithDomain:@"MAVideoCapture::AVCaptureDeviceInput init error"];
      return nil;
    }

    /****************** 设置输出设备 ************************/

    // 设置视频输出
    self.captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];

    NSDictionary *videoSetting = [NSDictionary
        dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:self.captureParam.pixelsFormatType],
                                     kCVPixelBufferPixelFormatTypeKey, nil];

    [self.captureVideoDataOutput setVideoSettings:videoSetting];
    // 设置输出串行队列和数据回调
    dispatch_queue_t outputQueue =
        dispatch_get_main_queue();  // dispatch_queue_create("VCVideoCapturerOutputQueue",
                                    // DISPATCH_QUEUE_SERIAL);
    [self.captureVideoDataOutput setSampleBufferDelegate:self queue:outputQueue];
    // 丢弃延迟的帧
    self.captureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;

    // 设置抓图输出
    self.captureStillImageOutput = [[AVCaptureStillImageOutput alloc] init];
    [self.captureStillImageOutput setOutputSettings:@{AVVideoCodecKey : AVVideoCodecJPEG}];

    /****************** 初始化会话 ************************/
    self.captureSession = [[AVCaptureSession alloc] init];
#if TARGET_OS_IPHONE
    self.captureSession.usesApplicationAudioSession = NO;
#endif
    // 添加输入设备到会话
    if ([self.captureSession canAddInput:self.captureDeviceInput]) {
      [self.captureSession addInput:self.captureDeviceInput];
    } else {
      [self p_errorWithDomain:@"MAVideoCapture::Add captureDeviceInput failed!"];
      return nil;
    }
    // 添加输出设备到会话
    if ([self.captureSession canAddOutput:self.captureVideoDataOutput]) {
      [self.captureSession addOutput:self.captureVideoDataOutput];
    } else {
      [self p_errorWithDomain:@"MAVideoCapture::Add captureVideoDataOutput Faild!"];
      return nil;
    }

    if ([self.captureSession canAddOutput:self.captureStillImageOutput]) {
      [self.captureSession addOutput:self.captureStillImageOutput];
    } else {
      [self p_errorWithDomain:@"MAVideoCapture::Add captureStillImageOutput Faild!"];
      return nil;
    }

      self.captureParam.sessionPreset = param.sessionPreset;

    // 设置分辨率
    if ([self.captureSession canSetSessionPreset:self.captureParam.sessionPreset]) {
      self.captureSession.sessionPreset = self.captureParam.sessionPreset;
    }

    /****************** 初始化连接 ************************/
    self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];

    // 设置摄像头镜像，不设置的话前置摄像头采集出来的图像是反转的
    if (self.captureParam.devicePosition == AVCaptureDevicePositionFront &&
        self.captureConnection.supportsVideoMirroring) {
      self.captureConnection.videoMirrored = YES;
    }
    self.captureConnection.videoOrientation = self.captureParam.videoOrientation;

    self.videoPreviewLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.captureSession];
    self.videoPreviewLayer.connection.videoOrientation = self.captureParam.videoOrientation;
    self.videoPreviewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
    if (error) {
      *error = errorMessage;
    }

    // 设置帧率
    [self adjustFrameRate:self.captureParam.frameRate];
  }
  return self;
}

/** 开始采集 */
- (NSError *)startCapture {
  if (self.isCapturing) {
    return [self p_errorWithDomain:@"MAVideoCapture::startCapture failed! is capturing!"];
  }
  // 摄像头权限判断
  AVAuthorizationStatus videoAuthStatus =
      [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
  if (videoAuthStatus != AVAuthorizationStatusAuthorized) {
    [self p_errorWithDomain:@"MAVideoCapture::Camera Authorizate failed!"];
  }
  [self.captureSession startRunning];
  self.isCapturing = YES;
  return nil;
}

/** 停止采集 */
- (NSError *)stopCapture {
  if (!self.isCapturing) {
    return [self p_errorWithDomain:@"MAVideoCapture::stopCapture failed! is not capturing!"];
  }
  [self.captureSession stopRunning];
  self.isCapturing = NO;
  return nil;
}

- (NSError *)adjustFrameRate:(NSInteger)frameRate {
  NSError *error = nil;
  AVFrameRateRange *frameRateRange =
      [self.captureDeviceInput.device.activeFormat.videoSupportedFrameRateRanges objectAtIndex:0];
  if (frameRate > frameRateRange.maxFrameRate || frameRate < frameRateRange.minFrameRate) {
    return [self p_errorWithDomain:@"MAVideoCapture::Set frame rate failed! out of range"];
  }

  [self.captureDeviceInput.device lockForConfiguration:&error];
  self.captureDeviceInput.device.activeVideoMinFrameDuration =
      CMTimeMake(1, (int)self.captureParam.frameRate);
  self.captureDeviceInput.device.activeVideoMaxFrameDuration =
      CMTimeMake(1, (int)self.captureParam.frameRate);
  [self.captureDeviceInput.device unlockForConfiguration];
  return error;
}

#if TARGET_OS_IPHONE
- (void)imageCapture:(void (^)(UIImage *image))completion {
  [self.captureStillImageOutput
      captureStillImageAsynchronouslyFromConnection:self.captureConnection
                                  completionHandler:^(CMSampleBufferRef imageDataSampleBuffer,
                                                      NSError *error) {
                                    if (imageDataSampleBuffer && completion) {
                                      UIImage *image = [UIImage
                                          imageWithData:[AVCaptureStillImageOutput
                                                            jpegStillImageNSDataRepresentation:
                                                                imageDataSampleBuffer]];
                                      completion(image);
                                    }
                                  }];
}
#endif

/** 翻转摄像头 */
- (NSError *)reverseCamera {
  // 获取所有摄像头
  NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
  // 获取当前摄像头方向
  AVCaptureDevicePosition currentPosition = self.captureDeviceInput.device.position;
  AVCaptureDevicePosition toPosition = AVCaptureDevicePositionUnspecified;
  if (currentPosition == AVCaptureDevicePositionBack ||
      currentPosition == AVCaptureDevicePositionUnspecified) {
    toPosition = AVCaptureDevicePositionFront;
  } else {
    toPosition = AVCaptureDevicePositionBack;
  }
  NSArray *captureDeviceArray = [cameras
      filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"position == %d", toPosition]];
  if (captureDeviceArray.count == 0) {
    return [self p_errorWithDomain:@"MAVideoCapture::reverseCamera failed! get new Camera Faild!"];
  }

  NSError *error = nil;
  AVCaptureDevice *camera = captureDeviceArray.firstObject;
  AVCaptureDeviceInput *newInput = [AVCaptureDeviceInput deviceInputWithDevice:camera error:&error];
  // 修改输入设备
  [self.captureSession beginConfiguration];
  [self.captureSession removeInput:self.captureDeviceInput];
  if ([_captureSession canAddInput:newInput]) {
    [_captureSession addInput:newInput];
    self.captureDeviceInput = newInput;
  }
  [self.captureSession commitConfiguration];

  // 重新获取连接并设置方向
  self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
  // 设置摄像头镜像，不设置的话前置摄像头采集出来的图像是反转的
  if (toPosition == AVCaptureDevicePositionFront && self.captureConnection.supportsVideoMirroring) {
    self.captureConnection.videoMirrored = YES;
  }
  self.captureConnection.videoOrientation = self.captureParam.videoOrientation;
  self.captureConnection.videoOrientation = self.captureParam.videoOrientation;

  return nil;
}

/** 采集过程中动态修改视频分辨率 */
- (void)changeSessionPreset:(AVCaptureSessionPreset)sessionPreset {
  self.captureParam.sessionPreset = sessionPreset;
  if ([self.captureSession canSetSessionPreset:self.captureParam.sessionPreset]) {
    self.captureSession.sessionPreset = self.captureParam.sessionPreset;
  }
}

- (NSError *)p_errorWithDomain:(NSString *)domain {
  return [NSError errorWithDomain:domain code:1 userInfo:nil];
}

#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate

/**
 摄像头采集的数据回调

 @param output 输出设备
 @param sampleBuffer 帧缓存数据，描述当前帧信息
 @param connection 连接
 */
- (void)captureOutput:(AVCaptureOutput *)output
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
           fromConnection:(AVCaptureConnection *)connection {
  if ([self.delegate respondsToSelector:@selector(videoCaptureOutputDataCallback:)]) {
    [self.delegate videoCaptureOutputDataCallback:sampleBuffer];
  }
}

- (NSString *)getSessionPresetForDevice:(AVCaptureDevice *)device {
  if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset640x480]) {
    return AVCaptureSessionPreset640x480;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1280x720]) {
    return AVCaptureSessionPreset1280x720;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset352x288]) {
    return AVCaptureSessionPreset352x288;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset3840x2160]) {
    return AVCaptureSessionPreset3840x2160;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPreset1920x1080]) {
    return AVCaptureSessionPreset1920x1080;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPresetHigh]) {
    return AVCaptureSessionPresetHigh;
  } else if ([device supportsAVCaptureSessionPreset:AVCaptureSessionPresetMedium]) {
    return AVCaptureSessionPresetMedium;
  } else {
    return AVCaptureSessionPresetLow;
  }
}

@end
