/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoFilterController.h"
#import "VideoCapturer.h"
#import <gpupixel/gpupixel.h>

using namespace gpupixel;

@interface VideoFilterController () <VCVideoCapturerDelegate> {
  bool captureYuvFrame;
  std::shared_ptr<SourceRawDataInput> gpuPixelRawInput;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
  std::shared_ptr<TargetRawDataOutput> targetRawOutput_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> lipstick_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> blusher_filter_;
}

//
@property(nonatomic, assign) CGFloat beautyValue;
@property(nonatomic, assign) CGFloat whithValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat thinFaceValue;
@property(nonatomic, assign) CGFloat eyeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;

// UI
@property (strong, nonatomic) VideoCapturer* capturer;
@property (strong, nonatomic) UIButton* cameraSwitchBtn;
@property (strong, nonatomic) UISegmentedControl* segment;
@property (strong, nonatomic) UISegmentedControl* effectSwitch;
@property (strong, nonatomic) UISlider *slider;
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
                                 self.view.frame.size.height - 70,
                                 self.view.frame.size.width - 20,
                                 30);
  self.segment.apportionsSegmentWidthsByContent = YES;
  self.segment.selectedSegmentIndex = 0;
  //添加事件
  [self.segment addTarget:self action:@selector(onFilterSelectChange:) forControlEvents:UIControlEventValueChanged];
  
  //添加到视图
  [self.view addSubview:self.segment];
  [self.view addSubview:self.effectSwitch];
  
  [self.view addSubview:self.cameraSwitchBtn];
  
  // 初始化
  self.slider = [[UISlider alloc] initWithFrame:CGRectMake(10,
                                                                self.view.frame.size.height - 120,
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
-(void)onFilterSelectChange:(UISegmentedControl *)sender{
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
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    gpuPixelRawInput = SourceRawDataInput::create();
    gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:gpuPixelView];
 
  
    lipstick_filter_ = LipstickFilter::create();
    blusher_filter_ = BlusherFilter::create();
 
    gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
      lipstick_filter_->SetFaceLandmarks(landmarks);
      blusher_filter_->SetFaceLandmarks(landmarks);
      face_reshape_filter_->SetFaceLandmarks(landmarks);
    });
  
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
    [gpuPixelView setBackgroundColor:[UIColor grayColor]];
    [gpuPixelView setFillMode:(gpupixel::TargetView::PreserveAspectRatioAndFill)];
  
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

//点击不同分段就会有不同的事件进行相应
-(void)onFilterSwitchChange:(UISegmentedControl *)sender{
  if (sender.selectedSegmentIndex == 0) {
    [self setBeautyValue: self.beautyValue];
    [self setWhithValue:self.whithValue];
    [self setThinFaceValue:self.thinFaceValue];
    [self setEyeValue: self.eyeValue];
    [self setLipstickValue: self.lipstickValue];
    [self setBlusherValue: self.blusherValue];
  } else {
    beauty_face_filter_->setBlurAlpha(0);
    beauty_face_filter_->setWhite(0);
    face_reshape_filter_->setFaceSlimLevel(0);
    face_reshape_filter_->setEyeZoomLevel(0);
    lipstick_filter_->setBlendLevel(0);
    blusher_filter_->setBlendLevel(0);
  }
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

-(UIButton*)cameraSwitchBtn {
  if(_cameraSwitchBtn == nil) {
    _cameraSwitchBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    _cameraSwitchBtn.frame = CGRectMake(self.view.bounds.size.width - 35,
                                        105,
                                        25,
                                        20);
    
    [_cameraSwitchBtn setBackgroundImage:[UIImage imageNamed:@"CameraIcon"] forState:UIControlStateNormal];
    [_cameraSwitchBtn addTarget: self action: @selector(onCameraSwitchBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
  }
  return _cameraSwitchBtn;
}

-(UISegmentedControl*)effectSwitch {
  if(_effectSwitch == nil) {
    NSArray *array = [NSArray arrayWithObjects:@"ON",@"OFF", nil];
    _effectSwitch = [[UISegmentedControl alloc]initWithItems:array];
    _effectSwitch.frame = CGRectMake(self.view.frame.size.width - 90,
                                     self.view.frame.size.height - 160,
                                    80,
                                    30);
    _effectSwitch.apportionsSegmentWidthsByContent = YES;
    _effectSwitch.selectedSegmentIndex = 0;
    
    [_effectSwitch addTarget:self action:@selector(onFilterSwitchChange:) forControlEvents:UIControlEventValueChanged];
 
  }
  return _effectSwitch;
}

 
@end
