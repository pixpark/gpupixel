/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "ImageFilterController.h"
#import "FilterResultViewController.h"
#import "FilterToolbarView.h"
#import "ImageConverter.h"

#import <gpupixel/face_detector.h>
#import <gpupixel/gpupixel.h>

using namespace gpupixel;

@interface ImageFilterController () <FilterToolbarViewDelegate> {
  GPUPixelView* _gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> _beautyFaceFilter;
  std::shared_ptr<FaceReshapeFilter> _faceReshapeFilter;
  std::shared_ptr<gpupixel::FaceMakeupFilter> _lipstickFilter;
  std::shared_ptr<gpupixel::FaceMakeupFilter> _blusherFilter;
  std::shared_ptr<SourceImage> _gpuSourceImage;
  std::shared_ptr<gpupixel::FaceDetector> _faceDetector;
  CADisplayLink* _displayLink;
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
@property(nonatomic, strong) FilterToolbarView* filterToolbarView;

@end

@implementation ImageFilterController

#pragma mark - Lazy load
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
  _displayLink.paused = NO;
  [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
  _displayLink.paused = YES;
  [super viewWillDisappear:animated];
}

- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // screen always light
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];

  // setup filter
  [self setupFilter];
  [self setupUI];
  [self setupDisplayLink];
}

#pragma mark - Setup
- (void)setupFilter {
  gpupixel::GPUPixelContext::GetInstance()->RunSync([&] {
    _gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    _gpuPixelView.backgroundColor = UIColor.grayColor;
    [self.view addSubview:_gpuPixelView];
    [_gpuPixelView
        setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];

    _lipstickFilter = LipstickFilter::Create();
    _blusherFilter = BlusherFilter::Create();
    _beautyFaceFilter = BeautyFaceFilter::Create();
    _faceReshapeFilter = FaceReshapeFilter::Create();
    _faceDetector = std::make_shared<FaceDetector>();

    NSString* imagePath = [[NSBundle mainBundle] pathForResource:@"sample_face"
                                                          ofType:@"png"];
    _gpuSourceImage = SourceImage::Create([imagePath UTF8String]);

    // filter pipline
    _gpuSourceImage->AddSink(_lipstickFilter)
        ->AddSink(_blusherFilter)
        ->AddSink(_faceReshapeFilter)
        ->AddSink(_beautyFaceFilter)
        ->AddSink(_gpuPixelView);
  });
}

- (void)setupUI {
  [self setupNavigation];
  [self.view addSubview:self.filterToolbarView];
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
  ]];
}

- (void)setupDisplayLink {
  _displayLink =
      [CADisplayLink displayLinkWithTarget:self
                                  selector:@selector(displayLinkDidFire:)];
  _displayLink.paused = YES;
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_10_0
  _displayLink.preferredFramesPerSecond = 30;
#else
  [_displayLink setFrameInterval:2];
#endif
  [_displayLink addToRunLoop:[NSRunLoop currentRunLoop]
                     forMode:NSRunLoopCommonModes];

  _displayLink.paused = NO;
}

#pragma mark - Selectors
/// Back to home page and release all memory of gpupixel
- (void)backAction {
  [_displayLink invalidate];
  _displayLink = nil;

  _beautyFaceFilter = nil;
  _faceReshapeFilter = nil;
  _lipstickFilter = nil;
  _blusherFilter = nil;
  [_gpuPixelView removeFromSuperview];
  _gpuPixelView = nil;
  _gpuSourceImage = nil;
  _faceDetector = nullptr;
  gpupixel::GPUPixelContext::GetInstance()->Destroy();

  [self.navigationController popViewControllerAnimated:true];
}

/// Get image from gpupixel after render and show result page
- (void)saveAction {
  int width = _gpuSourceImage->GetRotatedFramebufferWidth();
  int height = _gpuSourceImage->GetRotatedFramebufferHeight();
  // "beauty_face_filter_" is last filter in gpuSourceImage
  unsigned char* pixels =
      _gpuSourceImage->GetProcessedFrameData(_beautyFaceFilter, width, height);
  UIImage* resultImage = [ImageConverter imageFromRGBAData:pixels
                                                     width:width
                                                    height:height];

  // 创建并显示结果页面
  FilterResultViewController* resultVC =
      [[FilterResultViewController alloc] initWithImage:resultImage];
  [self.navigationController pushViewController:resultVC animated:YES];

  // Release memory
  free(pixels);
}

- (void)displayLinkDidFire:(CADisplayLink*)displayLink {
  if (!self.filterToolbarView.effectSwitch.isOn) {
    _beautyFaceFilter->SetSharpen(0);
    _beautyFaceFilter->SetBlurAlpha(0);
    _beautyFaceFilter->SetWhite(0);
    _faceReshapeFilter->SetFaceSlimLevel(0);
    _faceReshapeFilter->SetEyeZoomLevel(0);
    _lipstickFilter->SetBlendLevel(0);
    _blusherFilter->SetBlendLevel(0);
  }

  int width = _gpuSourceImage->GetWidth();
  int height = _gpuSourceImage->GetHeight();
  const unsigned char* buffer = _gpuSourceImage->GetRgbaImageBuffer();

  std::vector<float> landmarks =
      _faceDetector->Detect(buffer, width, height, GPUPIXEL_MODE_FMT_PICTURE,
                            GPUPIXEL_FRAME_TYPE_RGBA);

  if (!landmarks.empty()) {
    _lipstickFilter->SetFaceLandmarks(landmarks);
    _blusherFilter->SetFaceLandmarks(landmarks);
    _faceReshapeFilter->SetFaceLandmarks(landmarks);
  }

  _gpuSourceImage->Render();
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
    [self setSharpenValue:self.sharpenValue];
    [self setBlurValue:self.blurValue];
    [self setWhitenValue:self.whitenValue];
    [self setFaceSlimValue:self.faceSlimValue];
    [self setEyeEnlargeValue:self.eyeEnlargeValue];
    [self setLipstickValue:self.lipstickValue];
    [self setBlusherValue:self.blusherValue];
  }
}

@end
