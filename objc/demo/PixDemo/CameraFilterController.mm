/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "CameraFilterController.h"
#import "VideoCapturer.h"
#import "VideoPreview.h"
#import "FaceDetector.h"

@interface CameraFilterController () <VCVideoCapturerDelegate> {
    bool captureYuvFrame;
}

@property (strong, nonatomic) VideoPreview * faceBeautyVideoView;
@property (strong, nonatomic) VideoCapturer* capturer;

// Slider
@property (strong, nonatomic) UISlider* faceSmoothSlider;
@property (strong, nonatomic) UISlider* skinWhitenSlider;
@property (strong, nonatomic) UISlider* faceSlimSlider;
@property (strong, nonatomic) UISlider* bigEyeSlider;
// Label
@property (strong, nonatomic) UILabel* faceSmoothLabel;
@property (strong, nonatomic) UILabel* skinWhitenLabel;
@property (strong, nonatomic) UILabel* faceSlimLabel;
@property (strong, nonatomic) UILabel* bigEyeLabel;

// Button
@property (strong, nonatomic) UIButton* effectToggleBtn;
@end

@implementation CameraFilterController

- (void)viewDidLoad {
  [super viewDidLoad];
  [self.view setBackgroundColor:UIColor.whiteColor];
  captureYuvFrame = false;
  
  self.faceBeautyVideoView = [[VideoPreview alloc] initWithFrame:self.view.bounds];
  [self.view addSubview:self.faceBeautyVideoView];
  // slider
  self.faceSmoothSlider.value = 0.0f;
  self.skinWhitenSlider.value = 0.0f;
  self.faceSlimSlider.value = 0.0f;
  self.bigEyeSlider.value = 0.0f;
  [self.view addSubview:self.faceSmoothSlider];
  [self.view addSubview:self.skinWhitenSlider];
  [self.view addSubview:self.faceSlimSlider];
  [self.view addSubview:self.bigEyeSlider];
  
  // Label
  [self.view addSubview:self.faceSmoothLabel];
  [self.view addSubview:self.skinWhitenLabel];
  [self.view addSubview:self.faceSlimLabel];
  [self.view addSubview:self.bigEyeLabel];
  
  // toggle button
  [self.view addSubview:self.effectToggleBtn];
  
  
  [self.faceBeautyVideoView setFaceSmoothLevel:self.faceSmoothSlider.value];
  [self.faceBeautyVideoView setWhitenLevel:self.skinWhitenSlider.value];
  [self.faceBeautyVideoView setFaceSlimLevel:self.faceSlimSlider.value];
  [self.faceBeautyVideoView setEyeZoomLevel:self.bigEyeSlider.value];
  // Do any additional setup after loading the view from its nib.
  
  [self setupFacepp];
  
  // start camera capture
  [self.capturer startCapture];
}

- (void)viewWillDisappear:(BOOL)animated {
  [self.capturer stopCapture];
  
  [super viewWillDisappear:animated];
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
    
    [self.faceBeautyVideoView renderNV12:dataY
                             withStrideY:strideY
                              withDataUV:dataUV
                            withStrideUV:strideUV
                               withWidth:width
                              withHeight:height];
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  } else {
#if 0
    if (![FaceDetector shareInstance].isWorking) {
      CMSampleBufferRef detectSampleBufferRef = NULL;
      CMSampleBufferCreateCopy(kCFAllocatorDefault, sampleBuffer, &detectSampleBufferRef);
      
      
      [[FaceDetector shareInstance] getLandmarksFromSampleBuffer:detectSampleBufferRef];
      
      CFRelease(detectSampleBufferRef);
      
    }
    
    NSArray *landmarks = [FaceDetector shareInstance].oneFace.landmarks;
    if(landmarks) {
      [self.beautyFaceVideoView setFacePoint: landmarks];
    }
#endif
    //
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    [self.faceBeautyVideoView renderRGBA:CVPixelBufferGetWidth(imageBuffer)
                              withHeight:CVPixelBufferGetHeight(imageBuffer)
                              withStride:CVPixelBufferGetBytesPerRow(imageBuffer)/4
                              withPixels:(const uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer)];
    CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
  }
}

// setup face++ detect
- (void)setupFacepp {
  [FaceDetector shareInstance].sampleBufferOrientation = FaceDetectorSampleBufferOrientationNoRatation;
  [FaceDetector shareInstance].faceOrientation = 0;
  [FaceDetector shareInstance].sampleType = FaceDetectorSampleTypeCamera;
  
}

-(void)onSwitchBtnPress {
  if(self.faceBeautyVideoView) {
    [self.faceBeautyVideoView switchBeautyFace:false];
  }
}

-(void)onSwitchBtnUpInside {
  if(self.faceBeautyVideoView) {
    [self.faceBeautyVideoView switchBeautyFace:true];
  }
}

-(VideoCapturer*)capturer {
  if(_capturer == nil) {
    VCVideoCapturerParam *param = [[VCVideoCapturerParam alloc] init];
    param.frameRate = 15;
    
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

-(UISlider*)faceSmoothSlider {
  if(_faceSmoothSlider == nil) {
    _faceSmoothSlider = [[UISlider alloc] initWithFrame:CGRectMake(80.0,
                                                                   self.view.bounds.size.height - 250,
                                                                   self.view.bounds.size.width - 60.0 * 2,
                                                                   60.0)];
    _faceSmoothSlider.minimumValue = .0;
    _faceSmoothSlider.maximumValue = 10.0;
    
    [_faceSmoothSlider addTarget:self action:@selector(sliderValueDidChanged:) forControlEvents:UIControlEventValueChanged];
  }
  
  return _faceSmoothSlider;
}

-(UISlider*)skinWhitenSlider {
  if(_skinWhitenSlider == nil) {
    _skinWhitenSlider = [[UISlider alloc] initWithFrame:CGRectMake(80.0,
                                                                   self.view.bounds.size.height - 200,
                                                                   self.view.bounds.size.width - 60.0 * 2,
                                                                   60.0)];
    _skinWhitenSlider.minimumValue = .0;
    _skinWhitenSlider.maximumValue = 10.0;
    
    [_skinWhitenSlider addTarget:self action:@selector(sliderValueDidChanged:) forControlEvents:UIControlEventValueChanged];
  }
  
  return _skinWhitenSlider;
}

-(UISlider*)faceSlimSlider {
  if(_faceSlimSlider == nil) {
    _faceSlimSlider = [[UISlider alloc] initWithFrame:CGRectMake(80.0,
                                                                 self.view.bounds.size.height - 150,
                                                                 self.view.bounds.size.width - 60.0 * 2,
                                                                 60.0)];
    _faceSlimSlider.minimumValue = .0;
    _faceSlimSlider.maximumValue = 10.0;
    _faceSlimSlider.enabled = false;
    
    [_faceSlimSlider addTarget:self action:@selector(sliderValueDidChanged:) forControlEvents:UIControlEventValueChanged];
  }
  
  return _faceSlimSlider;
}

-(UISlider*)bigEyeSlider {
  if(_bigEyeSlider == nil) {
    _bigEyeSlider = [[UISlider alloc] initWithFrame:CGRectMake(80.0,
                                                               self.view.bounds.size.height - 100,
                                                               self.view.bounds.size.width - 60.0 * 2,
                                                               60.0)];
    _bigEyeSlider.minimumValue = .0;
    _bigEyeSlider.maximumValue = 10.0;
    _bigEyeSlider.enabled = false;
    [_bigEyeSlider addTarget:self action:@selector(sliderValueDidChanged:) forControlEvents:UIControlEventValueChanged];
  }
  
  return _bigEyeSlider;
}

- (void)sliderValueDidChanged:(UISlider *)slider {
  if(slider == self.faceSmoothSlider) {
    [self.faceBeautyVideoView setFaceSmoothLevel:slider.value];
  } else if(slider == self.skinWhitenSlider) {
    [self.faceBeautyVideoView setWhitenLevel:slider.value];
  } else if(slider == self.faceSlimSlider) {
    [self.faceBeautyVideoView setFaceSlimLevel:slider.value];
  } else if(slider == self.bigEyeSlider) {
    [self.faceBeautyVideoView setEyeZoomLevel:slider.value];
  }
}

-(UILabel*) faceSmoothLabel {
  if(_faceSmoothLabel == nil) {
    _faceSmoothLabel = [[UILabel alloc] initWithFrame:CGRectMake(35.0,
                                                                 self.view.bounds.size.height - 250,
                                                                 40,
                                                                 60.0)];
    _faceSmoothLabel.text = @"磨皮";
    _faceSmoothLabel.textColor = [UIColor whiteColor];
  }
  return _faceSmoothLabel;
}

-(UILabel*) skinWhitenLabel {
  if(_skinWhitenLabel == nil) {
    _skinWhitenLabel = [[UILabel alloc] initWithFrame:CGRectMake(35.0,
                                                                 self.view.bounds.size.height - 200,
                                                                 40,
                                                                 60.0)];
    _skinWhitenLabel.text = @"美白";
    _skinWhitenLabel.textColor = [UIColor whiteColor];
  }
  return _skinWhitenLabel;
}
-(UILabel*) faceSlimLabel {
  if(_faceSlimLabel == nil) {
    _faceSlimLabel = [[UILabel alloc] initWithFrame:CGRectMake(35.0,
                                                               self.view.bounds.size.height - 150,
                                                               40,
                                                               60.0)];
    _faceSlimLabel.text = @"瘦脸";
    _faceSlimLabel.textColor = [UIColor whiteColor];
  }
  return _faceSlimLabel;
}

-(UILabel*) bigEyeLabel {
  if(_bigEyeLabel == nil) {
    _bigEyeLabel = [[UILabel alloc] initWithFrame:CGRectMake(35.0,
                                                             self.view.bounds.size.height - 100,
                                                             40,
                                                             60.0)];
    _bigEyeLabel.text = @"大眼";
    _bigEyeLabel.textColor = [UIColor whiteColor];
  }
  return _bigEyeLabel;
}
 
-(UIButton*) effectToggleBtn {
    if(_effectToggleBtn == nil) {
        _effectToggleBtn = [UIButton buttonWithType:UIButtonTypeSystem];
        _effectToggleBtn.frame = CGRectMake(self.view.bounds.size.width - 60,
                                            self.view.bounds.size.height - 270,
                                            30,
                                            25);
        [_effectToggleBtn setBackgroundImage:[UIImage imageNamed:@"switch.png"] forState:UIControlStateNormal];
        [_effectToggleBtn addTarget: self action: @selector(onSwitchBtnPress) forControlEvents: UIControlEventTouchDown];
        [_effectToggleBtn addTarget: self action: @selector(onSwitchBtnUpInside) forControlEvents: UIControlEventTouchUpInside] ;
    }
    return _effectToggleBtn;
}
@end
