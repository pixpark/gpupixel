/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoFilterController.h"
#import "VideoCapturer.h"
#import "FaceDetector.h"
#import <GPUPixel/GPUPixel.h>

using namespace GPUPixel;

@interface VideoFilterController () <VCVideoCapturerDelegate> {
  bool captureYuvFrame;
  std::shared_ptr<SourceRawDataInput> gpuPixelRawInput;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
  std::shared_ptr<TargetRawDataOutput> targetRawOutput_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
  std::shared_ptr<GPUPixel::FaceMakeupFilter> lipstick_filter_;
  std::shared_ptr<GPUPixel::FaceMakeupFilter> blusher_filter_;
}

@property (strong, nonatomic) VideoCapturer* capturer;
//
@property (strong, nonatomic) UIButton* effectToggleBtn;
@property (strong, nonatomic) UIButton* cameraSwitchBtn;
@property (strong, nonatomic) UISegmentedControl* segment;
@property (strong, nonatomic) UISlider *slider;

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
  [self initUI];

  self.needSetupFaceDetector = TRUE;
  captureYuvFrame = false;
  // start camera capture
  [self.capturer startCapture];
}

-(void)initUI {
  NSArray *array = [NSArray arrayWithObjects:@"磨皮",@"美白",@"瘦脸",@"大眼", @"口红", @"腮红", nil];
  //初始化UISegmentedControl
  self.segment = [[UISegmentedControl alloc]initWithItems:array];
  //设置frame
  self.segment.frame = CGRectMake(10,
                                 self.view.frame.size.height - 120,
                                 self.view.frame.size.width - 20,
                                 30);
  self.segment.apportionsSegmentWidthsByContent = YES;
  self.segment.selectedSegmentIndex = 0;
  //添加事件
  [self.segment addTarget:self action:@selector(change:) forControlEvents:UIControlEventValueChanged];
  
  //添加到视图
  [self.view addSubview:self.segment];
  
  [self.view addSubview:self.effectToggleBtn];
  [self.view addSubview:self.cameraSwitchBtn];
  
  // 初始化
  self.slider = [[UISlider alloc] initWithFrame:CGRectMake(10,
                                                                self.view.frame.size.height - 170,
                                                                self.view.frame.size.width - 20,
                                                                30)];

  // 设置最小值
  self.slider.minimumValue = 0;

  self.slider.maximumValue = 10;
  self.slider.value = 0;
  [self.slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];

  [self.view addSubview:self.slider];
}

-(void)sliderValueChanged:(UISlider*) slider {
  if (self.segment.selectedSegmentIndex == 0) {         // 磨皮
    [self setBeautyValue: slider.value];
  } else if (self.segment.selectedSegmentIndex == 1) {  // 美白
    [self setWhithValue: slider.value];
  } else if (self.segment.selectedSegmentIndex == 2) {  // 瘦脸
    [self setThinFaceValue: slider.value];
  } else if (self.segment.selectedSegmentIndex == 3) {  // 大眼
    [self setEyeValue: slider.value];
  } else if (self.segment.selectedSegmentIndex == 4) {  // 口红
    [self setLipstickValue: slider.value];
  } else if (self.segment.selectedSegmentIndex == 5) {  // 腮红
    [self setBlusherValue: slider.value];
  }
}

//点击不同分段就会有不同的事件进行相应
-(void)change:(UISegmentedControl *)sender{
  if (self.segment.selectedSegmentIndex == 0) {         // 磨皮
    self.slider.value = _beautyValue;
  } else if (self.segment.selectedSegmentIndex == 1) {  // 美白
    self.slider.value = _whithValue;
  } else if (self.segment.selectedSegmentIndex == 2) {  // 瘦脸
    self.slider.value = _thinFaceValue;
  } else if (self.segment.selectedSegmentIndex == 3) {  // 大眼
    self.slider.value = _eyeValue;
  } else if (self.segment.selectedSegmentIndex == 4) {  // 口红
    self.slider.value = _lipstickValue;
  } else if (self.segment.selectedSegmentIndex == 5) {  // 腮红
    self.slider.value = _blusherValue;
  }
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
    beauty_face_filter_ = BeautyFaceFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    
    
    
    
    // filter pipline
    gpuPixelRawInput->addTarget(lipstick_filter_)
                    ->addTarget(blusher_filter_)
                    ->addTarget(face_reshape_filter_)
                    ->addTarget(beauty_face_filter_)
                    ->addTarget(gpuPixelView);
  });
}
 
#pragma mark - 属性赋值
- (void)setBeautyValue:(CGFloat)value {
  _beautyValue = value;
  beauty_face_filter_->setBlurAlpha(value/10);
}
- (void)setWhithValue:(CGFloat)value{
  _whithValue = value;
  beauty_face_filter_->setWhite(value/20);
}
- (void)setSaturationValue:(CGFloat)value{
  _saturationValue = value;
}

- (void)setThinFaceValue:(CGFloat)value{
  _thinFaceValue = value;
  face_reshape_filter_->setFaceSlimLevel(value/100);
}

- (void)setEyeValue:(CGFloat)value{
  _eyeValue = value;
  face_reshape_filter_->setEyeZoomLevel(value/50);
}

- (void)setLipstickValue:(CGFloat)value{
  _lipstickValue = value;
  lipstick_filter_->setBlendLevel(value/10);
}

- (void)setBlusherValue:(CGFloat)value{
  _blusherValue = value;
  blusher_filter_->setBlendLevel(value/10);
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


-(void)onCameraSwitchBtnUpInside {
  [self.capturer reverseCamera];
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
    _effectToggleBtn.frame = CGRectMake(self.view.bounds.size.width - 35,
                                        self.view.bounds.size.height - 200,
                                        25,
                                        25);
    [_effectToggleBtn setBackgroundImage:[UIImage imageNamed:@"ToggleBtnIcon"] forState:UIControlStateNormal];
    [_effectToggleBtn addTarget: self action: @selector(onToggleBtnPress) forControlEvents: UIControlEventTouchDown];
    [_effectToggleBtn addTarget: self action: @selector(onToggleBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
  }
  return _effectToggleBtn;
}

-(UIButton*)cameraSwitchBtn {
  if(_cameraSwitchBtn == nil) {
    _cameraSwitchBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    _cameraSwitchBtn.frame = CGRectMake(self.view.bounds.size.width - 35,
                                        100,
                                        25,
                                        20);
    
    [_cameraSwitchBtn setBackgroundImage:[UIImage imageNamed:@"CameraIcon"] forState:UIControlStateNormal];
    [_cameraSwitchBtn addTarget: self action: @selector(onCameraSwitchBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
  }
  return _cameraSwitchBtn;
}


 
@end
