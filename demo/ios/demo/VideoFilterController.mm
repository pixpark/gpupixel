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
  bool _isCaptureYuvFrame;
  std::shared_ptr<SourceRawData> _sourceRawData;
  std::shared_ptr<SinkRawData> _sinkRawData;
  GPUPixelView *_gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> _beautyFaceFilter;
  std::shared_ptr<FaceReshapeFilter> _faceReshapeFilter;
  std::shared_ptr<gpupixel::LipstickFilter> _lipstickFilter;
  std::shared_ptr<gpupixel::BlusherFilter> _blusherFilter;
  
  RawOutputCallback _rawOutputCallback;
}

//
@property(nonatomic, assign) CGFloat sharpenValue;
@property(nonatomic, assign) CGFloat blurValue;
@property(nonatomic, assign) CGFloat whitenValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat faceSlimValue;
@property(nonatomic, assign) CGFloat eyeEnlargeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;

// UI
@property (strong, nonatomic) VideoCapturer* videoCapturer;
@property (strong, nonatomic) UIButton* cameraSwitchButton;
@property (strong, nonatomic) UISegmentedControl* filterSegment;
@property (strong, nonatomic) UISegmentedControl* effectSwitch;
@property (strong, nonatomic) UISlider *intensitySlider;

@property (assign, nonatomic) BOOL isSavingImage;

@end

@implementation VideoFilterController
- (void)viewWillDisappear:(BOOL)animated {
  [self.videoCapturer stopCapture];
  [super viewWillDisappear:animated];
}
- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // screen always light
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
  
  // init video filter
  [self initVideoFilter];
  [self initUI];
  
  _isCaptureYuvFrame = false;
  // start camera capture
  [self.videoCapturer startCapture];
}
- (void) initVideoFilter {
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    _sourceRawData = SourceRawData::create();
    _gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:_gpuPixelView];
    
    _lipstickFilter = LipstickFilter::create();
    _blusherFilter = BlusherFilter::create();
    _sourceRawData->RegLandmarkCallback([=](std::vector<float> landmarks) {
      _lipstickFilter->SetFaceLandmarks(landmarks);
      _blusherFilter->SetFaceLandmarks(landmarks);
      _faceReshapeFilter->SetFaceLandmarks(landmarks);
    });
    
    // create filter
    _sinkRawData = SinkRawData::create();
    _beautyFaceFilter = BeautyFaceFilter::create();
    _faceReshapeFilter = FaceReshapeFilter::create();
    // filter pipline
    _sourceRawData->addSink(_lipstickFilter)
    ->addSink(_blusherFilter)
    ->addSink(_faceReshapeFilter)
    ->addSink(_beautyFaceFilter)
    ->addSink(_gpuPixelView);
    
    [_gpuPixelView setBackgroundColor:[UIColor grayColor]];
    [_gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];
    
    _beautyFaceFilter->addSink(_sinkRawData);
    __weak typeof(VideoFilterController*)weakSelf = self;
    _rawOutputCallback = [weakSelf]( const uint8_t* data, int width, int height, int64_t ts) {
      if (weakSelf.isSavingImage == YES) {
        unsigned char *imageData = (unsigned char *)data;
        size_t bitsPerComponent = 8; //r g b a 每个component bits数目
        size_t bytesPerRow = width * 4; //一张图片每行字节数目(每个像素点包含r g b a 四个字节)
        CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB(); //创建rgb颜色空间
        uint32_t bitmapInfo = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little;
        CGContextRef context = CGBitmapContextCreate(imageData, width, height, bitsPerComponent, bytesPerRow, space, bitmapInfo);
        CGImageRef cgImage = CGBitmapContextCreateImage(context);
        CIImage *ciImage = [CIImage imageWithCGImage:cgImage];
        UIImage *resultImage = [UIImage imageWithCIImage:ciImage];
        NSLog(@"%@", resultImage);
        
        //Release memory
        CGContextRelease(context);
        CGImageRelease(cgImage);
        weakSelf.isSavingImage = NO;
      }
    };
    _sinkRawData->setPixelsCallbck(_rawOutputCallback);
    self.isSavingImage = NO;
  });
}
- (void)initUI {
  NSArray *array = [NSArray arrayWithObjects:@"锐化",@"磨皮",@"美白",@"瘦脸",@"大眼", @"口红", @"腮红", nil];
  //初始化UISegmentedControl
  self.filterSegment = [[UISegmentedControl alloc]initWithItems:array];
  //设置frame
  self.filterSegment.frame = CGRectMake(10,
                                  self.view.frame.size.height - 70,
                                  self.view.frame.size.width - 20,
                                  30);
  self.filterSegment.apportionsSegmentWidthsByContent = YES;
  self.filterSegment.selectedSegmentIndex = 0;
  //添加事件
  [self.filterSegment addTarget:self action:@selector(onFilterSelectChange:) forControlEvents:UIControlEventValueChanged];
  
  //添加到视图
  [self.view addSubview:self.filterSegment];
  [self.view addSubview:self.effectSwitch];
  
  [self.view addSubview:self.cameraSwitchButton];
  
  // 初始化
  self.intensitySlider = [[UISlider alloc] initWithFrame:CGRectMake(50,
                                                           self.view.frame.size.height - 120,
                                                           self.view.frame.size.width - 100,
                                                           30)];
  
  // 设置最小值
  self.intensitySlider.minimumValue = 0;
  
  self.intensitySlider.maximumValue = 10;
  self.intensitySlider.value = 0;
  [self.intensitySlider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
  
  [self.view addSubview:self.intensitySlider];
  
  UIBarButtonItem *backItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(backAction)];
  self.navigationItem.leftBarButtonItem = backItem;
  UIBarButtonItem *saveItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave target:self action:@selector(saveImageAction)];
  self.navigationItem.rightBarButtonItem = saveItem;
}
/// Back to home page and release all memory of gpupixel
- (void)backAction {
  [self.videoCapturer stopCapture];
  self.videoCapturer = nil;
  
  _beautyFaceFilter = nil;
  _faceReshapeFilter = nil;
  _lipstickFilter = nil;
  _blusherFilter = nil;
  [_gpuPixelView removeFromSuperview];
  _gpuPixelView = nil;
  _sinkRawData = nil;
  _sourceRawData = nil;
  gpupixel::GPUPixelContext::getInstance()->destroy();
  _rawOutputCallback = nil;
  
  [self.navigationController popViewControllerAnimated:true];
}
/// Get image from gpupixel after render
- (void)saveImageAction {
  self.isSavingImage = YES;
}

- (void)sliderValueChanged:(UISlider*) slider {
  if (self.filterSegment.selectedSegmentIndex == 0) {  // 锐化
    [self setSharpenValue:slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 1) { // 磨皮
    [self setBlurValue: slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 2) {  // 美白
    [self setWhitenValue: slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 3) {  // 瘦脸
    [self setFaceSlimValue: slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 4) {  // 大眼
    [self setEyeEnlargeValue: slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 5) {  // 口红
    [self setLipstickValue: slider.value];
  } else if (self.filterSegment.selectedSegmentIndex == 6) {  // 腮红
    [self setBlusherValue: slider.value];
  }
}

//点击不同分段就会有不同的事件进行相应
- (void)onFilterSelectChange:(UISegmentedControl *)sender {
  if (self.filterSegment.selectedSegmentIndex == 0) {  // 锐化
    self.intensitySlider.value = _sharpenValue;
  } else if (self.filterSegment.selectedSegmentIndex == 1) {  // 磨皮
    self.intensitySlider.value = _blurValue;
  } else if (self.filterSegment.selectedSegmentIndex == 2) {  // 美白
    self.intensitySlider.value = _whitenValue;
  } else if (self.filterSegment.selectedSegmentIndex == 3) {  // 瘦脸
    self.intensitySlider.value = _faceSlimValue;
  } else if (self.filterSegment.selectedSegmentIndex == 4) {  // 大眼
    self.intensitySlider.value = _eyeEnlargeValue;
  } else if (self.filterSegment.selectedSegmentIndex == 5) {  // 口红
    self.intensitySlider.value = _lipstickValue;
  } else if (self.filterSegment.selectedSegmentIndex == 6) {  // 腮红
    self.intensitySlider.value = _blusherValue;
  }
}
#pragma mark - 属性赋值
- (void)onCameraSwitchBtnUpInside {
  [self.videoCapturer reverseCamera];
}

- (VideoCapturer*)videoCapturer {
  if(_videoCapturer == nil) {
    VCVideoCapturerParam *param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 30;
    
    param.sessionPreset = AVCaptureSessionPresetHigh;
    if(_isCaptureYuvFrame) {
      param.pixelsFormatType = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
    } else {
      param.pixelsFormatType = kCVPixelFormatType_32BGRA;
    }
    
    param.devicePosition = AVCaptureDevicePositionFront;
    param.videoOrientation = AVCaptureVideoOrientationPortrait;
    _videoCapturer = [[VideoCapturer alloc] initWithCaptureParam:param error:nil];
    _videoCapturer.delegate = self;
  }
  
  return _videoCapturer;
}

- (UIButton*)cameraSwitchButton {
  if(_cameraSwitchButton == nil) {
    _cameraSwitchButton = [UIButton buttonWithType:UIButtonTypeSystem];
    _cameraSwitchButton.frame = CGRectMake(self.view.bounds.size.width - 35,
                                        105,
                                        25,
                                        20);
    
    [_cameraSwitchButton setBackgroundImage:[UIImage imageNamed:@"CameraIcon"] forState:UIControlStateNormal];
    [_cameraSwitchButton addTarget: self action: @selector(onCameraSwitchBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
  }
  return _cameraSwitchButton;
}

- (void)setSharpenValue:(CGFloat)value {
  _sharpenValue = value;
  _beautyFaceFilter->setSharpen(value/5);
}

- (void)setBlurValue:(CGFloat)value {
  _blurValue = value;
  _beautyFaceFilter->setBlurAlpha(value/10);
}
- (void)setWhitenValue:(CGFloat)value{
  _whitenValue = value;
  _beautyFaceFilter->setWhite(value/20);
}
- (void)setSaturationValue:(CGFloat)value{
  _saturationValue = value;
}

- (void)setFaceSlimValue:(CGFloat)value{
  _faceSlimValue = value;
  _faceReshapeFilter->setFaceSlimLevel(value/100);
}

- (void)setEyeEnlargeValue:(CGFloat)value{
  _eyeEnlargeValue = value;
  _faceReshapeFilter->setEyeZoomLevel(value/50);
}

- (void)setLipstickValue:(CGFloat)value{
  _lipstickValue = value;
  _lipstickFilter->setBlendLevel(value/10);
}

- (void)setBlusherValue:(CGFloat)value{
  _blusherValue = value;
  _blusherFilter->setBlendLevel(value/10);
}

// camera frame callback
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer {
  //If set OFF, cancel all effects for comparing with the original image
  if (self.effectSwitch.selectedSegmentIndex == 1) {
    _beautyFaceFilter->setSharpen(0);
    _beautyFaceFilter->setBlurAlpha(0);
    _beautyFaceFilter->setWhite(0);
    _faceReshapeFilter->setFaceSlimLevel(0);
    _faceReshapeFilter->setEyeZoomLevel(0);
    _lipstickFilter->setBlendLevel(0);
    _blusherFilter->setBlendLevel(0);
  }
  
  if(_isCaptureYuvFrame) {
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
  } else { //
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    auto width = CVPixelBufferGetWidth(imageBuffer);
    auto height = CVPixelBufferGetHeight(imageBuffer);
    auto stride = CVPixelBufferGetBytesPerRow(imageBuffer)/4;
    auto pixels = (const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
    _sourceRawData->processData(pixels, stride, height, stride);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  }
}

//点击不同分段就会有不同的事件进行相应
- (void)onFilterSwitchChange:(UISegmentedControl *)sender {
  //If set ON, apply all effects for recoverying functions
  if (sender.selectedSegmentIndex == 0) {
    [self setSharpenValue:self.sharpenValue];
    [self setBlurValue: self.blurValue];
    [self setWhitenValue:self.whitenValue];
    [self setFaceSlimValue:self.faceSlimValue];
    [self setEyeEnlargeValue: self.eyeEnlargeValue];
    [self setLipstickValue: self.lipstickValue];
    [self setBlusherValue: self.blusherValue];
  }
}
/// For comparing with the original image
- (UISegmentedControl*)effectSwitch {
  if (_effectSwitch == nil) {
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
