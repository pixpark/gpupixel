/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <AVFoundation/AVFoundation.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#endif

@protocol VCVideoCapturerDelegate <NSObject>

/**
 * Camera capture data output
 *
 * @param sampleBuffer Captured data
 */
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer;

@end

@interface VCVideoCapturerParam : NSObject

/** Camera position, default is front camera AVCaptureDevicePositionFront */
@property(nonatomic, assign) AVCaptureDevicePosition devicePosition;
/** Video resolution, default is AVCaptureSessionPreset1280x720 */
@property(nonatomic, assign) AVCaptureSessionPreset sessionPreset;
/** Frame rate in frames/second, default is 15fps */
@property(nonatomic, assign) NSInteger frameRate;
/** Camera orientation, default is current device screen orientation */
@property(nonatomic, assign) AVCaptureVideoOrientation videoOrientation;

@property(nonatomic, assign) NSInteger pixelsFormatType;

@end

@interface VideoCapturer : NSObject

/** Delegate */
@property(nonatomic, weak) id<VCVideoCapturerDelegate> delegate;
/** Preview layer, add this layer to View and set its frame to play */
@property(nonatomic, strong, readonly)
    AVCaptureVideoPreviewLayer* videoPreviewLayer;
/** Video capture parameters */
@property(nonatomic, strong) VCVideoCapturerParam* captureParam;
/**
 Initialization method

 @param param Parameters
 @return Instance
 */
- (instancetype)initWithCaptureParam:(VCVideoCapturerParam*)param
                               error:(NSError**)error;

/** Start capture */
- (NSError*)startCapture;

/** Stop capture */
- (NSError*)stopCapture;

/** Capture image, block returns UIImage */
#if TARGET_OS_IPHONE
- (void)imageCapture:(void (^)(UIImage* image))completion;
#endif

/** Dynamically adjust frame rate */
- (NSError*)adjustFrameRate:(NSInteger)frameRate;

/** Switch camera */
- (NSError*)reverseCamera;

/** Dynamically change video resolution during capture */
- (void)changeSessionPreset:(AVCaptureSessionPreset)sessionPreset;

@end
