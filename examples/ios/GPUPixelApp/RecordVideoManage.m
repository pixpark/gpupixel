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

/// 当前帧Index下标
@property (assign, nonatomic) int64_t frameIndex;
/// 是否是视频首帧
@property (assign, nonatomic) BOOL isLockThread;
/// 视频帧率
@property (assign, nonatomic) int32_t fps;

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
    
    singleton = nil;
    onceToken = 0L;
}
#pragma mark ================== 初始化单例 end ==================
#define kOutputRecordVideoPath [NSTemporaryDirectory() stringByAppendingPathComponent:@"output.mp4"]
/// 开始录制视频
/// - Parameters:
///   - frameSize: 视频帧长宽(务必和CVPixelBufferRef长宽一致, 否则录制视频将会失败)
///   - fps: 视频帧率
- (void)startRecordVideo:(CGSize)frameSize fps:(int32_t)fps {
    if (fps < 1) {
        fps = 1;
    }
    self.fps = fps;
    
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
        AVVideoWidthKey: @(frameSize.width),
        AVVideoHeightKey: @(frameSize.height),
    };
    self.writerInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
    self.writerInput.expectsMediaDataInRealTime = YES;
    
    if (self.adaptor != nil) {
        self.adaptor = nil;
    }
    NSDictionary *attributes = @{
        (NSString *)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
        (NSString *)kCVPixelBufferWidthKey: @(frameSize.width),
        (NSString *)kCVPixelBufferHeightKey: @(frameSize.height),
        (NSString *)kCVPixelFormatOpenGLESCompatibility: @(YES)
    };
    self.adaptor = [[AVAssetWriterInputPixelBufferAdaptor alloc] initWithAssetWriterInput:self.writerInput sourcePixelBufferAttributes:attributes];
    
    if ([self.assetWriter canAddInput:self.writerInput]) {
        [self.assetWriter addInput:self.writerInput];
    }
    [self.assetWriter startWriting];
    [self.assetWriter startSessionAtSourceTime:kCMTimeZero];
    self.frameIndex = 0;
}

/// 添加插入 CVPixelBufferRef像素帧流
/// - Parameter pixelBuffer: CVPixelBufferRef像素帧流
- (void)addPixelBuffer:(CVPixelBufferRef)pixelBuffer {
    if (pixelBuffer == nil) {
        return;
    }
    
    if (self.isLockThread == YES) {
        return;
    }
    self.isLockThread = YES;
    //创建子线程队列
    dispatch_async(dispatch_queue_create(NSStringFromClass(self.class).UTF8String, DISPATCH_QUEUE_SERIAL), ^{
        CMTime frameTime = CMTimeMake(self.frameIndex, self.fps);
        BOOL result = [self.adaptor appendPixelBuffer:pixelBuffer withPresentationTime:frameTime];
        if (result == true) {
            self.frameIndex = self.frameIndex + 1;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            self.isLockThread = NO;
        });
    });
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

@end
