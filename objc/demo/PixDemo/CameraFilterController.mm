/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "CameraFilterController.h"
#import "VideoCapturer.h"
#import "VideoPreview.h"
#import "PanelMenuView.h"
#import "PanelBeautyParams.h"

@interface CameraFilterController () <VCVideoCapturerDelegate, FilterMenuPanelDelegate> {
    bool captureYuvFrame;
}

@property (strong, nonatomic) VideoPreview * faceBeautyVideoView;
@property (strong, nonatomic) VideoCapturer* capturer;
//
@property (nonatomic, strong) PanelMenuView *menusView;
// Slider
@property (strong, nonatomic) UISlider* faceSmoothSlider;
@property (strong, nonatomic) UISlider* skinWhitenSlider;
@property (strong, nonatomic) UISlider* faceSlimSlider;
@property (strong, nonatomic) UISlider* bigEyeSlider;
// Label
@property (strong, nonatomic) UILabel* faceSmoothLabel;
@property (strong, nonatomic) UILabel* skinWhitenLabel;
@property (strong, nonatomic) UILabel* faceSlimLabel;
@property (strong, nonatomic) UILabel* bigEyeLabel;

// Button
@property (strong, nonatomic) UIButton* effectToggleBtn;
@end

@implementation CameraFilterController

- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // screen always light
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
  
  captureYuvFrame = false;
  self.faceBeautyVideoView = [[VideoPreview alloc] initWithFrame:self.view.bounds];
  [self.view addSubview:self.faceBeautyVideoView];

  [self.view addSubview:self.effectToggleBtn];
  // start camera capture
  [self.capturer startCapture];
  
  [self.menusView showMenuView:YES];
}

- (void)viewWillDisappear:(BOOL)animated {
  [self.capturer stopCapture];
  
  [super viewWillDisappear:animated];
}

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [_menusView showMenuView:YES];
}

#pragma mark - 属性赋值
- (void)setBeautyValue:(CGFloat)beautyValue{
  _beautyValue = beautyValue;
  [self.faceBeautyVideoView setFaceSmoothLevel:beautyValue];
}
- (void)setWhithValue:(CGFloat)whithValue{
  _whithValue = whithValue;
  [self.faceBeautyVideoView setWhitenLevel:whithValue];
}
- (void)setSaturationValue:(CGFloat)saturationValue{
  _saturationValue = saturationValue;
}

- (void)setThinFaceValue:(CGFloat)thinFaceValue{
  _thinFaceValue = thinFaceValue;
  [self.faceBeautyVideoView setFaceSlimLevel:thinFaceValue];
}

- (void)setEyeValue:(CGFloat)eyeValue{
  _eyeValue = eyeValue;
  [self.faceBeautyVideoView setEyeZoomLevel:eyeValue];
}
 
// camera frame callback
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer {
  if(captureYuvFrame) {
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    const uint8_t* dataY = (const uint8_t*)CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 0);//YYYYYYYY
    size_t strideY = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
    const uint8_t* dataUV = (const uint8_t*)CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 1);//UVUVUVUV
    size_t strideUV = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 1);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    [self.faceBeautyVideoView renderNV12:dataY
                             withStrideY:strideY
                              withDataUV:dataUV
                            withStrideUV:strideUV
                               withWidth:width
                              withHeight:height];
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  } else {
#if 0
    if (![FaceDetector shareInstance].isWorking) {
      CMSampleBufferRef detectSampleBufferRef = NULL;
      CMSampleBufferCreateCopy(kCFAllocatorDefault, sampleBuffer, &detectSampleBufferRef);
      
      
      [[FaceDetector shareInstance] getLandmarksFromSampleBuffer:detectSampleBufferRef];
      
      CFRelease(detectSampleBufferRef);
      
    }
    
    NSArray *landmarks = [FaceDetector shareInstance].oneFace.landmarks;
    if(landmarks) {
      [self.beautyFaceVideoView setFacePoint: landmarks];
    }
#endif
    //
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    [self.faceBeautyVideoView renderRGBA:CVPixelBufferGetWidth(imageBuffer)
                              withHeight:CVPixelBufferGetHeight(imageBuffer)
                              withStride:CVPixelBufferGetBytesPerRow(imageBuffer)/4
                              withPixels:(const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer)];
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  }
}
 
-(void)onToggleBtnPress {
  if(self.faceBeautyVideoView) {
    [self.faceBeautyVideoView toggleBeautyFace:false];
  }
}

-(void)onToggleBtnUpInside {
  if(self.faceBeautyVideoView) {
    [self.faceBeautyVideoView toggleBeautyFace:true];
  }
}

-(VideoCapturer*)capturer {
  if(_capturer == nil) {
    VCVideoCapturerParam *param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 30;
    
    param.sessionPreset = AVCaptureSessionPreset1280x720;
    if(captureYuvFrame) {
      param.pixelsFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
    } else {
      param.pixelsFormatType = kCVPixelFormatType_32BGRA;
    }
    
    param.devicePosition = AVCaptureDevicePositionFront;
    param.videoOrientation = AVCaptureVideoOrientationPortrait;
    _capturer = [[VideoCapturer alloc] initWithCaptureParam:param error:nil];
    _capturer.delegate = self;
  }
  
  return _capturer;
}
 
-(UIButton*)effectToggleBtn {
    if(_effectToggleBtn == nil) {
        _effectToggleBtn = [UIButton buttonWithType:UIButtonTypeSystem];
        _effectToggleBtn.frame = CGRectMake(self.view.bounds.size.width - 50,
                                            self.view.bounds.size.height - 318,
                                            26,
                                            24);
        [_effectToggleBtn setBackgroundImage:[UIImage imageNamed:@"ToggleBtnIcon"] forState:UIControlStateNormal];
        [_effectToggleBtn addTarget: self action: @selector(onToggleBtnPress) forControlEvents: UIControlEventTouchDown];
        [_effectToggleBtn addTarget: self action: @selector(onToggleBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
    }
    return _effectToggleBtn;
}

- (PanelMenuView *)menusView {
  if (!_menusView) {
      _menusView = [[PanelMenuView alloc] initWithFrame:CGRectMake(0,
                                                                       window_height - PanelMeiyanMenuHeight - BottomIndicatorHeight,
                                                                       window_width,
                                                                       PanelMeiyanMenuHeight)
                                                  superView:self.view
                                                   delegate:self  viewController:self];

    }
    return _menusView;
}

- (void)cameraAction {
    
}

- (void)recordAction {
  
}

- (void)encodeWithCoder:(nonnull NSCoder *)coder {
  
}

- (void)traitCollectionDidChange:(nullable UITraitCollection *)previousTraitCollection {
  
}

- (void)preferredContentSizeDidChangeForChildContentContainer:(nonnull id<UIContentContainer>)container {
  
}

- (CGSize)sizeForChildContentContainer:(nonnull id<UIContentContainer>)container withParentContainerSize:(CGSize)parentSize {
  return CGSizeZero;
}

- (void)systemLayoutFittingSizeDidChangeForChildContentContainer:(nonnull id<UIContentContainer>)container {
  
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(nonnull id<UIViewControllerTransitionCoordinator>)coordinator {
  
}

- (void)willTransitionToTraitCollection:(nonnull UITraitCollection *)newCollection withTransitionCoordinator:(nonnull id<UIViewControllerTransitionCoordinator>)coordinator {
  
}

- (void)didUpdateFocusInContext:(nonnull UIFocusUpdateContext *)context withAnimationCoordinator:(nonnull UIFocusAnimationCoordinator *)coordinator {
  
}

- (void)setNeedsFocusUpdate {
  
}

- (BOOL)shouldUpdateFocusInContext:(nonnull UIFocusUpdateContext *)context {
  return NO;
}

- (void)updateFocusIfNeeded {
  
}

@end
