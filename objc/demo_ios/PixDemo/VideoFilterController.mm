/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoFilterController.h"
#import "VideoCapturer.h"
#import "PanelMenuView.h"
#import "PanelBeautyParams.h"
#import "FaceDetector.h"
#import <GPUPixel/GPUPixel.h>

using namespace GPUPixel;

@interface VideoFilterController () <VCVideoCapturerDelegate, FilterMenuPanelDelegate> {
  bool captureYuvFrame;
  std::shared_ptr<SourceRawDataInput> gpuPixelRawInput;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<FaceBeautyFilter> beauty_face_filter_;
  std::shared_ptr<TargetRawDataOutput> targetRawOutput_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
  std::shared_ptr<GPUPixel::FaceMakeupFilter> lipstick_filter_;
  std::shared_ptr<GPUPixel::FaceMakeupFilter> blusher_filter_;
}

@property (strong, nonatomic) VideoCapturer* capturer;
//
@property (nonatomic, strong) PanelMenuView *menusView;
@property (strong, nonatomic) UIButton* effectToggleBtn;

@property BOOL needSetupFaceDetector;
@end

@implementation VideoFilterController

- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // screen always light
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
  // init video filter
  [self initVideoFilter];
  
  [self.view addSubview:self.effectToggleBtn];
  [self.menusView showMenuView:YES];
  
  self.needSetupFaceDetector = TRUE;
  captureYuvFrame = false;
  // start camera capture
  [self.capturer startCapture];
}

- (void)viewWillDisappear:(BOOL)animated {
  [self.capturer stopCapture];
  
  [super viewWillDisappear:animated];
}

-(void) initVideoFilter {
  GPUPixel::GPUPixelContext::getInstance()->runSync([&] {
    gpuPixelRawInput = SourceRawDataInput::create();
    gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:gpuPixelView];
    
 
    auto mouth = SourceImage::create("mouth.png");
    lipstick_filter_ = FaceMakeupFilter::create();
    lipstick_filter_->setImageTexture(mouth);
    lipstick_filter_->setTextureBounds(FrameBounds{502.5, 710, 262.5, 167.5});

    auto blusher = SourceImage::create("blusher.png");
    blusher_filter_ = FaceMakeupFilter::create();
    blusher_filter_->setImageTexture(blusher);
    blusher_filter_->setTextureBounds(FrameBounds{395, 520, 489, 209});

    // create filter
    targetRawOutput_ = TargetRawDataOutput::create();
    beauty_face_filter_ = FaceBeautyFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    
    
    
    
    // filter pipline
    gpuPixelRawInput->addTarget(lipstick_filter_)
                    ->addTarget(blusher_filter_)
                    ->addTarget(face_reshape_filter_)
                    ->addTarget(beauty_face_filter_)
                    ->addTarget(gpuPixelView);
  });
}

- (void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  [_menusView showMenuView:YES];
}

#pragma mark - 属性赋值
- (void)setBeautyValue:(CGFloat)beautyValue{
  _beautyValue = beautyValue;
  beauty_face_filter_->setBlurAlpha(beautyValue);  //[0.0, 1.0]
}
- (void)setWhithValue:(CGFloat)whithValue{
  _whithValue = whithValue;
  beauty_face_filter_->setWhite(whithValue /3);  // [0.0, 0.01]
}
- (void)setSaturationValue:(CGFloat)saturationValue{
  _saturationValue = saturationValue;
}

- (void)setThinFaceValue:(CGFloat)thinFaceValue{
  _thinFaceValue = thinFaceValue;
  face_reshape_filter_->setFaceSlimLevel(thinFaceValue/2000);
}

- (void)setEyeValue:(CGFloat)eyeValue{
  _eyeValue = eyeValue;
  face_reshape_filter_->setEyeZoomLevel(eyeValue/700);
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
    
    // todo render nv12
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  } else {
    if ([self isFaceDetectorEnable] && ![FaceDetector shareInstance].isWorking) {
      if(self.needSetupFaceDetector) {
        // setup face++
        [FaceDetector shareInstance].sampleBufferOrientation = FaceDetectorSampleBufferOrientationNoRatation;
        [FaceDetector shareInstance].faceOrientation = 0;
        [FaceDetector shareInstance].sampleType = FaceDetectorSampleTypeCamera;
        [[FaceDetector shareInstance] auth];
        
        self.needSetupFaceDetector = NO;
      }
      
      [[FaceDetector shareInstance] getLandmarksFromSampleBuffer:sampleBuffer];
      
      NSArray *landmarks = [FaceDetector shareInstance].oneFace.landmarks;
      if(landmarks && face_reshape_filter_) {
        std::vector<float> land_marks;
        for (NSValue *value in landmarks) {
            CGPoint point = [value CGPointValue];
          land_marks.push_back(point.x);
          land_marks.push_back(point.y);
        }
        
        face_reshape_filter_->setLandmarks(land_marks);
        face_reshape_filter_->setHasFace(true);
        
        lipstick_filter_->setFaceLandmarks(land_marks);
        lipstick_filter_->setHasFace(true);
        
        blusher_filter_->setFaceLandmarks(land_marks);
        blusher_filter_->setHasFace(true);
      } else {
        face_reshape_filter_->setHasFace(false);
        lipstick_filter_->setHasFace(false);
        blusher_filter_->setHasFace(false);
      }
    }
 
    //
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    auto width = CVPixelBufferGetWidth(imageBuffer);
    auto height = CVPixelBufferGetHeight(imageBuffer);
    auto stride = CVPixelBufferGetBytesPerRow(imageBuffer)/4;
    auto pixels = (const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
    gpuPixelRawInput->uploadBytes(pixels, width, height, stride);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  }
}

-(void)onToggleBtnPress {
 
}

-(bool)isFaceDetectorEnable {
  return true;
//  return self.thinFaceValue != 0 || self.eyeValue != 0;
}

-(void)onToggleBtnUpInside {

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
