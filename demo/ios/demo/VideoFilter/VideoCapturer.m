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

/** Capture session */
@property(nonatomic, strong) AVCaptureSession *captureSession;
/** Capture input device, i.e. camera */
@property(nonatomic, strong) AVCaptureDeviceInput *captureDeviceInput;
/** Capture output */
@property(nonatomic, strong) AVCaptureVideoDataOutput *captureVideoDataOutput;
/** Photo capture output */
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
@property(nonatomic, strong) AVCapturePhotoOutput *capturePhotoOutput;
#else
@property(nonatomic, strong) AVCaptureStillImageOutput *captureStillImageOutput;
#endif
/** Preview layer, add this layer to View to play */
@property(nonatomic, strong) AVCaptureVideoPreviewLayer *videoPreviewLayer;
/** Output connection */
@property(nonatomic, strong) AVCaptureConnection *captureConnection;
/** Is capturing */
@property(nonatomic, assign) BOOL isCapturing;
/** Photo capture completion callback */
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

/** Start capture */
- (NSError *)startCapture
{
    if (self.isCapturing) {
        return [self p_errorWithDomain:@"MAVideoCapture::startCapture failed! is capturing!"];
    }
    // Check camera permission
    AVAuthorizationStatus videoAuthStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (videoAuthStatus != AVAuthorizationStatusAuthorized) {
        return [self p_errorWithDomain:@"MAVideoCapture::Camera Authorizate failed!"];
    }
    
    // Start session in background thread to avoid UI lag
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [self.captureSession startRunning];
        dispatch_async(dispatch_get_main_queue(), ^{
            self.isCapturing = YES;
        });
    });
    
    return nil;
}

/** Stop capture */
- (NSError *)stopCapture
{
    if (!self.isCapturing) {
        return [self p_errorWithDomain:@"MAVideoCapture::stopCapture failed! is not capturing!"];
    }
    
    // Stop session in background thread to avoid UI lag
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        [self.captureSession stopRunning];
        dispatch_async(dispatch_get_main_queue(), ^{
            self.isCapturing = NO;
        });
    });
    
    return nil;
}

/**
 Adjust camera frame rate
 
 @param frameRate Target frame rate in frames per second
 @return Error message if adjustment fails; otherwise nil
 */
- (NSError *)adjustFrameRate:(NSInteger)frameRate
{
    if (!self.captureDeviceInput || !self.captureDeviceInput.device) {
        return [self p_errorWithDomain:@"MAVideoCapture::Set frame rate failed! No valid device"];
    }
    
    NSError *error = nil;
    
    // Get device supported frame rate range
    NSArray<AVFrameRateRange *> *frameRateRanges = self.captureDeviceInput.device.activeFormat.videoSupportedFrameRateRanges;
    if (frameRateRanges.count == 0) {
        return [self p_errorWithDomain:@"MAVideoCapture::Set frame rate failed! No supported frame rates"];
    }
    
    AVFrameRateRange *frameRateRange = [frameRateRanges objectAtIndex:0];
    
    // Check if frame rate is within supported range
    if (frameRate > frameRateRange.maxFrameRate || frameRate < frameRateRange.minFrameRate) {
        NSString *errorMsg = [NSString stringWithFormat:@"MAVideoCapture::Set frame rate failed! %ld is out of range (%.2f-%.2f)",
                              (long)frameRate, frameRateRange.minFrameRate, frameRateRange.maxFrameRate];
        return [self p_errorWithDomain:errorMsg];
    }
    
    // Lock device for configuration
    if ([self.captureDeviceInput.device lockForConfiguration:&error]) {
        // Update parameters
        self.captureParam.frameRate = frameRate;
        
        // Set minimum and maximum frame duration
        CMTime frameDuration = CMTimeMake(1, (int)frameRate);
        self.captureDeviceInput.device.activeVideoMinFrameDuration = frameDuration;
        self.captureDeviceInput.device.activeVideoMaxFrameDuration = frameDuration;
        
        // Unlock device
        [self.captureDeviceInput.device unlockForConfiguration];
    } else {
        NSLog(@"Cannot lock device for frame rate configuration: %@", error.localizedDescription);
        return error;
    }
    
    return nil;
}

#if TARGET_OS_IPHONE
/**
 Capture still image
 
 @param completion Callback after photo is taken, returns the captured UIImage object or nil if capture fails
 */
- (void)imageCapture:(void (^)(UIImage *image))completion
{
    // Check if currently capturing
    if (!self.isCapturing) {
        NSLog(@"Cannot take photo: camera is not capturing");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
    // iOS 10+ uses AVCapturePhotoOutput
    if (!self.capturePhotoOutput) {
        NSLog(@"Cannot take photo: photo output not initialized");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    self.photoCaptureCompletionBlock = completion;
    
    // Create photo settings
    AVCapturePhotoSettings *photoSettings;
    
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_11_0
    photoSettings = [AVCapturePhotoSettings photoSettingsWithFormat:@{AVVideoCodecKey : AVVideoCodecTypeJPEG}];
#else
    // iOS 10.0-10.3
    photoSettings = [AVCapturePhotoSettings photoSettingsWithFormat:@{AVVideoCodecKey : AVVideoCodecJPEG}];
#endif
    
    // Set flash mode (if needed)
    if ([self.captureDeviceInput.device hasFlash]) {
        photoSettings.flashMode = AVCaptureFlashModeAuto;
    }
    
    // Take photo
    [self.capturePhotoOutput capturePhotoWithSettings:photoSettings delegate:self];
#else
    // iOS versions below 10 use AVCaptureStillImageOutput
    if (!self.captureStillImageOutput) {
        NSLog(@"Cannot take photo: photo output not initialized");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    // Get suitable connection for photo
    AVCaptureConnection *connection = [self.captureStillImageOutput connectionWithMediaType:AVMediaTypeVideo];
    if (!connection) {
        NSLog(@"Cannot take photo: cannot find suitable connection");
        if (completion) {
            completion(nil);
        }
        return;
    }
    
    // Take photo
    [self.captureStillImageOutput captureStillImageAsynchronouslyFromConnection:connection
                                                              completionHandler:^(CMSampleBufferRef imageDataSampleBuffer, NSError *error) {
        if (imageDataSampleBuffer && !error) {
            NSData *imageData = [AVCaptureStillImageOutput jpegStillImageNSDataRepresentation:imageDataSampleBuffer];
            UIImage *image = [UIImage imageWithData:imageData];
            if (completion) {
                completion(image);
            }
        } else {
            NSLog(@"Photo processing failed: %@", error ? error.localizedDescription : @"Unknown error");
            if (completion) {
                completion(nil);
            }
        }
    }];
#endif
}
#endif

/**
 Switch camera
 This method switches between front and rear cameras, and updates the video connection's mirror settings accordingly
 */
- (NSError *)reverseCamera
{
    // If currently taking a photo, don't switch
    if (self.photoCaptureCompletionBlock) {
        return [self p_errorWithDomain:@"MAVideoCapture::Switch camera failed! is capturing!"];
    }
    
    AVCaptureDevicePosition currentPosition = self.captureParam.devicePosition;
    AVCaptureDevicePosition newPosition = (currentPosition == AVCaptureDevicePositionFront) ? AVCaptureDevicePositionBack : AVCaptureDevicePositionFront;
    
    // Get new camera device
    AVCaptureDevice *newCamera = [self getCameraDeviceWithPosition:newPosition];
    if (!newCamera) {
        NSLog(@"Cannot get %@ camera", newPosition == AVCaptureDevicePositionFront ? @"front" : @"back");
        return [self p_errorWithDomain:@"MAVideoCapture::Switch camera failed! No valid device"];
    }
    
    NSError *error = nil;
    AVCaptureDeviceInput *newInput = [AVCaptureDeviceInput deviceInputWithDevice:newCamera error:&error];
    if (!newInput || error) {
        NSLog(@"Cannot create camera input: %@", error.localizedDescription);
        return error;
    }
    
    // Begin session configuration
    [self.captureSession beginConfiguration];
    
    // Remove old input
    [self.captureSession removeInput:self.captureDeviceInput];
    
    // Add new input
    if ([self.captureSession canAddInput:newInput]) {
        [self.captureSession addInput:newInput];
        self.captureDeviceInput = newInput;
        self.captureParam.devicePosition = newPosition;
    } else {
        // If can't add new input, restore original input
        [self.captureSession addInput:self.captureDeviceInput];
        NSLog(@"Cannot add new camera input");
    }
    
    // Update connection and set mirroring
    self.captureConnection = [self.captureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    if (self.captureConnection) {
        if (newPosition == AVCaptureDevicePositionFront && self.captureConnection.supportsVideoMirroring) {
            self.captureConnection.videoMirrored = YES;
        } else {
            self.captureConnection.videoMirrored = NO;
        }
        self.captureConnection.videoOrientation = self.captureParam.videoOrientation;
    }
    
    // Commit session configuration
    [self.captureSession commitConfiguration];
    
    // Adjust frame rate for new camera
    [self adjustFrameRate:self.captureParam.frameRate];
    
    return nil;
}


/** Dynamically change video resolution during capture */
- (void)changeSessionPreset:(AVCaptureSessionPreset)sessionPreset
{
    self.captureParam.sessionPreset = sessionPreset;
    if ([self.captureSession canSetSessionPreset:self.captureParam.sessionPreset]) {
        self.captureSession.sessionPreset = self.captureParam.sessionPreset;
    }
}

/**
 Create a standard error object
 
 @param domain Error domain
 @param code Error code, default is 1
 @param userInfo Additional information, can be nil
 @return Created NSError object
 */
- (NSError *)p_errorWithDomain:(NSString *)domain code:(NSInteger)code userInfo:(NSDictionary *)userInfo
{
    return [NSError errorWithDomain:domain code:code userInfo:userInfo];
}

/**
 Create a standard error object (using default error code 1 and nil userInfo)
 
 @param domain Error domain
 @return Created NSError object
 */
- (NSError *)p_errorWithDomain:(NSString *)domain
{
    return [self p_errorWithDomain:domain code:1 userInfo:nil];
}

#pragma mark - AVCaptureVideoDataOutputSampleBufferDelegate

/**
 Camera capture data callback
 
 @param output Output device
 @param sampleBuffer Frame buffer data describing current frame
 @param connection Connection
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
        NSLog(@"Photo processing failed: %@", error.localizedDescription);
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
            NSLog(@"Cannot get photo data");
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(nil);
            }
        }
    } else {
        NSLog(@"Photo object is null");
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
        }
    }
    
    // Clear callback
    self.photoCaptureCompletionBlock = nil;
}

#elif __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
- (void)captureOutput:(AVCapturePhotoOutput *)output didFinishProcessingPhotoSampleBuffer:(CMSampleBufferRef)photoSampleBuffer previewPhotoSampleBuffer:(CMSampleBufferRef)previewPhotoSampleBuffer resolvedSettings:(AVCaptureResolvedPhotoSettings *)resolvedSettings bracketSettings:(AVCaptureBracketedStillImageSettings *)bracketSettings error:(NSError *)error API_AVAILABLE(ios(10.0)) API_DEPRECATED("Use -captureOutput:didFinishProcessingPhoto:error: instead", ios(10.0, 11.0))
{
    if (error) {
        NSLog(@"Photo processing failed: %@", error.localizedDescription);
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
            NSLog(@"Cannot get photo data");
            if (self.photoCaptureCompletionBlock) {
                self.photoCaptureCompletionBlock(nil);
            }
        }
    } else {
        NSLog(@"Photo sample buffer is empty");
        if (self.photoCaptureCompletionBlock) {
            self.photoCaptureCompletionBlock(nil);
        }
    }
    
    // Clear callback
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
