/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "VideoFilterController.h"
#import <gpupixel/gpupixel.h>
#import "FilterResultViewController.h"
#import "FilterToolbarView.h"
#import "ImageConverter.h"
#import "VideoCapturer.h"

using namespace gpupixel;

@interface VideoFilterController () <VCVideoCapturerDelegate,
                                     FilterToolbarViewDelegate> {
  std::shared_ptr<SourceRawData> _sourceRawData;
  std::shared_ptr<SinkRawData> _sinkRawData;
  GPUPixelView* _gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> _beautyFaceFilter;
  std::shared_ptr<FaceReshapeFilter> _faceReshapeFilter;
  std::shared_ptr<gpupixel::LipstickFilter> _lipstickFilter;
  std::shared_ptr<gpupixel::BlusherFilter> _blusherFilter;
  std::shared_ptr<gpupixel::FaceDetector> _faceDetector;
}

// Filter intensity
@property(nonatomic, assign) CGFloat sharpenValue;
@property(nonatomic, assign) CGFloat blurValue;
@property(nonatomic, assign) CGFloat whitenValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat faceSlimValue;
@property(nonatomic, assign) CGFloat eyeEnlargeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;

// UI
@property(strong, nonatomic) VideoCapturer* videoCapturer;
@property(strong, nonatomic) UIButton* cameraSwitchButton;
@property(strong, nonatomic) FilterToolbarView* filterToolbarView;

@property(assign, nonatomic) BOOL isSave;

@end

@implementation VideoFilterController

#pragma mark - Lazy load
- (VideoCapturer*)videoCapturer {
  if (!_videoCapturer) {
    VCVideoCapturerParam* param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 30;

    param.sessionPreset = AVCaptureSessionPresetHigh;
    param.pixelsFormatType = kCVPixelFormatType_32BGRA;

    param.devicePosition = AVCaptureDevicePositionFront;
    param.videoOrientation = AVCaptureVideoOrientationPortrait;
    _videoCapturer = [[VideoCapturer alloc] initWithCaptureParam:param
                                                           error:nil];
    _videoCapturer.delegate = self;
  }

  return _videoCapturer;
}

- (UIButton*)cameraSwitchButton {
  if (!_cameraSwitchButton) {
    _cameraSwitchButton = [UIButton buttonWithType:UIButtonTypeSystem];
    _cameraSwitchButton.tintColor = UIColor.whiteColor;
    _cameraSwitchButton.translatesAutoresizingMaskIntoConstraints = NO;
    [_cameraSwitchButton setImage:[UIImage imageNamed:@"CameraIcon"]
                         forState:UIControlStateNormal];
    [_cameraSwitchButton addTarget:self
                            action:@selector(onCameraSwitchBtnUpInside)
                  forControlEvents:UIControlEventTouchUpInside];
  }
  return _cameraSwitchButton;
}

- (FilterToolbarView*)filterToolbarView {
  if (!_filterToolbarView) {
    NSArray* filterTitles =
        @[ @"锐化", @"磨皮", @"美白", @"瘦脸", @"大眼", @"口红", @"腮红" ];
    _filterToolbarView = [[FilterToolbarView alloc] initWithFrame:CGRectZero
                                                     filterTitles:filterTitles];
    _filterToolbarView.delegate = self;
    _filterToolbarView.translatesAutoresizingMaskIntoConstraints = NO;
  }
  return _filterToolbarView;
}

#pragma mark - Life cycle
- (void)viewWillAppear:(BOOL)animated {
  [self.videoCapturer startCapture];
  [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
  [self.videoCapturer stopCapture];
  [super viewWillDisappear:animated];
}

- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // 保持屏幕常亮
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];

  // 初始化视频滤镜
  [self setupGPUPixel];
  [self setupUI];

  // 启动相机捕获
  [self.videoCapturer startCapture];
}

#pragma mark - Setup
- (void)setupGPUPixel {
  _gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
  _gpuPixelView.backgroundColor = UIColor.grayColor;
  [self.view addSubview:_gpuPixelView];
  [_gpuPixelView
      setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];

  _sourceRawData = SourceRawData::Create();

  _beautyFaceFilter = BeautyFaceFilter::Create();
  _faceReshapeFilter = FaceReshapeFilter::Create();
  _lipstickFilter = LipstickFilter::Create();
  _blusherFilter = BlusherFilter::Create();
  _sinkRawData = SinkRawData::Create();
  _faceDetector = FaceDetector::Create();

  _sourceRawData->AddSink(_lipstickFilter)
      ->AddSink(_blusherFilter)
      ->AddSink(_faceReshapeFilter)
      ->AddSink(_beautyFaceFilter)
      ->AddSink(_gpuPixelView);

  _beautyFaceFilter->AddSink(_sinkRawData);
  self.isSave = NO;
}

- (void)setupUI {
  [self setupNavigation];
  [self.view addSubview:self.filterToolbarView];
  [self.view addSubview:self.cameraSwitchButton];
  [self setupConstraints];
  [self.filterToolbarView selectFilterAtIndex:0 animated:NO];
}

- (void)setupNavigation {
  UIBarButtonItem* backItem = [[UIBarButtonItem alloc]
      initWithBarButtonSystemItem:UIBarButtonSystemItemCancel
                           target:self
                           action:@selector(backAction)];
  self.navigationItem.leftBarButtonItem = backItem;
  UIBarButtonItem* saveItem = [[UIBarButtonItem alloc]
      initWithBarButtonSystemItem:UIBarButtonSystemItemSave
                           target:self
                           action:@selector(saveAction)];
  self.navigationItem.rightBarButtonItem = saveItem;
}

- (void)setupConstraints {
  [NSLayoutConstraint activateConstraints:@[
    [self.filterToolbarView.centerXAnchor
        constraintEqualToAnchor:self.view.centerXAnchor],
    [self.filterToolbarView.leadingAnchor
        constraintEqualToAnchor:self.view.leadingAnchor],
    [self.filterToolbarView.bottomAnchor
        constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor],

    [self.cameraSwitchButton.topAnchor
        constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor
                       constant:30],
    [self.cameraSwitchButton.trailingAnchor
        constraintEqualToAnchor:self.view.trailingAnchor
                       constant:-16],
    [self.cameraSwitchButton.heightAnchor
        constraintEqualToAnchor:self.cameraSwitchButton.widthAnchor],
    [self.cameraSwitchButton.widthAnchor constraintEqualToConstant:32],
  ]];
}

#pragma mark - Selectors
- (void)backAction {
  self.videoCapturer.delegate = nil;
  [self.videoCapturer stopCapture];
  self.videoCapturer = nil;

  [self destroyGPUPixel];
  [self.navigationController popViewControllerAnimated:true];
}
- (void)destroyGPUPixel {
  _beautyFaceFilter = nil;
  _faceReshapeFilter = nil;
  _lipstickFilter = nil;
  _blusherFilter = nil;
  [_gpuPixelView removeFromSuperview];
  _gpuPixelView = nil;
  _sinkRawData = nil;
  _sourceRawData = nil;
  gpupixel::GPUPixelContext::GetInstance()->Destroy();
}

/// 保存当前处理后的图像并显示结果页面
- (void)saveAction {
  self.isSave = YES;
}

- (void)onCameraSwitchBtnUpInside {
  [self.videoCapturer reverseCamera];
}

/// 图片保存到相册的回调方法
- (void)image:(UIImage*)image
    didFinishSavingWithError:(NSError*)error
                 contextInfo:(void*)contextInfo {
  NSString* message = nil;
  if (error) {
    message = [NSString
        stringWithFormat:@"保存图片失败: %@", error.localizedDescription];
  } else {
    message = @"图片已保存到相册";
  }

  // 显示保存结果提示
  UIAlertController* alert =
      [UIAlertController alertControllerWithTitle:nil
                                          message:message
                                   preferredStyle:UIAlertControllerStyleAlert];
  UIAlertAction* okAction =
      [UIAlertAction actionWithTitle:@"确定"
                               style:UIAlertActionStyleDefault
                             handler:nil];
  [alert addAction:okAction];
  [self presentViewController:alert animated:YES completion:nil];
}

#pragma mark - 属性赋值
- (void)setSharpenValue:(CGFloat)value {
  _sharpenValue = value;
  _beautyFaceFilter->SetSharpen(value / 5);
}

- (void)setBlurValue:(CGFloat)value {
  _blurValue = value;
  _beautyFaceFilter->SetBlurAlpha(value / 10);
}

- (void)setWhitenValue:(CGFloat)value {
  _whitenValue = value;
  _beautyFaceFilter->SetWhite(value / 20);
}

- (void)setSaturationValue:(CGFloat)value {
  _saturationValue = value;
}

- (void)setFaceSlimValue:(CGFloat)value {
  _faceSlimValue = value;
  _faceReshapeFilter->SetFaceSlimLevel(value / 100);
}

- (void)setEyeEnlargeValue:(CGFloat)value {
  _eyeEnlargeValue = value;
  _faceReshapeFilter->SetEyeZoomLevel(value / 50);
}

- (void)setLipstickValue:(CGFloat)value {
  _lipstickValue = value;
  _lipstickFilter->SetBlendLevel(value / 10);
}

- (void)setBlusherValue:(CGFloat)value {
  _blusherValue = value;
  _blusherFilter->SetBlendLevel(value / 10);
}

#pragma mark - VCVideoCapturerDelegate
// 相机帧回调处理
- (void)videoCaptureOutputDataCallback:(CMSampleBufferRef)sampleBuffer {
  CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
  if (!imageBuffer) {
    return;
  }

  CVPixelBufferLockBaseAddress(imageBuffer, 0);
  auto width = CVPixelBufferGetWidth(imageBuffer);
  auto height = CVPixelBufferGetHeight(imageBuffer);
  auto stride = CVPixelBufferGetBytesPerRow(imageBuffer);
  auto pixels = (const uint8_t*)CVPixelBufferGetBaseAddress(imageBuffer);

  // 使用FaceDetector进行人脸检测
  std::vector<float> landmarks =
      _faceDetector->Detect(pixels, width, height, stride,
                            GPUPIXEL_MODE_FMT_VIDEO, GPUPIXEL_FRAME_TYPE_BGRA);

  if (!landmarks.empty()) {
    _lipstickFilter->SetFaceLandmarks(landmarks);
    _blusherFilter->SetFaceLandmarks(landmarks);
    _faceReshapeFilter->SetFaceLandmarks(landmarks);
  }

  _sourceRawData->ProcessData(pixels, stride, height, stride,
                              GPUPIXEL_FRAME_TYPE_BGRA);

  if (self.isSave) {
    dispatch_async(dispatch_get_main_queue(), ^{
      self.videoCapturer.delegate = nil;
      [self.videoCapturer stopCapture];
      self.videoCapturer = nil;
      
      const uint8_t* data = _sinkRawData->GetRgbaBuffer();
      const int w = _sinkRawData->GetWidth();
      const int h = _sinkRawData->GetHeight();
      UIImage* resultImage = [ImageConverter imageFromRGBAData:data width:w height:h];

      // 创建并显示结果页面
      FilterResultViewController* resultVC = [[FilterResultViewController alloc] initWithImage:resultImage];
      [self.navigationController pushViewController:resultVC animated:YES];
      self.isSave = NO;
      [self destroyGPUPixel];
    });
  }

  CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

#pragma mark - FilterToolbarViewDelegate
- (void)filterToolbarView:(FilterToolbarView*)toolbarView
    didSelectFilterAtIndex:(NSInteger)index {
  CGFloat filterValues[] = {
      _sharpenValue,     // 锐化
      _blurValue,        // 磨皮
      _whitenValue,      // 美白
      _faceSlimValue,    // 瘦脸
      _eyeEnlargeValue,  // 大眼
      _lipstickValue,    // 口红
      _blusherValue      // 腮红
  };

  if (index >= 0 && index < sizeof(filterValues) / sizeof(filterValues[0])) {
    [toolbarView updateSliderValue:filterValues[index]];
  }
}

- (void)filterToolbarView:(FilterToolbarView*)toolbarView
     didChangeSliderValue:(CGFloat)value {
  switch (toolbarView.selectedFilterIndex) {
    case 0:  // 锐化
      [self setSharpenValue:value];
      break;
    case 1:  // 磨皮
      [self setBlurValue:value];
      break;
    case 2:  // 美白
      [self setWhitenValue:value];
      break;
    case 3:  // 瘦脸
      [self setFaceSlimValue:value];
      break;
    case 4:  // 大眼
      [self setEyeEnlargeValue:value];
      break;
    case 5:  // 口红
      [self setLipstickValue:value];
      break;
    case 6:  // 腮红
      [self setBlusherValue:value];
      break;
    default:
      break;
  }
}

- (void)filterToolbarView:(FilterToolbarView*)toolbarView
    didChangeEffectSwitchState:(BOOL)isOn {
  if (isOn) {
    _beautyFaceFilter->SetSharpen(_sharpenValue / 5);
    _beautyFaceFilter->SetBlurAlpha(_blurValue / 10);
    _beautyFaceFilter->SetWhite(_whitenValue / 20);
    _faceReshapeFilter->SetFaceSlimLevel(_faceSlimValue / 100);
    _faceReshapeFilter->SetEyeZoomLevel(_eyeEnlargeValue / 50);
    _lipstickFilter->SetBlendLevel(_lipstickValue / 10);
    _blusherFilter->SetBlendLevel(_blusherValue / 10);
  } else {
    _beautyFaceFilter->SetSharpen(0);
    _beautyFaceFilter->SetBlurAlpha(0);
    _beautyFaceFilter->SetWhite(0);
    _faceReshapeFilter->SetFaceSlimLevel(0);
    _faceReshapeFilter->SetEyeZoomLevel(0);
    _lipstickFilter->SetBlendLevel(0);
    _blusherFilter->SetBlendLevel(0);
  }
}

@end
