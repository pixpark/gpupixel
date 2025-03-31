/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoCapturer.h"

@implementation VCVideoCapturerParam

- (instancetype)init
{
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

@interface VideoCapturer () <AVCaptureVideoDataOutputSampleBufferDelegate
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
, AVCapturePhotoCaptureDelegate
#endif
>

/** 采集会话 */
@property(nonatomic, strong) AVCaptureSession *captureSession;
/** 采集输入设备 也就是摄像头 */
@property(nonatomic, strong) AVCaptureDeviceInput *captureDeviceInput;
/** 采集输出 */
@property(nonatomic, strong) AVCaptureVideoDataOutput *captureVideoDataOutput;
/** 抓图输出 */
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
@property(nonatomic, strong) AVCapturePhotoOutput *capturePhotoOutput;
#else
@property(nonatomic, strong) AVCaptureStillImageOutput *captureStillImageOutput;
#endif
/** 预览图层，把这个图层加在 View 上就能播放 */
@property(nonatomic, strong) AVCaptureVideoPreviewLayer *videoPreviewLayer;
/** 输出连接 */
@property(nonatomic, strong) AVCaptureConnection *captureConnection;
/** 是否已经在采集 */
@property(nonatomic, assign) BOOL isCapturing;
/** 拍照完成回调 */
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
@property(nonatomic, copy) void (^photoCaptureCompletionBlock)(UIImage *image);
#endif

@end

@implementation VideoCapturer

- (instancetype)initWithCaptureParam:(VCVideoCapturerParam *)param
                               error:(NSError *__autoreleasing _Nullable *_Nullable)error
{
    if (self = [super init]) {
        NSError *errorMessage = nil;
        self.captureParam = param;
        
        AVCaptureDevice *camera = [self getCameraDeviceWithPosition:_captureParam.devicePosition];
        if (!camera) {
            return nil;
        }
        
        self.captureDeviceInput = [AVCaptureDeviceInput deviceInputWithDevice:camera
                                                                        error:&errorMessage];
        if (errorMessage) {
            return nil;
        }
        
        self.captureVideoDataOutput = [[AVCaptureVideoDataOutput alloc] init];
        NSDictionary *videoSetting = @{(id)kCVPixelBufferPixelFormatTypeKey: @(self.captureParam.pixelsFormatType)};
        [self.captureVideoDataOutput setVideoSettings:videoSetting];
        dispatch_queue_t outputQueue = dispatch_get_main_queue();
        [self.captureVideoDataOutput setSampleBufferDelegate:self queue:outputQueue];
        self.captureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;
        
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
        self.capturePhotoOutput = [[AVCapturePhotoOutput alloc] init];
#else
        self.captureStillImageOutput = [[AVCaptureStillImageOutput alloc] init];
        [self.captureStillImageOutput setOutputSettings:@{AVVideoCodecKey : AVVideoCodecJPEG}];
#endif
        
        self.captureSession = [[AVCaptureSession alloc] init];
#if TARGET_OS_IPHONE
        self.captureSession.usesApplicationAudioSession = NO;
#endif
        if ([self.captureSession canAddInput:self.captureDeviceInput]) {
            [self.captureSession addInput:self.captureDeviceInput];
        } else {
            return nil;
        }
        if ([self.captureSession canAddOutput:self.captureVideoDataOutput]) {
            [self.captureSession addOutput:self.captureVideoDataOutput];
        } else {
            return nil;
        }
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
        if ([self.captureSession canAddOutput:self.capturePhotoOutput]) {
            [self.captureSession addOutput:self.capturePhotoOutput];
        }
#else
        if ([self.captureSession canAddOutput:self.captureStillImageOutput]) {
            [self.captureSession addOutput:self.captureStillImageOutput];
        }
#endif
        self.captureParam.sessionPreset = param.sessionPreset;
        if ([self.captureSession canSetSessionPreset:self.captureParam.sessionPreset]) {
            self.captureSession.sessionPreset = self.captureParam.sessionPreset;
        }
        
        self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
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
        [self adjustFrameRate:self.captureParam.frameRate];
    }
    return self;
}

- (AVCaptureDevice *)getCameraDeviceWithPosition:(AVCaptureDevicePosition)position
{
    AVCaptureDevice *device = nil;
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    AVCaptureDeviceDiscoverySession *discoverySession =
    [AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:@[AVCaptureDeviceTypeBuiltInWideAngleCamera]
                                                           mediaType:AVMediaTypeVideo
                                                            position:position];
    device = discoverySession.devices.firstObject;
#else
    for (AVCaptureDevice *dev in [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo]) {
        if (dev.position == position) {
            device = dev;
            break;
        }
    }
#endif
    return device;
}

/** 开始采集 */
- (NSError *)startCapture
{
    if (self.isCapturing) {
        return [self p_errorWithDomain:@"MAVideoCapture::startCapture failed! is capturing!"];
    }
    // 摄像头权限判断
    AVAuthorizationStatus videoAuthStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (videoAuthStatus != AVAuthorizationStatusAuthorized) {
        return [self p_errorWithDomain:@"MAVideoCapture::Camera Authorizate failed!"];
    }
    
    // 在后台线程启动会话，避免UI卡顿
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [self.captureSession startRunning];
        dispatch_async(dispatch_get_main_queue(), ^{
            self.isCapturing = YES;
        });
    });
    
    return nil;
}

/** 停止采集 */
- (NSError *)stopCapture
{
    if (!self.isCapturing) {
        return [self p_errorWithDomain:@"MAVideoCapture::stopCapture failed! is not capturing!"];
    }
    
    // 在后台线程停止会话，避免UI卡顿
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [self.captureSession stopRunning];
        dispatch_async(dispatch_get_main_queue(), ^{
            self.isCapturing = NO;
        });
    });
    
    return nil;
}

/**
 调整摄像头帧率
 
 @param frameRate 目标帧率，单位为帧/秒
 @return 如果调整失败，返回错误信息；否则返回nil
 */
- (NSError *)adjustFrameRate:(NSInteger)frameRate
{
    if (!self.captureDeviceInput || !self.captureDeviceInput.device) {
        return [self p_errorWithDomain:@"MAVideoCapture::Set frame rate failed! No valid device"];
    }
    
    NSError *error = nil;
    
    // 获取设备支持的帧率范围
    NSArray<AVFrameRateRange *> *frameRateRanges = self.captureDeviceInput.device.activeFormat.videoSupportedFrameRateRanges;
    if (frameRateRanges.count == 0) {
        return [self p_errorWithDomain:@"MAVideoCapture::Set frame rate failed! No supported frame rates"];
    }
    
    AVFrameRateRange *frameRateRange = [frameRateRanges objectAtIndex:0];
    
    // 检查帧率是否在支持范围内
    if (frameRate > frameRateRange.maxFrameRate || frameRate < frameRateRange.minFrameRate) {
        NSString *errorMsg = [NSString stringWithFormat:@"MAVideoCapture::Set frame rate failed! %ld is out of range (%.2f-%.2f)",
                              (long)frameRate, frameRateRange.minFrameRate, frameRateRange.maxFrameRate];
        return [self p_errorWithDomain:errorMsg];
    }
    
    // 锁定设备进行配置
    if ([self.captureDeviceInput.device lockForConfiguration:&error]) {
        // 更新参数
        self.captureParam.frameRate = frameRate;
        
        // 设置最小和最大帧持续时间
        CMTime frameDuration = CMTimeMake(1, (int)frameRate);
        self.captureDeviceInput.device.activeVideoMinFrameDuration = frameDuration;
        self.captureDeviceInput.device.activeVideoMaxFrameDuration = frameDuration;
        
        // 解锁设备
        [self.captureDeviceInput.device unlockForConfiguration];
    } else {
        NSLog(@"无法锁定设备进行帧率配置: %@", error.localizedDescription);
        return error;
    }
    
    return nil;
}

#if TARGET_OS_IPHONE
/**
 拍摄静态图片
 
 @param completion 拍照完成后的回调，返回拍摄的UIImage对象，如果拍摄失败则返回nil
 */
- (void)imageCapture:(void (^)(UIImage *image))completion
{
    // 检查是否正在采集
    if (!self.isCapturing) {
        NSLog(@"无法拍照：摄像头未开始采集");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    // iOS 10及以上版本使用AVCapturePhotoOutput
    if (!self.capturePhotoOutput) {
        NSLog(@"无法拍照：照片输出未初始化");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    self.photoCaptureCompletionBlock = completion;
    
    // 创建照片设置
    AVCapturePhotoSettings *photoSettings;
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_11_0
    photoSettings = [AVCapturePhotoSettings photoSettingsWithFormat:@{AVVideoCodecKey : AVVideoCodecTypeJPEG}];
#else
    // iOS 10.0-10.3
    photoSettings = [AVCapturePhotoSettings photoSettingsWithFormat:@{AVVideoCodecKey : AVVideoCodecJPEG}];
#endif
    
    // 设置闪光灯模式（如果需要）
    if ([self.captureDeviceInput.device hasFlash]) {
        photoSettings.flashMode = AVCaptureFlashModeAuto;
    }
    
    // 拍照
    [self.capturePhotoOutput capturePhotoWithSettings:photoSettings delegate:self];
#else
    // iOS 10以下版本使用AVCaptureStillImageOutput
    if (!self.captureStillImageOutput) {
        NSLog(@"无法拍照：照片输出未初始化");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    // 获取适合拍照的连接
    AVCaptureConnection *connection = [self.captureStillImageOutput connectionWithMediaType:AVMediaTypeVideo];
    if (!connection) {
        NSLog(@"无法拍照：找不到合适的连接");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    // 拍照
    [self.captureStillImageOutput captureStillImageAsynchronouslyFromConnection:connection
                                                              completionHandler:^(CMSampleBufferRef imageDataSampleBuffer, NSError *error) {
        if (imageDataSampleBuffer && !error) {
            NSData *imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:imageDataSampleBuffer];
            UIImage *image = [UIImage imageWithData:imageData];
            if (completion) {
                completion(image);
            }
        } else {
            NSLog(@"拍照失败: %@", error ? error.localizedDescription : @"未知错误");
            if (completion) {
                completion(nil);
            }
        }
    }];
#endif
}
#endif

/**
 翻转摄像头
 该方法会切换前后摄像头，并相应地更新视频连接的镜像设置
 */
- (NSError *)reverseCamera
{
    // 如果当前正在拍照，不执行切换操作
    if (self.photoCaptureCompletionBlock) {
        return [self p_errorWithDomain:@"MAVideoCapture::Switch camera failed! is capturing!"];
    }
    
    AVCaptureDevicePosition currentPosition = self.captureParam.devicePosition;
    AVCaptureDevicePosition newPosition = (currentPosition == AVCaptureDevicePositionFront) ? AVCaptureDevicePositionBack : AVCaptureDevicePositionFront;
    
    // 获取新的摄像头设备
    AVCaptureDevice *newCamera = [self getCameraDeviceWithPosition:newPosition];
    if (!newCamera) {
        NSLog(@"无法获取%@摄像头", newPosition == AVCaptureDevicePositionFront ? @"前置" : @"后置");
        return [self p_errorWithDomain:@"MAVideoCapture::Switch camera failed! No valid device"];
    }
    
    NSError *error = nil;
    AVCaptureDeviceInput *newInput = [AVCaptureDeviceInput deviceInputWithDevice:newCamera error:&error];
    if (!newInput || error) {
        NSLog(@"创建摄像头输入失败: %@", error.localizedDescription);
        return error;
    }
    
    // 开始会话配置
    [self.captureSession beginConfiguration];
    
    // 移除旧的输入
    [self.captureSession removeInput:self.captureDeviceInput];
    
    // 添加新的输入
    if ([self.captureSession canAddInput:newInput]) {
        [self.captureSession addInput:newInput];
        self.captureDeviceInput = newInput;
        self.captureParam.devicePosition = newPosition;
    } else {
        // 如果无法添加新输入，恢复原来的输入
        [self.captureSession addInput:self.captureDeviceInput];
        NSLog(@"无法添加新的摄像头输入");
    }
    
    // 更新连接并设置镜像
    self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    if (self.captureConnection) {
        if (newPosition == AVCaptureDevicePositionFront && self.captureConnection.supportsVideoMirroring) {
            self.captureConnection.videoMirrored = YES;
        } else {
            self.captureConnection.videoMirrored = NO;
        }
        self.captureConnection.videoOrientation = self.captureParam.videoOrientation;
    }
    
    // 提交会话配置
    [self.captureSession commitConfiguration];
    
    // 调整帧率以适应新的摄像头
    [self adjustFrameRate:self.captureParam.frameRate];
    
    return nil;
}


/** 采集过程中动态修改视频分辨率 */
- (void)changeSessionPreset:(AVCaptureSessionPreset)sessionPreset
{
    self.captureParam.sessionPreset = sessionPreset;
    if ([self.captureSession canSetSessionPreset:self.captureParam.sessionPreset]) {
        self.captureSession.sessionPreset = self.captureParam.sessionPreset;
    }
}

/**
 创建一个标准格式的错误对象
 
 @param domain 错误域
 @param code 错误码，默认为1
 @param userInfo 附加信息，可以为nil
 @return 创建的NSError对象
 */
- (NSError *)p_errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo
{
    return [NSError errorWithDomain:domain code:code userInfo:userInfo];
}

/**
 创建一个标准格式的错误对象（使用默认错误码1和nil的userInfo）
 
 @param domain 错误域
 @return 创建的NSError对象
 */
- (NSError *)p_errorWithDomain:(NSString *)domain
{
    return [self p_errorWithDomain:domain code:1 userInfo:nil];
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
       fromConnection:(AVCaptureConnection *)connection
{
    if ([self.delegate respondsToSelector:@selector(videoCaptureOutputDataCallback:)]) {
        [self.delegate videoCaptureOutputDataCallback:sampleBuffer];
    }
}

#pragma mark - AVCapturePhotoCaptureDelegate
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_11_0
- (void)captureOutput:(AVCapturePhotoOutput *)output didFinishProcessingPhoto:(AVCapturePhoto *)photo error:(NSError *)error API_AVAILABLE(ios(11.0))
{
    if (error) {
        NSLog(@"照片处理失败: %@", error.localizedDescription);
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
            self.photoCaptureCompletionBlock = nil;
        }
        return;
    }
    
    if (photo) {
        NSData *photoData = [photo fileDataRepresentation];
        if (photoData) {
            UIImage *image = [UIImage imageWithData:photoData];
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(image);
            }
        } else {
            NSLog(@"无法获取照片数据");
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(nil);
            }
        }
    } else {
        NSLog(@"照片对象为空");
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
        }
    }
    
    // 清理回调
    self.photoCaptureCompletionBlock = nil;
}

#elif __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
- (void)captureOutput:(AVCapturePhotoOutput *)output didFinishProcessingPhotoSampleBuffer:(CMSampleBufferRef)photoSampleBuffer previewPhotoSampleBuffer:(CMSampleBufferRef)previewPhotoSampleBuffer resolvedSettings:(AVCaptureResolvedPhotoSettings *)resolvedSettings bracketSettings:(AVCaptureBracketedStillImageSettings *)bracketSettings error:(NSError *)error API_AVAILABLE(ios(10.0)) API_DEPRECATED("Use -captureOutput:didFinishProcessingPhoto:error: instead", ios(10.0, 11.0))
{
    if (error) {
        NSLog(@"照片处理失败: %@", error.localizedDescription);
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
            self.photoCaptureCompletionBlock = nil;
        }
        return;
    }
    
    if (photoSampleBuffer) {
        NSData *photoData = [AVCapturePhotoOutput JPEGPhotoDataRepresentationForJPEGSampleBuffer:photoSampleBuffer previewPhotoSampleBuffer:previewPhotoSampleBuffer];
        if (photoData) {
            UIImage *image = [UIImage imageWithData:photoData];
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(image);
            }
        } else {
            NSLog(@"无法获取照片数据");
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(nil);
            }
        }
    } else {
        NSLog(@"照片样本缓冲区为空");
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
        }
    }
    
    // 清理回调
    self.photoCaptureCompletionBlock = nil;
}

#endif

- (NSString *)getSessionPresetForDevice:(AVCaptureDevice *)device
{
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
