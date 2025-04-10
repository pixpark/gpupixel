//
//  ViewController.m
//  SimpleVideoFilter
//
//  Created by PixPark on 2021/8/25.
//

#import "ViewController.h"
#import <gpupixel/gpupixel.h>
#import "VideoCameraManager.h"

using namespace gpupixel;
@interface ViewController () <GPUImageVideoCameraDelegate> {
  std::shared_ptr<SourceRawData> sourceRawData;
  GPUPixelView* gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beautyFaceFilter;
  std::shared_ptr<FaceReshapeFilter> faceReshapeFilter;
  std::shared_ptr<gpupixel::LipstickFilter> lipstickFilter;
  std::shared_ptr<gpupixel::BlusherFilter> blusherFilter;
  std::shared_ptr<gpupixel::FaceDetector> faceDetector;
}

@property(weak) IBOutlet NSSlider* levelSlider;
@property(weak) IBOutlet NSSegmentedControl* segment;

//
@property(nonatomic, assign) CGFloat beautyIntensity;
@property(nonatomic, assign) CGFloat whitenIntensity;
@property(nonatomic, assign) CGFloat saturationIntensity;
@property(nonatomic, assign) CGFloat faceSlimIntensity;
@property(nonatomic, assign) CGFloat eyeEnlargeIntensity;
@property(nonatomic, assign) CGFloat lipstickIntensity;
@property(nonatomic, assign) CGFloat blusherIntensity;

@property(nonatomic, strong) VideoCameraManager* videoCamera;

@end

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];

  self.levelSlider.continuous = YES;

  // init video filter
  [self initVideoFilter];
  self.videoCamera = [[VideoCameraManager alloc]
      initWithSessionPreset:AVCaptureSessionPreset1280x720
               cameraDevice:nil];
  self.videoCamera.delegate = self;
  [self.videoCamera startCameraCapture];
}

- (void)onBtnClicked:(NSView*)sender {
  [self.videoCamera startCameraCapture];
}

- (IBAction)sliderAction:(id)sender {
  NSSlider* slider = (NSSlider*)sender;
  if (self.segment.selectedSegment == 0) {  // 磨皮
    [self setBeautyIntensity:slider.floatValue];
  } else if (self.segment.selectedSegment == 1) {  // 美白
    [self setWhitenIntensity:slider.floatValue];
  } else if (self.segment.selectedSegment == 2) {  // 瘦脸
    [self setFaceSlimIntensity:slider.floatValue];
  } else if (self.segment.selectedSegment == 3) {  // 大眼
    [self setEyeEnlargeIntensity:slider.floatValue];
  } else if (self.segment.selectedSegment == 4) {  // 口红
    [self setLipstickIntensity:slider.floatValue];
  } else if (self.segment.selectedSegment == 5) {  // 腮红
    [self setBlusherIntensity:slider.floatValue];
  }
}

- (IBAction)onSegmentSelect:(id)sender {
  if (self.segment.selectedSegment == 0) {  // 磨皮
    self.levelSlider.floatValue = _beautyIntensity;
  } else if (self.segment.selectedSegment == 1) {  // 美白
    self.levelSlider.floatValue = _whitenIntensity;
  } else if (self.segment.selectedSegment == 2) {  // 瘦脸
    self.levelSlider.floatValue = _faceSlimIntensity;
  } else if (self.segment.selectedSegment == 3) {  // 大眼
    self.levelSlider.floatValue = _eyeEnlargeIntensity;
  } else if (self.segment.selectedSegment == 4) {  // 口红
    self.levelSlider.floatValue = _lipstickIntensity;
  } else if (self.segment.selectedSegment == 5) {  // 腮红
    self.levelSlider.floatValue = _blusherIntensity;
  }
}

- (void)initVideoFilter {
  gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
  [self.view addSubview:gpuPixelView positioned:NSWindowBelow relativeTo:nil];
  [gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];

  sourceRawData = SourceRawData::Create();

  // create filter

  lipstickFilter = LipstickFilter::Create();
  blusherFilter = BlusherFilter::Create();
  faceReshapeFilter = FaceReshapeFilter::Create();
  faceDetector = FaceDetector::Create();

  // create filter
  beautyFaceFilter = BeautyFaceFilter::Create();

  sourceRawData->AddSink(lipstickFilter)
      ->AddSink(blusherFilter)
      ->AddSink(faceReshapeFilter)
      ->AddSink(beautyFaceFilter)
      ->AddSink(gpuPixelView);
}

#pragma mark - 属性赋值
- (void)setBeautyIntensity:(CGFloat)value {
  _beautyIntensity = value;
  beautyFaceFilter->SetBlurAlpha(value / 10);
}
- (void)setWhitenIntensity:(CGFloat)value {
  _whitenIntensity = value;
  beautyFaceFilter->SetWhite(value / 20);
}
- (void)setSaturationIntensity:(CGFloat)value {
  _saturationIntensity = value;
}

- (void)setFaceSlimIntensity:(CGFloat)value {
  _faceSlimIntensity = value;
  faceReshapeFilter->SetFaceSlimLevel(value / 100);
}

- (void)setEyeEnlargeIntensity:(CGFloat)value {
  _eyeEnlargeIntensity = value;
  faceReshapeFilter->SetEyeZoomLevel(value / 50);
}

- (void)setLipstickIntensity:(CGFloat)value {
  _lipstickIntensity = value;
  lipstickFilter->SetBlendLevel(value / 10);
}

- (void)setBlusherIntensity:(CGFloat)value {
  _blusherIntensity = value;
  blusherFilter->SetBlendLevel(value / 10);
}

- (void)willOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer {
  CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
  CVPixelBufferLockBaseAddress(imageBuffer, 0);

  auto width = CVPixelBufferGetWidth(imageBuffer);
  auto height = CVPixelBufferGetHeight(imageBuffer);
  auto stride = CVPixelBufferGetBytesPerRow(imageBuffer);
  auto pixels = (const uint8_t*)CVPixelBufferGetBaseAddress(imageBuffer);

  std::vector<float> landmarks =
      faceDetector->Detect(pixels, width, height, stride,
                           GPUPIXEL_MODE_FMT_VIDEO, GPUPIXEL_FRAME_TYPE_BGRA);

  if (!landmarks.empty()) {
    lipstickFilter->SetFaceLandmarks(landmarks);
    blusherFilter->SetFaceLandmarks(landmarks);
    faceReshapeFilter->SetFaceLandmarks(landmarks);
  }

  sourceRawData->ProcessData(pixels, width, height, stride,
                             GPUPIXEL_FRAME_TYPE_BGRA);
  CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
}

- (void)setRepresentedObject:(id)representedObject {
  [super setRepresentedObject:representedObject];

  // Update the view, if already loaded.
}

@end
