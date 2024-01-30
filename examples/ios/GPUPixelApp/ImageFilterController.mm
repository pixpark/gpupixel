/*
 * GPUPixelDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "ImageFilterController.h"
#import <AVFoundation/AVSampleBufferDisplayLayer.h>

#import "VideoFilterController.h"
#import "VideoCapturer.h"
#import "FaceDetector.h"
#import <gpupixel/gpupixel.h>

using namespace gpupixel;

@interface ImageFilterController () {
  GPUPixelView *gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> lipstick_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> blusher_filter_;
  std::shared_ptr<SourceImage> gpuSourceImage;
  CMSampleBufferRef sampleImageBuffer;
  CADisplayLink *_displayLink;
}
 
//
@property(nonatomic, assign) CGFloat beautyValue;
@property(nonatomic, assign) CGFloat whithValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat thinFaceValue;
@property(nonatomic, assign) CGFloat eyeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;
@property(nonatomic, assign) Boolean needSetupFaceDetector;

//
@property (strong, nonatomic) UISegmentedControl* segment;
@property (strong, nonatomic) UISegmentedControl* effectSwitch;
@property (strong, nonatomic) UISlider *slider;

@end

@implementation ImageFilterController
- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  // screen always light
  [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
  
  // init video filter
  [self initVideoFilter];
  [self initUI];
 
  NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"sample_face" ofType:@"jpg"];
  UIImage *image = [UIImage imageWithContentsOfFile:imagePath];
 
  sampleImageBuffer = [self uiImageToCMSampleBuffer:image];
  self.needSetupFaceDetector = TRUE;
  
  //
  _displayLink.paused = NO;
}

-(void)initUI {
  NSArray *array = [NSArray arrayWithObjects:@"Smooth", @"White", @"ThinFace", @"BigEye", @"Lipstick", @"Blusher", nil];
  self.segment = [[UISegmentedControl alloc]initWithItems:array];
  self.segment.frame = CGRectMake(10,
                                 self.view.frame.size.height - 70,
                                 self.view.frame.size.width - 20,
                                 30);
  self.segment.apportionsSegmentWidthsByContent = YES;
  self.segment.selectedSegmentIndex = 0;
  [self.segment addTarget:self action:@selector(onFilterSelectChange:) forControlEvents:UIControlEventValueChanged];
 
  [self.view addSubview:self.segment];
  [self.view addSubview:self.effectSwitch];
  
 
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
}

- (void)viewWillDisappear:(BOOL)animated {
  _displayLink.paused = YES;
  [[FaceDetector shareInstance] resetFacepp];
  [super viewWillDisappear:animated];
}

-(void) initVideoFilter {
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
    [self.view addSubview:gpuPixelView];
    
    auto mouth = SourceImage::create(Util::getResourcePath("mouth.png"));
    lipstick_filter_ = FaceMakeupFilter::create();
    lipstick_filter_->setImageTexture(mouth);
    lipstick_filter_->setTextureBounds(FrameBounds{502.5, 710, 262.5, 167.5});

    auto blusher = SourceImage::create(Util::getResourcePath("blusher.png"));
    blusher_filter_ = FaceMakeupFilter::create();
    blusher_filter_->setImageTexture(blusher);
    blusher_filter_->setTextureBounds(FrameBounds{395, 520, 489, 209});

    beauty_face_filter_ = BeautyFaceFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    
    NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"sample_face" ofType:@"jpg"];

    gpuSourceImage = SourceImage::create([imagePath UTF8String]);
    
    // filter pipline
    gpuSourceImage->addTarget(lipstick_filter_)
                    ->addTarget(blusher_filter_)
                    ->addTarget(face_reshape_filter_)
                    ->addTarget(beauty_face_filter_)
                    ->addTarget(gpuPixelView);
 
    [gpuPixelView setBackgroundColor:[UIColor grayColor]];
    [gpuPixelView setFillMode:(gpupixel::TargetView::PreserveAspectRatioAndFill)];
  
  });
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
 
// for face detect
-(CMSampleBufferRef)uiImageToCMSampleBuffer:(UIImage*) image {
  CGImageRef cgImage = [image CGImage];
  
  size_t width = CGImageGetWidth(cgImage);
  size_t height = CGImageGetHeight(cgImage);

  NSDictionary *pixelAttributes = @{
      (__bridge NSString *)kCVPixelBufferCGImageCompatibilityKey: @YES,
      (__bridge NSString *)kCVPixelBufferCGBitmapContextCompatibilityKey: @YES
  };

  CVPixelBufferRef pixelBuffer;
  CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32ARGB, (__bridge CFDictionaryRef)pixelAttributes, &pixelBuffer);
  
  CVPixelBufferLockBaseAddress(pixelBuffer, 0);
  void *baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);

  size_t bytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);
  CGContextRef context = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, CGImageGetColorSpace(cgImage), kCGImageAlphaNoneSkipFirst);
  CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);
  CGContextRelease(context);
  CVPixelBufferUnlockBaseAddress(pixelBuffer, 0);
  
  CMSampleBufferRef sampleBuffer;
  CMVideoFormatDescriptionRef formatDescription;
  CMVideoFormatDescriptionCreateForImageBuffer(kCFAllocatorDefault, pixelBuffer, &formatDescription);

  CMSampleTimingInfo timingInfo = {kCMTimeInvalid, kCMTimeInvalid, kCMTimeInvalid};
  CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault, pixelBuffer, true, NULL, NULL, formatDescription, &timingInfo, &sampleBuffer);

  CFRelease(formatDescription);
  
  return sampleBuffer;
}

- (void)displayLinkDidFire:(CADisplayLink *)displayLink {
  if (![FaceDetector shareInstance].isWorking) {
    if(self.needSetupFaceDetector) {
      // setup face++
      [FaceDetector shareInstance].sampleBufferOrientation = FaceDetectorSampleBufferOrientationNoRatation;
      [FaceDetector shareInstance].faceOrientation = 0;
      [FaceDetector shareInstance].sampleType = FaceDetectorSampleTypeCamera;
      [[FaceDetector shareInstance] auth];
      
      self.needSetupFaceDetector = NO;
    }
    
    [[FaceDetector shareInstance] getLandmarksFromSampleBuffer:sampleImageBuffer];
    
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
  // must call proceed function
  gpuSourceImage->proceed();
}
 
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
 
