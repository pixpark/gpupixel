//
//  ViewController.m
//  SimpleVideoFilter
//
//  Created by gezhaoyou on 2021/8/25.
//

#import "ViewController.h"
#import "VideoCameraManager.h"
#import <gpupixel/gpupixel.h>
 
using namespace gpupixel;
@interface ViewController() <GPUImageVideoCameraDelegate> {
  std::shared_ptr<SourceRawDataInput> gpuPixelRawInput;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
  std::shared_ptr<TargetRawDataOutput> targetRawOutput_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> lipstick_filter_;
  std::shared_ptr<gpupixel::FaceMakeupFilter> blusher_filter_;
}

@property (weak) IBOutlet NSSlider *levelSlider;
@property (weak) IBOutlet NSSegmentedControl *segment;

//
@property(nonatomic, assign) CGFloat beautyValue;
@property(nonatomic, assign) CGFloat whithValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat thinFaceValue;
@property(nonatomic, assign) CGFloat eyeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;

@property(nonatomic, strong) VideoCameraManager* videoCamera;
 
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

 
    self.levelSlider.continuous = YES;
 

    // init video filter
    [self initVideoFilter];
    self.videoCamera = [[VideoCameraManager alloc] initWithSessionPreset:AVCaptureSessionPreset1280x720 cameraDevice:nil];
    self.videoCamera.delegate = self;
    [self.videoCamera startCameraCapture];

}

-(void)onBtnClicked:(NSView*) sender {
    [self.videoCamera startCameraCapture];
}

- (IBAction)sliderAction:(id)sender {
  NSSlider *slider = (NSSlider *)sender;
  if (self.segment.selectedSegment == 0) {         // 磨皮
    [self setBeautyValue:slider.floatValue];
  } else if (self.segment.selectedSegment == 1) {  // 美白
    [self setWhithValue:slider.floatValue];
  } else if (self.segment.selectedSegment == 2) {  // 瘦脸
    [self setThinFaceValue:slider.floatValue];
  } else if (self.segment.selectedSegment == 3) {  // 大眼
    [self setEyeValue:slider.floatValue];
  } else if (self.segment.selectedSegment == 4) {  // 口红
    [self setLipstickValue:slider.floatValue];
  } else if (self.segment.selectedSegment == 5) {  // 腮红
    [self setBlusherValue: slider.floatValue];
  }
}

- (IBAction)onSegmentSelect:(id)sender {
  if (self.segment.selectedSegment == 0) {         // 磨皮
    self.levelSlider.floatValue = _beautyValue;
  } else if (self.segment.selectedSegment == 1) {  // 美白
    self.levelSlider.floatValue = _whithValue;
  } else if (self.segment.selectedSegment == 2) {  // 瘦脸
    self.levelSlider.floatValue = _thinFaceValue;
  } else if (self.segment.selectedSegment == 3) {  // 大眼
    self.levelSlider.floatValue = _eyeValue;
  } else if (self.segment.selectedSegment == 4) {  // 口红
    self.levelSlider.floatValue = _lipstickValue;
  } else if (self.segment.selectedSegment == 5) {  // 腮红
    self.levelSlider.floatValue = _blusherValue;
  }
}
 
-(void) initVideoFilter {
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    gpuPixelRawInput = SourceRawDataInput::create();
    gpuPixelView = [[GPUPixelView alloc] initWithFrame: self.view.frame];
    [self.view addSubview:gpuPixelView positioned:NSWindowBelow relativeTo:nil];
 
    // create filter
 
    lipstick_filter_ = LipstickFilter::create();
    blusher_filter_ = BlusherFilter::create();
    face_reshape_filter_ = FaceReshapeFilter::create();
    
    gpuPixelRawInput->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstick_filter_->SetFaceLandmarks(landmarks);
       blusher_filter_->SetFaceLandmarks(landmarks);
       face_reshape_filter_->SetFaceLandmarks(landmarks);
     });
 
    // create filter
    targetRawOutput_ = TargetRawDataOutput::create();
    beauty_face_filter_ = BeautyFaceFilter::create();
  
    
    gpuPixelRawInput->addTarget(lipstick_filter_)
                       ->addTarget(blusher_filter_)
                       ->addTarget(face_reshape_filter_)
                       ->addTarget(beauty_face_filter_)
                       ->addTarget(gpuPixelView);
  
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
 

- (void)willOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer {
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    bool yuv = false;
    if(yuv) {
        CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
        // 获取采集的数据
        CVPixelBufferLockBaseAddress(imageBuffer, 0);
        const uint8_t* dataY = (const uint8_t*)CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 0);//YYYYYYYY
        size_t strideY = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 0);
        const uint8_t* dataUV = (const uint8_t*)CVPixelBufferGetBaseAddressOfPlane(imageBuffer, 1);//UVUVUVUV
        size_t strideUV = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, 1);
        size_t width = CVPixelBufferGetWidth(imageBuffer);
        size_t height = CVPixelBufferGetHeight(imageBuffer);
        
//        [self.beautyfaceView renderNV12:dataY
//                                 withStrideY:strideY
//                                  withDataUV:dataUV
//                                withStrideUV:strideUV
//                                   withWidth:width
//                                  withHeight:height];
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    } else {
      CVPixelBufferLockBaseAddress(imageBuffer, 0);
      auto width = CVPixelBufferGetWidth(imageBuffer);
      auto height = CVPixelBufferGetHeight(imageBuffer);
      auto stride = CVPixelBufferGetBytesPerRow(imageBuffer)/4;
      auto pixels = (const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
      gpuPixelRawInput->uploadBytes(pixels, width, height, stride);
      CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    }
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}
 
@end
