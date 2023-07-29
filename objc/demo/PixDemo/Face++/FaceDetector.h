/*
 * Pixdemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <CoreMedia/CoreMedia.h>
#import <UIKit/UIKit.h>

typedef NS_ENUM(NSUInteger, FaceDetectorSampleBufferOrientation) {
  FaceDetectorSampleBufferOrientationCameraFrontAndHorizontallyMirror,
  FaceDetectorSampleBufferOrientationCameraBack,
  FaceDetectorSampleBufferOrientationNoRatation,
  FaceDetectorSampleBufferOrientationRatation90,   // 逆时针90
  FaceDetectorSampleBufferOrientationRatation180,  // 逆时针180
  FaceDetectorSampleBufferOrientationRatation270,  // 逆时针270
};

typedef NS_ENUM(NSUInteger, FaceDetectorSampleType) {
  FaceDetectorSampleTypeNone,
  FaceDetectorSampleTypeCamera,
  FaceDetectorSampleTypeMovieFile,
};

NS_ASSUME_NONNULL_BEGIN

@interface FaceModel : NSObject
@property(nonatomic, assign) CGRect bounds;
@property(nonatomic, copy) NSArray<NSValue*>* landmarks;  // 106 CGPoint

@property(nonatomic, assign) CGFloat yawAngle;
@property(nonatomic, assign) CGFloat rollAngle;   // 翻滚角-歪头
@property(nonatomic, assign) CGFloat pitchAngle;  // 俯仰角-点头

@end

@interface FaceDetector : NSObject

@property(assign, nonatomic) BOOL isAuth;

@property(copy, nonatomic, readonly) NSArray<FaceModel*>* faceModels;
@property(strong, nonatomic, readonly) FaceModel* oneFace;
@property(assign, nonatomic, readonly) BOOL isWorking;
@property(assign, nonatomic) int faceOrientation;
@property(assign, nonatomic)
    FaceDetectorSampleBufferOrientation sampleBufferOrientation;
@property(assign, nonatomic) FaceDetectorSampleType sampleType;

+ (instancetype)shareInstance;
- (void)getLandmarksFromSampleBuffer:(CMSampleBufferRef)detectSampleBufferRef;
- (void)auth;
@end

NS_ASSUME_NONNULL_END
