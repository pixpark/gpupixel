//
//  RecordVideoManage.m
//  GPUPixelApp
//
//  Created by PixPark. on 26/5/2025.
//

#import "RecordVideoManage.h"

@interface RecordVideoManage ()
/// 媒体资源写入器
@property (strong, nonatomic) AVAssetWriter* assetWriter;
/// 媒体资源输入器
@property (strong, nonatomic) AVAssetWriterInput* writerInput;
/// 媒体资源适配编码器
@property (strong, nonatomic) AVAssetWriterInputPixelBufferAdaptor* adaptor;

/// 是否是视频首帧
@property (assign, nonatomic) BOOL isFirstFrame;
/// 当前帧Index下标
@property (assign, nonatomic) int64_t frameIndex;
/// 当前帧CVPixelBufferRef像素帧流
@property (assign, nonatomic) CVPixelBufferRef currentCVPixelBufferRef;

@end
@implementation RecordVideoManage
#pragma mark - ================== 初始化单例 start ================
static RecordVideoManage *singleton = nil;
static dispatch_once_t onceToken;
/// 获取单例对象
+ (instancetype)share {
    dispatch_once(&onceToken, ^{
        singleton = [[RecordVideoManage alloc] init];
    });
    return singleton;
}
/// 销毁单例
+ (void)destroySingleton {
    if (AVAssetWriterStatusWriting == singleton.assetWriter.status) {
        [singleton stopRecordVideo];
    }
    
    singleton.assetWriter = nil;
    singleton.writerInput = nil;
    singleton.adaptor = nil;
    singleton.currentCVPixelBufferRef = nil;
    
    singleton = nil;
    onceToken = 0L;
}
#pragma mark ================== 初始化单例 end ==================
#define kOutputRecordVideoPath [NSTemporaryDirectory() stringByAppendingPathComponent:@"output.mp4"]
/// 开始录制视频
/// - Parameters:
///   - videoSize: 视频长宽(务必和CVPixelBufferRef长宽一致, 否则录制视频将会失败)
///   - fps: 视频帧率
- (void)startRecordVideo:(CGSize)videoSize fps:(int32_t)fps {
    if (self.assetWriter != nil) {
        self.assetWriter = nil;
    }
    
    NSError *error = nil;
    NSString *outputFilePath = kOutputRecordVideoPath;
    NSFileHandle *hand = [NSFileHandle fileHandleForWritingAtPath:outputFilePath];
    if (hand != nil) {//如果存在,删除以前文件
        [[NSFileManager defaultManager] removeItemAtPath:outputFilePath error:nil];
    }
    NSURL *outputURL = [NSURL fileURLWithPath:outputFilePath];
    self.assetWriter = [[AVAssetWriter alloc] initWithURL:outputURL fileType:AVFileTypeMPEG4 error:&error];
    
    if (self.writerInput != nil) {
        self.writerInput = nil;
    }
    NSDictionary *videoSettings = @{
        AVVideoCodecKey: AVVideoCodecTypeH264,
        AVVideoWidthKey: @(videoSize.width),
        AVVideoHeightKey: @(videoSize.height),
    };
    self.writerInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
    self.writerInput.expectsMediaDataInRealTime = YES;
    
    if (self.adaptor != nil) {
        self.adaptor = nil;
    }
    NSDictionary *attributes = @{
        (NSString *)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
        (NSString *)kCVPixelBufferWidthKey: @(videoSize.width),
        (NSString *)kCVPixelBufferHeightKey: @(videoSize.height),
        (NSString *)kCVPixelFormatOpenGLESCompatibility: @(YES)
    };
    self.adaptor = [[AVAssetWriterInputPixelBufferAdaptor alloc] initWithAssetWriterInput:self.writerInput sourcePixelBufferAttributes:attributes];
    
    if ([self.assetWriter canAddInput:self.writerInput]) {
        [self.assetWriter addInput:self.writerInput];
    }
    [self.assetWriter startWriting];
    [self.assetWriter startSessionAtSourceTime:kCMTimeZero];
    self.frameIndex = 0;
    self.currentCVPixelBufferRef = nil;
    
    dispatch_queue_t writerInputQueue = dispatch_queue_create(NSStringFromClass(self.class).UTF8String, NULL);
    [self.writerInput requestMediaDataWhenReadyOnQueue:writerInputQueue usingBlock:^{
        int64_t frameIndex = self.frameIndex;
        while ([self.writerInput isReadyForMoreMediaData]) {
            if (self.isFirstFrame != YES && frameIndex == self.frameIndex) {
                continue;
            }
            
            frameIndex = self.frameIndex;
            CMTime frameTime = CMTimeMake(frameIndex, fps);
            if (self.currentCVPixelBufferRef == nil) {
                continue;
            }
            CVPixelBufferRef buffer = self.currentCVPixelBufferRef;
            BOOL result = [self.adaptor appendPixelBuffer:buffer withPresentationTime:frameTime];
            if (result == NO) {
                NSLog(@"Failed to append pixel buffer");
                continue;
            }
            if (self.isFirstFrame == true) {
                self.frameIndex = self.frameIndex + 1;
            }
        }
    }];
}

/// 添加插入 CVPixelBufferRef像素帧流
/// - Parameter pixelBuffer: CVPixelBufferRef像素帧流
- (void)addPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    self.currentCVPixelBufferRef = pixelBuffer;
    if (self.frameIndex != 0) {
        self.frameIndex = self.frameIndex + 1;
    }
}

/// 停止录制, 并封装成MP4等视频格式
- (void)stopRecordVideo {
    if (self.writerInput) {
        [self.writerInput markAsFinished];
    }
    if (self.assetWriter) {
        [self.assetWriter finishWritingWithCompletionHandler:^{
            NSLog(@"Asset Writer Completed Writing || Record Video Path : %@ ", kOutputRecordVideoPath);
        }];
        self.frameIndex = 0;
    }
}

/// 是否为首帧
- (BOOL)isFirstFrame {
    BOOL isFirstFrame = NO;
    //当前帧下标为0, 且当前帧非空, 即当前帧为首帧
    if (self.frameIndex == 0 && self.currentCVPixelBufferRef != nil) {
        isFirstFrame = true;
    }
    
    return isFirstFrame;
}

@end
