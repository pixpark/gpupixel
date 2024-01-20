//
//  ViewController.m
//  VideoFilter
//
//  Created by admin on 2022/7/19.
//

#import "ViewController.h"
#import <zp-sdk-matrix/ZPVideoCapturer.h>
#import <zp-sdk-matrix/NebulaMatrixManager.h>
#import <AVFoundation/AVFoundation.h>
#import "NebulaMetalView.h"

@interface ViewController()<VCVideoCapturerDelegate, NebulaMatrixManagerDelegate>{
}

@property (weak) IBOutlet NSView *beautyfaceView;
@property(strong, nonatomic) AVSampleBufferDisplayLayer* sampleBufferDisplayLayer;
@property(strong, nonatomic) ZPVideoCapturer* capturer;
@property(nonatomic, strong) NebulaMatrixManager *matrixManager;
@property(nonatomic, strong) NebulaMetalView *metalView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.sampleBufferDisplayLayer = [[AVSampleBufferDisplayLayer alloc] init];
    self.sampleBufferDisplayLayer.frame = CGRectMake(0, 0, 380, 300);
    self.sampleBufferDisplayLayer.videoGravity = AVLayerVideoGravityResizeAspect;
    self.sampleBufferDisplayLayer.opaque = YES;
    self.beautyfaceView.wantsLayer = YES;
    [self.beautyfaceView setLayer:self.sampleBufferDisplayLayer];
    NSLog(@"layers = %@", self.view.layer.sublayers);
    
    self.metalView = [[NebulaMetalView alloc] initWithFrame:CGRectMake(0, 0, 380, 380)];
    self.metalView.mirror = YES;
    [self.view addSubview:self.metalView];
    
    self.matrixManager = [[NebulaMatrixManager alloc] init];
    [self.matrixManager setBeautyType:NebulaBeautyFaceTypeGPU];
    [self.matrixManager setDelegate:self];
}

- (void)viewWillAppear{
    [super viewWillAppear];
}

- (IBAction)startClick:(id)sender {
    [self.capturer startCapture];
}

- (IBAction)backgroundOps:(id)sender {
    NSLog(@"background operations");
    [self.matrixManager setBackgroundOps:NebulaBackgroundBlur backgroundImagePathStr:@""];
}

- (IBAction)smoothChange:(NSSlider *)sender {
    NSLog(@"change smooth value = %f", sender.floatValue);
    [self.matrixManager setBeautyLevel:sender.floatValue];
}

- (IBAction)whiteChange:(NSSlider *)sender {
    [self.matrixManager setWhitenessLevel:sender.floatValue];
}

- (IBAction)ruddyChange:(NSSlider *)sender {
    [self.matrixManager setRuddyLevel:sender.floatValue];
}

- (ZPVideoCapturer*)capturer {
  if (_capturer == nil) {
    VCVideoCapturerParam* param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 15;

    param.sessionPreset = AVCaptureSessionPreset1280x720;
//    if (captureAsYUV) {
//      param.pixelsFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
//    } else {
      param.pixelsFormatType = kCVPixelFormatType_32BGRA;
//    }

    param.devicePosition = AVCaptureDevicePositionFront;
    param.videoOrientation = AVCaptureVideoOrientationPortrait;
    _capturer = [[ZPVideoCapturer alloc] initWithCaptureParam:param error:nil];
    _capturer.delegate = self;
  }

  return _capturer;
}

- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer{
    CVPixelBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);

    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    int bufferWidth = (int)CVPixelBufferGetWidth(imageBuffer);
    int bufferHeight = (int)CVPixelBufferGetHeight(imageBuffer);
    int planey = (int)CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);

    uint8_t* imgBufAddr = (uint8_t*)CVPixelBufferGetBaseAddress(imageBuffer);
    [self.matrixManager processRGBAFrame:imgBufAddr width:planey/4 height:bufferHeight stride:bufferWidth ts:0];
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    CFRetain(sampleBuffer);
//    [self.sampleBufferDisplayLayer enqueueSampleBuffer:sampleBuffer];
    CFRelease(sampleBuffer);

}

- (void)onOutputI420Data:(const uint8_t *)bytes width:(int)width height:(int)height ts:(int64_t)ts{
    CVPixelBufferRef pixelBuf = [self coverToSampleBuffer:bytes width:width height:height ts:ts];
    [self.metalView renderPixelBuffer:pixelBuf];
    CVPixelBufferRelease(pixelBuf);
}

- (void)onOutputI420Data:(const uint8_t *)bytes
                   width:(int)width
                  height:(int)height
                      ts:(int64_t)ts
              objcRegion:(ObjcRegion)objcRegion{
    CVPixelBufferRef pixelBuf = [self coverToSampleBuffer:bytes width:width height:height ts:ts];
//    [self.sampleBufferDisplayLayer enqueueSampleBuffer:sampleBuffer];
    [self.metalView renderPixelBuffer:pixelBuf];
    CVPixelBufferRelease(pixelBuf);
}

- (CVPixelBufferRef)coverToSampleBuffer:(const uint8_t *)yuvData width:(int)width height:(int)height ts:(int64_t)ts{
    const uint8_t *dataY = yuvData;
    const uint8_t *dataU = yuvData + width * height;
    const uint8_t *dataV = dataU + width * height / 4;
    CVPixelBufferRef pixelBuf = NULL;
    NSDictionary *textureAttributes = @{(__bridge NSString*)kCVPixelBufferMetalCompatibilityKey : @YES};
    CVPixelBufferCreate(kCFAllocatorDefault, width, height,
                        kCVPixelFormatType_420YpCbCr8PlanarFullRange,  (__bridge CFDictionaryRef)textureAttributes,
                        &pixelBuf);
    CVPixelBufferLockBaseAddress(pixelBuf, 0);
    uint8_t *dstY = reinterpret_cast<uint8_t *>(
        CVPixelBufferGetBaseAddressOfPlane(pixelBuf, 0));
    uint8_t *dstU = reinterpret_cast<uint8_t *>(
        CVPixelBufferGetBaseAddressOfPlane(pixelBuf, 1));
    uint8_t *dstV = reinterpret_cast<uint8_t *>(
        CVPixelBufferGetBaseAddressOfPlane(pixelBuf, 2));
    memcpy(dstY, dataY, width * height);
    memcpy(dstU, dataU, width * height / 4);
    memcpy(dstV, dataV, width * height / 4);
    CVPixelBufferUnlockBaseAddress(pixelBuf, 0);
    return pixelBuf;
    
//    CMSampleBufferRef newSampleBuffer = NULL;
//    CMSampleTimingInfo timimgInfo = kCMTimingInfoInvalid;
//    CMVideoFormatDescriptionRef videoInfo = NULL;
//    CMVideoFormatDescriptionCreateForImageBuffer(nil, pixelBuf, &videoInfo);
//    CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault, pixelBuf, true, nil, nil, videoInfo, &timimgInfo, &newSampleBuffer);
//    return newSampleBuffer;
}

@end
