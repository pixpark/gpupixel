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
  std::shared_ptr<SourceRawData> sourceRawData;
  std::shared_ptr<SinkRawData> sinkRawData;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beautyFaceFilter;
  std::shared_ptr<FaceReshapeFilter> faceReshapeFilter;
  std::shared_ptr<gpupixel::LipstickFilter> lipstickFilter;
  std::shared_ptr<gpupixel::BlusherFilter> blusherFilter;
  
  RawOutputCallback rawOutputCallback;
}

//
@property(nonatomic, assign) CGFloat sharpenIntensity;
@property(nonatomic, assign) CGFloat blurIntensity;
@property(nonatomic, assign) CGFloat whitenIntensity;
@property(nonatomic, assign) CGFloat saturationIntensity;
@property(nonatomic, assign) CGFloat faceSlimIntensity;
@property(nonatomic, assign) CGFloat eyeEnlargeIntensity;
@property(nonatomic, assign) CGFloat lipstickIntensity;
@property(nonatomic, assign) CGFloat blusherIntensity;

// UI
@property (strong, nonatomic) VideoCapturer* capturer;
@property (strong, nonatomic) UIButton* cameraSwitchBtn;
@property (strong, nonatomic) UISegmentedControl* segment;
@property (strong, nonatomic) UISegmentedControl* effectSwitch;
@property (strong, nonatomic) UISlider *slider;

@property (assign, nonatomic) BOOL isTrySaveImage;

@end

@implementation VideoFilterController
- (void)viewWillDisappear:(BOOL)animated {
  [self.capturer stopCapture];
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
  
  captureYuvFrame = false;
  // start camera capture
  [self.capturer startCapture];
}
- (void) initVideoFilter {
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    sourceRawData = SourceRawData::create();
    gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:gpuPixelView];
    
    lipstickFilter = LipstickFilter::create();
    blusherFilter = BlusherFilter::create();
    sourceRawData->RegLandmarkCallback([=](std::vector<float> landmarks) {
      lipstickFilter->SetFaceLandmarks(landmarks);
      blusherFilter->SetFaceLandmarks(landmarks);
      faceReshapeFilter->SetFaceLandmarks(landmarks);
    });
    
    // create filter
    sinkRawData = SinkRawData::create();
    beautyFaceFilter = BeautyFaceFilter::create();
    faceReshapeFilter = FaceReshapeFilter::create();
    // filter pipline
    sourceRawData->addSink(lipstickFilter)
    ->addSink(blusherFilter)
    ->addSink(faceReshapeFilter)
    ->addSink(beautyFaceFilter)
    ->addSink(gpuPixelView);
    
    [gpuPixelView setBackgroundColor:[UIColor grayColor]];
    [gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];
    
    beautyFaceFilter->addSink(sinkRawData);
    __weak typeof(VideoFilterController*)weakSelf = self;
    rawOutputCallback = [weakSelf]( const uint8_t* data, int width, int height, int64_t ts) {
      if (weakSelf.isTrySaveImage == YES) {
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
        weakSelf.isTrySaveImage = NO;
      }
    };
    sinkRawData->setPixelsCallbck(rawOutputCallback);
    self.isTrySaveImage = NO;
  });
}
- (void)initUI {
  NSArray *array = [NSArray arrayWithObjects:@"锐化",@"磨皮",@"美白",@"瘦脸",@"大眼", @"口红", @"腮红", nil];
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
  self.slider = [[UISlider alloc] initWithFrame:CGRectMake(50,
                                                           self.view.frame.size.height - 120,
                                                           self.view.frame.size.width - 100,
                                                           30)];
  
  // 设置最小值
  self.slider.minimumValue = 0;
  
  self.slider.maximumValue = 10;
  self.slider.value = 0;
  [self.slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
  
  [self.view addSubview:self.slider];
  
  UIBarButtonItem *backItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(backAction)];
  self.navigationItem.leftBarButtonItem = backItem;
  UIBarButtonItem *saveItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave target:self action:@selector(saveImageAction)];
  self.navigationItem.rightBarButtonItem = saveItem;
}
/// Back to home page and release all memory of gpupixel
- (void)backAction {
  [self.capturer stopCapture];
  self.capturer = nil;
  
  beautyFaceFilter = nil;
  faceReshapeFilter = nil;
  lipstickFilter = nil;
  blusherFilter = nil;
  [gpuPixelView removeFromSuperview];
  gpuPixelView = nil;
  sinkRawData = nil;
  sourceRawData = nil;
  gpupixel::GPUPixelContext::getInstance()->destroy();
  rawOutputCallback = nil;
  
  [self.navigationController popViewControllerAnimated:true];
}
/// Get image from gpupixel after render
- (void)saveImageAction {
  self.isTrySaveImage = YES;
}

- (void)sliderValueChanged:(UISlider*) slider {
  if (self.segment.selectedSegmentIndex == 0) {  // 锐化
    [self setSharpenIntensity:slider.value];
  } else if (self.segment.selectedSegmentIndex == 1) { // 磨皮
    [self setBlurIntensity: slider.value];
  } else if (self.segment.selectedSegmentIndex == 2) {  // 美白
    [self setWhitenIntensity: slider.value];
  } else if (self.segment.selectedSegmentIndex == 3) {  // 瘦脸
    [self setFaceSlimIntensity: slider.value];
  } else if (self.segment.selectedSegmentIndex == 4) {  // 大眼
    [self setEyeEnlargeIntensity: slider.value];
  } else if (self.segment.selectedSegmentIndex == 5) {  // 口红
    [self setLipstickIntensity: slider.value];
  } else if (self.segment.selectedSegmentIndex == 6) {  // 腮红
    [self setBlusherIntensity: slider.value];
  }
}

//点击不同分段就会有不同的事件进行相应
- (void)onFilterSelectChange:(UISegmentedControl *)sender {
  if (self.segment.selectedSegmentIndex == 0) {  // 锐化
    self.slider.value = _sharpenIntensity;
  } else if (self.segment.selectedSegmentIndex == 1) {  // 磨皮
    self.slider.value = _blurIntensity;
  } else if (self.segment.selectedSegmentIndex == 2) {  // 美白
    self.slider.value = _whitenIntensity;
  } else if (self.segment.selectedSegmentIndex == 3) {  // 瘦脸
    self.slider.value = _faceSlimIntensity;
  } else if (self.segment.selectedSegmentIndex == 4) {  // 大眼
    self.slider.value = _eyeEnlargeIntensity;
  } else if (self.segment.selectedSegmentIndex == 5) {  // 口红
    self.slider.value = _lipstickIntensity;
  } else if (self.segment.selectedSegmentIndex == 6) {  // 腮红
    self.slider.value = _blusherIntensity;
  }
}
#pragma mark - 属性赋值
- (void)onCameraSwitchBtnUpInside {
  [self.capturer reverseCamera];
}

- (VideoCapturer*)capturer {
  if(_capturer == nil) {
    VCVideoCapturerParam *param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 30;
    
    param.sessionPreset = AVCaptureSessionPresetHigh;
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

- (UIButton*)cameraSwitchBtn {
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

- (void)setSharpenIntensity:(CGFloat)value {
  _sharpenIntensity = value;
  beautyFaceFilter->setSharpen(value/5);
}

- (void)setBlurIntensity:(CGFloat)value {
  _blurIntensity = value;
  beautyFaceFilter->setBlurAlpha(value/10);
}
- (void)setWhitenIntensity:(CGFloat)value{
  _whitenIntensity = value;
  beautyFaceFilter->setWhite(value/20);
}
- (void)setSaturationIntensity:(CGFloat)value{
  _saturationIntensity = value;
}

- (void)setFaceSlimIntensity:(CGFloat)value{
  _faceSlimIntensity = value;
  faceReshapeFilter->setFaceSlimLevel(value/100);
}

- (void)setEyeEnlargeIntensity:(CGFloat)value{
  _eyeEnlargeIntensity = value;
  faceReshapeFilter->setEyeZoomLevel(value/50);
}

- (void)setLipstickIntensity:(CGFloat)value{
  _lipstickIntensity = value;
  lipstickFilter->setBlendLevel(value/10);
}

- (void)setBlusherIntensity:(CGFloat)value{
  _blusherIntensity = value;
  blusherFilter->setBlendLevel(value/10);
}

// camera frame callback
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer {
  //If set OFF, cancel all effects for comparing with the original image
  if (self.effectSwitch.selectedSegmentIndex == 1) {
    beautyFaceFilter->setSharpen(0);
    beautyFaceFilter->setBlurAlpha(0);
    beautyFaceFilter->setWhite(0);
    faceReshapeFilter->setFaceSlimLevel(0);
    faceReshapeFilter->setEyeZoomLevel(0);
    lipstickFilter->setBlendLevel(0);
    blusherFilter->setBlendLevel(0);
  }
  
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
  } else { //
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    auto width = CVPixelBufferGetWidth(imageBuffer);
    auto height = CVPixelBufferGetHeight(imageBuffer);
    auto stride = CVPixelBufferGetBytesPerRow(imageBuffer)/4;
    auto pixels = (const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
    sourceRawData->processData(pixels, stride, height, stride);
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  }
}

//点击不同分段就会有不同的事件进行相应
- (void)onFilterSwitchChange:(UISegmentedControl *)sender {
  //If set ON, apply all effects for recoverying functions
  if (sender.selectedSegmentIndex == 0) {
    [self setSharpenIntensity:self.sharpenIntensity];
    [self setBlurIntensity: self.blurIntensity];
    [self setWhitenIntensity:self.whitenIntensity];
    [self setFaceSlimIntensity:self.faceSlimIntensity];
    [self setEyeEnlargeIntensity: self.eyeEnlargeIntensity];
    [self setLipstickIntensity: self.lipstickIntensity];
    [self setBlusherIntensity: self.blusherIntensity];
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
