//
//  RecordVideoManage.h
//  GPUPixelApp
//
//  Created by PixPark. on 26/5/2025.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
NS_ASSUME_NONNULL_BEGIN

@interface RecordVideoManage : NSObject
#pragma mark - ================== 初始化单例 start ================
/// 获取单例对象
+ (instancetype)share;
/// 销毁单例
+ (void)destroySingleton;
/// 封锁init方法,不允许外界通过init进行初始化
- (instancetype)init NS_UNAVAILABLE;
/// 封锁new方法,不允许外界通过init进行初始化
+ (instancetype)new NS_UNAVAILABLE;
#pragma mark ================== 初始化单例 end ==================
/// 开始录制视频
/// - Parameters:
///   - videoSize: 视频长宽(务必和CVPixelBufferRef长宽一致, 否则录制视频将会失败)
///   - fps: 视频帧率
- (void)startRecordVideo:(CGSize)videoSize fps:(int32_t)fps;
/// 添加插入 CVPixelBufferRef像素帧流
/// - Parameter pixelBuffer: CVPixelBufferRef像素帧流
- (void)addPixelBuffer:(CVPixelBufferRef)pixelBuffer;
/// 停止录制, 并封装成MP4等视频格式
- (void)stopRecordVideo;
@end

NS_ASSUME_NONNULL_END
