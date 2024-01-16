/*
 * Pixdemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "MGFaceLicenseHandle.h"
#import "MGFaceInfo.h"
#import "MGFacepp.h"
#import "FaceDetector.h"

CGPoint center(CGPoint point1, CGPoint point2) {
    CGPoint point = CGPointMake(point1.x + point2.x, point1.y + point2.y);
    point = CGPointMake(point.x/2, point.y/2);
    return point;
}

@implementation FaceModel



- (void)setLandmarks:(NSArray<NSValue *> *)landmarks {
    CGPoint point106 = center([landmarks[98] CGPointValue], [landmarks[102] CGPointValue]);
    CGPoint point107 = center([landmarks[35] CGPointValue], [landmarks[65] CGPointValue]);
    CGPoint point108 = center([landmarks[40] CGPointValue], [landmarks[70] CGPointValue]);
    CGPoint point109 = center([landmarks[5] CGPointValue], [landmarks[80] CGPointValue]);
    CGPoint point110 = center([landmarks[27] CGPointValue], [landmarks[81] CGPointValue]);

    NSMutableArray *tmp = [NSMutableArray arrayWithArray:landmarks];
    [tmp addObject:[NSValue valueWithCGPoint:point106]];
    [tmp addObject:[NSValue valueWithCGPoint:point107]];
    [tmp addObject:[NSValue valueWithCGPoint:point108]];
    [tmp addObject:[NSValue valueWithCGPoint:point109]];
    [tmp addObject:[NSValue valueWithCGPoint:point110]];
    
    _landmarks = tmp;
}

@end

@interface FaceDetector()
{
    CGFloat frameWidth, frameHeight;
}
@property (nonatomic, strong) MGFacepp *markManager;
@property (copy, nonatomic) NSArray<FaceModel *> *faceModels;

@property (nonatomic, assign) double allTime;
@property (nonatomic, assign) NSInteger count;
@end
@implementation FaceDetector

static FaceDetector *share;
+ (instancetype)shareInstance {
    if (share == nil) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            share = [FaceDetector new];
            
        });
    }
    return share;
}

- (instancetype)init {
    if (self = [super init]) {
        [self setupFacepp];
    }
    return self;
}

- (void)setupFacepp {
    if (!self.isAuth) {
        return;
    }
    NSString *modelPath = [[NSBundle mainBundle] pathForResource:KMGFACEMODELNAME ofType:@""];
    NSData *modelData = [NSData dataWithContentsOfFile:modelPath];
    MGFacepp *markManager = [[MGFacepp alloc] initWithModel:modelData
                                               maxFaceCount:1
                                              faceppSetting:^(MGFaceppConfig *config) {
                                                  config.minFaceSize = 100;
                                                  config.interval = 40;
                                                  config.orientation = 90;
                                                  config.detectionMode = MGFppDetectionModeTrackingRobust;
                                                  config.pixelFormatType = PixelFormatTypeRGBA;
                                              }];
    self.markManager = markManager;

}

- (void)resetFacepp {
    if (!self.isAuth) {
        return;
    }
    int faceOrientation = self.faceOrientation;
    FaceDetectorSampleBufferOrientation sampleBufferOrientation = self.sampleBufferOrientation;
    [self setupFacepp];
    self.faceOrientation = faceOrientation;
    self.sampleBufferOrientation = sampleBufferOrientation;
}

- (void)getLandmarksFromSampleBuffer:(CMSampleBufferRef)detectSampleBufferRef {
    
    if (self.isAuth && !self.markManager) {
        [self resetFacepp];
    }
    
    MGImageData *imageData = [[MGImageData alloc] initWithSampleBuffer:detectSampleBufferRef];
    frameWidth = imageData.width;
    frameHeight = imageData.height;
    
    [self.markManager beginDetectionFrame];
    
    NSDate *date1, *date2;
    date1 = [NSDate date];
    
    NSArray *faceList = [self.markManager detectWithImageData:imageData];
    
    date2 = [NSDate date];
    double timeUsed = [date2 timeIntervalSinceDate:date1] * 1000;
    
    _allTime += timeUsed;
    _count ++;
 
    NSMutableArray *tmpArr = [NSMutableArray array];
    for (int i = 0; i < faceList.count; i ++) {
        MGFaceInfo *faceInfo = faceList[i];
        [self.markManager GetGetLandmark:faceInfo isSmooth:YES pointsNumber:106];
        
        NSMutableArray *points = [NSMutableArray array];
        for (NSValue *v in faceInfo.points) {
            CGPoint point = [self transformPointToPortrait:v.CGPointValue];
            [points addObject:[NSValue valueWithCGPoint:point]];
        }
        
        BOOL isFront = self.sampleBufferOrientation == FaceDetectorSampleBufferOrientationCameraFrontAndHorizontallyMirror;
        
        NSInteger index = self.sampleBufferOrientation == FaceDetectorSampleBufferOrientationCameraBack ? 1 : self.sampleBufferOrientation - FaceDetectorSampleBufferOrientationNoRatation;
        FaceModel *model = [FaceModel new];
        model.bounds = faceInfo.rect;
        model.landmarks = points;
        model.pitchAngle = -faceInfo.pitch;
        model.yawAngle = isFront ? -faceInfo.yaw : faceInfo.yaw;
        model.rollAngle = isFront  ? -(M_PI/2 - faceInfo.roll) : (M_PI/2 * index - faceInfo.roll);
        [tmpArr addObject:model];
 
    }
    [self.markManager endDetectionFrame];
    
    self.faceModels = tmpArr;
}

- (CGPoint)transformPointToPortrait:(CGPoint)point {
    
    CGFloat width = frameWidth;
    CGFloat height = frameHeight;
    switch (self.sampleBufferOrientation) {
        case FaceDetectorSampleBufferOrientationCameraFrontAndHorizontallyMirror:
            return CGPointMake(point.y/height, point.x/width);
            
        case FaceDetectorSampleBufferOrientationCameraBack:
            return CGPointMake(1 - point.y/height, point.x/width);
            
        case FaceDetectorSampleBufferOrientationRatation90:
            return CGPointMake(1 - point.y/height, point.x/width);
            
        case FaceDetectorSampleBufferOrientationRatation180:
            return CGPointMake(1 - point.x/width, 1 - point.y/height);
            
        case FaceDetectorSampleBufferOrientationRatation270:
            return CGPointMake(point.y/height, 1 - point.x/width);
            
        case FaceDetectorSampleBufferOrientationNoRatation:
            return CGPointMake(point.x/width, point.y/height);
    }
}

- (FaceModel *)oneFace {
    return self.faceModels.firstObject;
}

- (int)orientation {
    return [self.markManager getFaceppConfig].orientation;
}

- (void)setFaceOrientation:(int)faceOrientation {
    [self.markManager updateFaceppSetting:^(MGFaceppConfig *config) {
        config.orientation = faceOrientation;
    }];
}

- (BOOL)isWorking {
    return self.markManager.status == MGMarkWorking;
}

- (void)setSampleType:(FaceDetectorSampleType)sampleType {
    if (_sampleType != FaceDetectorSampleTypeNone && _sampleType != sampleType) {
        [self resetFacepp];
    }
    _sampleType = sampleType;
}

- (void)auth {
    self.isAuth = false;
    [MGFaceLicenseHandle licenseForNetwokrFinish:^(bool License, NSDate *sdkDate){
        if (!License) {
            NSLog(@"联⽹网授权失败 !!!");
            self.isAuth = false;
        } else {
            NSLog(@"联⽹网授权成功");
            self.isAuth = true;
        }
    }];
}
@end
