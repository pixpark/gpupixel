//
//  NebulaMetalView.m
//  zp-sdk-avmodule
//
//  Created by caozijun on 2023/2/27.
//

#import "NebulaMetalView.h"
#import "NebulaShaderType.h"
#import <AVFoundation/AVFoundation.h>
#import <MetalKit/MetalKit.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#include <array>

// 颜色空间转换矩阵，BT.601 Video Range。
static const matrix_float3x3 NebulaColorMatrix601VideoRange = (matrix_float3x3){
    (simd_float3){1.164, 1.164, 1.164},
    (simd_float3){0.0, -0.392, 2.017},
    (simd_float3){1.596, -0.813, 0.0},
};

// 颜色空间转换矩阵，BT.601 Full Range。
static const matrix_float3x3 NebulaColorMatrix601FullRange = (matrix_float3x3){
    (simd_float3){1.0, 1.0, 1.0},
    (simd_float3){0.0, -0.343, 1.765},
    (simd_float3){1.4, -0.711, 0.0},
};

// 颜色空间转换矩阵，BT.709 Video Range。
static const matrix_float3x3 NebulaColorMatrix709VideoRange = (matrix_float3x3){
    (simd_float3){1.164, 1.164, 1.164},
    (simd_float3){0.0, -0.213, 2.112},
    (simd_float3){1.793, -0.533, 0.0},
};

// 颜色空间转换矩阵，BT.709 Full Range。
static const matrix_float3x3 NebulaColorMatrix709FullRange = (matrix_float3x3){
    (simd_float3){1.0, 1.0, 1.0},
    (simd_float3){0.0, -0.187, 1.856},
    (simd_float3){1.575, -0.468, 0.0},
};

@interface NebulaMetalView () <MTKViewDelegate>
@property(nonatomic, assign)
    CVPixelBufferRef pixelBuffer; // 外层输入的最后一帧数据。
@property(nonatomic, strong) dispatch_semaphore_t
    semaphore; // 处理 PixelBuffer 锁，防止外层输入线程与渲染线程同时操作
               // Crash。
@property(nonatomic, assign) CVMetalTextureCacheRef
    textureCache; // 纹理缓存，根据 pixelbuffer 获取纹理。
@property(nonatomic, strong) MTKView *mtkView;    // Metal 渲染的 view。
@property(nonatomic, assign) CGSize viewportSize; // 视口大小。
@property(nonatomic, strong) id<MTLRenderPipelineState>
    pipelineState; // 渲染管道，管理顶点函数和片元函数。
@property(nonatomic, strong) id<MTLCommandQueue> commandQueue; // 渲染指令队列。
@property(nonatomic, strong) id<MTLBuffer> vertices; // 顶点缓存对象。
@property(nonatomic, assign) NSUInteger numVertices; // 顶点数量。
@property(nonatomic, strong) id<MTLBuffer> yuvMatrix; // YUV 数据矩阵对象。
@property(nonatomic, assign) BOOL updateFillMode; // 填充模式变更标记。
@property(nonatomic, assign) CGSize pixelBufferSize; // pixelBuffer 数据尺寸。
@property(nonatomic, assign) CGSize currentViewSize; // 当前视图大小。
@property(nonatomic, strong) dispatch_queue_t renderQueue; // 渲染线程。
@end

@implementation NebulaMetalView
#pragma mark - LifeCycle
- (instancetype)initWithFrame:(CGRect)frame {
  self = [super initWithFrame:frame];
  if (self) {
    _currentViewSize = frame.size;
    _fillMode = NebulaMetalViewContentModeFit;
    _updateFillMode = YES;
    //  创建 Metal 渲染视图且添加到当前视图。
    self.mtkView = [[MTKView alloc] initWithFrame:self.bounds];
    self.mtkView.device = MTLCreateSystemDefaultDevice();
    [self addSubview:self.mtkView];
    self.mtkView.delegate = self;
    self.mtkView.framebufferOnly = YES;
    self.viewportSize = self.mtkView.drawableSize;

    // 创建渲染线程。
    _semaphore = dispatch_semaphore_create(1);
    _renderQueue = dispatch_queue_create(
        "com.KeyFrameKit.metalView.renderQueue", DISPATCH_QUEUE_SERIAL);

    // 创建纹理缓存。
    CVMetalTextureCacheCreate(NULL, NULL, self.mtkView.device, NULL,
                              &_textureCache);
  }

  return self;
}

//- (void)layoutSubviews {
//  // 视图自动调整布局，同步至 Metal 视图。
//  [super layoutSubviews];
//  self.mtkView.frame = self.bounds;
//  _currentViewSize = self.bounds.size;
//}

- (void)dealloc {
  // 释放最后一帧数据、纹理缓存。
  dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
  if (_pixelBuffer) {
    CFRelease(_pixelBuffer);
    _pixelBuffer = NULL;
  }

  if (_textureCache) {
    CVMetalTextureCacheFlush(_textureCache, 0);
    CFRelease(_textureCache);
    _textureCache = NULL;
  }
  dispatch_semaphore_signal(_semaphore);
  [self.mtkView releaseDrawables];
}

#pragma mark - Public Method
- (void)renderPixelBuffer:(CVPixelBufferRef)pixelBuffer {
  if (!pixelBuffer) {
    return;
  }
  // 外层输入 BGRA、YUV 数据。
  dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
  if (_pixelBuffer) {
    CFRelease(_pixelBuffer);
    _pixelBuffer = NULL;
  }
  _pixelBuffer = pixelBuffer;
  _pixelBufferSize = CGSizeMake(CVPixelBufferGetWidth(pixelBuffer),
                                CVPixelBufferGetHeight(pixelBuffer));
  CFRetain(pixelBuffer);
  dispatch_semaphore_signal(_semaphore);
}

- (void)setFillMode:(NebulaMetalViewContentMode)fillMode {
  // 更改视图填充模式。
  _fillMode = fillMode;
  _updateFillMode = YES;
}

- (void)setRotation:(NebulaMetalViewRotation)rotation {
  _rotation = rotation;
  _updateFillMode = YES;
}

- (void)setMirror:(BOOL)mirror {
  _mirror = mirror;
  _updateFillMode = YES;
}

#pragma mark - Private Method
- (void)_setupPipeline:(OSType)type {
  // 根据本地 shader 文件初始化渲染管道与渲染指令队列。
  NSBundle *sdkMainBundle = [NSBundle bundleForClass:[self class]];
  id<MTLLibrary> defaultLibrary = [self.mtkView.device newDefaultLibraryWithBundle:sdkMainBundle error:nil];
  id<MTLFunction> vertexFunction =
      [defaultLibrary newFunctionWithName:@"vertexShader"];
  id<MTLFunction> fragmentFunction;
  if (type == kCVPixelFormatType_420YpCbCr8Planar || type == kCVPixelFormatType_420YpCbCr8PlanarFullRange){
    fragmentFunction = [defaultLibrary newFunctionWithName: @"yuvSamplingShader"];
  } else if (type == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange || type == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
    fragmentFunction = [defaultLibrary newFunctionWithName: @"nv12SamplingShader"];
  } else{
    fragmentFunction = [defaultLibrary newFunctionWithName: @"rgbSamplingShader"];
  }

  MTLRenderPipelineDescriptor *pipelineStateDescriptor =
      [[MTLRenderPipelineDescriptor alloc] init];
  pipelineStateDescriptor.vertexFunction = vertexFunction;
  pipelineStateDescriptor.fragmentFunction = fragmentFunction;
  pipelineStateDescriptor.colorAttachments[0].pixelFormat =
      self.mtkView.colorPixelFormat;
  self.pipelineState = [self.mtkView.device
      newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                     error:NULL];
  self.commandQueue = [self.mtkView.device newCommandQueue];
}

- (void)_setupYUVMatrix:(BOOL)isFullRange colorSpace:(CFTypeRef)colorSpace {
  // 初始化 YUV 矩阵，判断 pixelBuffer 的颜色格式是 601 还是
  // 709，创建对应的矩阵。
  NebulaConvertMatrix matrix;
  if (colorSpace) {
    if (CFEqual(colorSpace, kCVImageBufferYCbCrMatrix_ITU_R_601_4)) {
      matrix.matrix = isFullRange ? NebulaColorMatrix601FullRange
                                  : NebulaColorMatrix601VideoRange;
    } else if (CFEqual(colorSpace,kCVImageBufferYCbCrMatrix_ITU_R_709_2)) {
      matrix.matrix = isFullRange ? NebulaColorMatrix709FullRange
                                  : NebulaColorMatrix709VideoRange;
    }
  }else{
    matrix.matrix = isFullRange ? NebulaColorMatrix601FullRange
                                : NebulaColorMatrix601VideoRange;
  }
  matrix.fullRange = isFullRange;
  self.yuvMatrix =
      [self.mtkView.device newBufferWithBytes:&matrix
                                       length:sizeof(NebulaConvertMatrix)
                                      options:MTLResourceStorageModeShared];
}

- (void)_updaterVertices {
  // 根据填充模式计算顶点数据。
  float heightScaling = 1.0;
  float widthScaling = 1.0;

  if (!CGSizeEqualToSize(_currentViewSize, CGSizeZero) &&
      !CGSizeEqualToSize(_pixelBufferSize, CGSizeZero)) {
    CGRect insetRect = AVMakeRectWithAspectRatioInsideRect(
        _pixelBufferSize,
        CGRectMake(0, 0, _currentViewSize.width, _currentViewSize.height));

    switch (_fillMode) {
    case NebulaMetalViewContentModeStretch: {
      widthScaling = 1.0;
      heightScaling = 1.0;
      break;
    }
    case NebulaMetalViewContentModeFit: {
      widthScaling = insetRect.size.width / _currentViewSize.width;
      heightScaling = insetRect.size.height / _currentViewSize.height;
      break;
    }
    case NebulaMetalViewContentModeFill: {
      widthScaling = _currentViewSize.height / insetRect.size.height;
      heightScaling = _currentViewSize.width / insetRect.size.width;
      break;
    }
    }
  }

  std::array<std::array<GLfloat, 2>, 4> UVCoords = {{
      {{0, 1}}, // Lower left.
      {{1, 1}}, // Lower right.
      {{1, 0}}, // Upper right.
      {{0, 0}}, // Upper left.
  }};

  int rotation_offset;
  switch (_rotation) {
  case NebulaMetalViewRotation0:
    rotation_offset = 0;
    break;
  case NebulaMetalViewRotation90:
    rotation_offset = 1;
    break;
  case NebulaMetalViewRotation180:
    rotation_offset = 2;
    break;
  case NebulaMetalViewRotation270:
    rotation_offset = 3;
    break;
  }
  std::rotate(UVCoords.begin(), UVCoords.begin() + rotation_offset,
              UVCoords.end());

  NebulaVertex quadVertices[] = {
      {{-widthScaling, -heightScaling, 0.0, 1.0},
       {UVCoords[0][0], UVCoords[0][1]}},
      {{widthScaling, -heightScaling, 0.0, 1.0},
       {UVCoords[1][0], UVCoords[1][1]}},
      {{-widthScaling, heightScaling, 0.0, 1.0},
       {UVCoords[3][0], UVCoords[3][1]}},
      {{widthScaling, heightScaling, 0.0, 1.0},
       {UVCoords[2][0], UVCoords[2][1]}},
  };

  // mirror
  NebulaVertex quadMirrorVertices[] = {
      {{-widthScaling, -heightScaling, 0.0, 1.0},
       {UVCoords[1][0], UVCoords[1][1]}},
      {{widthScaling, -heightScaling, 0.0, 1.0},
       {UVCoords[0][0], UVCoords[0][1]}},
      {{-widthScaling, heightScaling, 0.0, 1.0},
       {UVCoords[2][0], UVCoords[2][1]}},
      {{widthScaling, heightScaling, 0.0, 1.0},
       {UVCoords[3][0], UVCoords[3][1]}},
  };
  // MTLResourceStorageModeShared
  // 属性可共享的，表示可以被顶点或者片元函数或者其他函数使用。
  if (!_mirror) {
    self.vertices =
        [self.mtkView.device newBufferWithBytes:quadVertices
                                         length:sizeof(quadVertices)
                                        options:MTLResourceStorageModeShared];
    self.numVertices = sizeof(quadVertices) / sizeof(NebulaVertex);
  } else {
    self.vertices =
        [self.mtkView.device newBufferWithBytes:quadMirrorVertices
                                         length:sizeof(quadMirrorVertices)
                                        options:MTLResourceStorageModeShared];
    self.numVertices = sizeof(quadVertices) / sizeof(NebulaVertex);
  }
}

- (BOOL)_pixelBufferIsFullRange:(CVPixelBufferRef)pixelBuffer {
  // 判断 YUV 数据是否为 full range。
  if (@available(iOS 15, *)) {
      CFDictionaryRef cfDicAttributes = CVPixelBufferCopyCreationAttributes(pixelBuffer);
      NSDictionary *dicAttributes = (__bridge_transfer NSDictionary*)cfDicAttributes;
      if (dicAttributes && [dicAttributes objectForKey:@"PixelFormatDescription"]) {
          NSDictionary *pixelFormatDescription = [dicAttributes objectForKey:@"PixelFormatDescription"];
          if (pixelFormatDescription && [pixelFormatDescription objectForKey:(__bridge NSString*)kCVPixelFormatComponentRange]) {
              NSString *componentRange = [pixelFormatDescription objectForKey:(__bridge NSString *)kCVPixelFormatComponentRange];
              return [componentRange isEqualToString:(__bridge NSString *)kCVPixelFormatComponentRange_FullRange];
          }
      }
  } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
      OSType formatType = CVPixelBufferGetPixelFormatType(pixelBuffer);
      return formatType == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
#pragma clang diagnostic pop
  }
  
  return NO;
}

- (void)_drawInMTKView:(MTKView *)view {
  // 渲染数据。
  dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
  if (_pixelBuffer) {
    // 为当前渲染的每个渲染传递创建一个新的命令缓冲区。
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    // 获取渲染命令编码器 MTLRenderCommandEncoder 的描述符。
    // currentRenderPassDescriptor 描述符包含 currentDrawable
    // 的纹理、视图的深度、模板和 sample 缓冲区和清晰的值。
    // MTLRenderPassDescriptor 描述一系列 attachments 的值，类似 OpenGL 的
    // FrameBuffer；同时也用来创建 MTLRenderCommandEncoder。
    MTLRenderPassDescriptor *renderPassDescriptor =
        view.currentRenderPassDescriptor;
    if (renderPassDescriptor) {
      // 根据描述创建 x 渲染命令编码器。
      id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer
          renderCommandEncoderWithDescriptor:renderPassDescriptor];
      // 设置绘制区域。
      [renderEncoder
          setViewport:(MTLViewport){0.0, 0.0, self.viewportSize.width,
                                    self.viewportSize.height, -1.0, 1.0}];
      OSType type =  CVPixelBufferGetPixelFormatType(_pixelBuffer);

      // 根据是否为 YUV 初始化渲染管道。
      if (!self.pipelineState) {
        [self _setupPipeline:type];
      }
      // 设置渲染管道。
      [renderEncoder setRenderPipelineState:self.pipelineState];

      // 更新填充模式。
      if (_updateFillMode) {
        [self _updaterVertices];
        _updateFillMode = NO;
      }
      // 传递顶点缓存。
      [renderEncoder setVertexBuffer:self.vertices
                              offset:0
                             atIndex:NebulaVertexInputIndexVertices];
      CVPixelBufferRef pixelBuffer = _pixelBuffer;

      if (type == kCVPixelFormatType_420YpCbCr8Planar || type == kCVPixelFormatType_420YpCbCr8PlanarFullRange) {
        // 获取 y、uv 纹理。
        id<MTLTexture> textureY = nil;
        id<MTLTexture> textureU = nil;
        id<MTLTexture> textureV = nil;
        {
          size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
          size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
          MTLPixelFormat pixelFormat = MTLPixelFormatR8Unorm;

          CVMetalTextureRef texture = NULL;
          CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
              kCFAllocatorDefault, _textureCache, pixelBuffer, NULL,
              pixelFormat, width, height, 0, &texture);
          if (status == kCVReturnSuccess) {
            textureY = CVMetalTextureGetTexture(texture);
            CFRelease(texture);
          }
        }

        {
          size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
          size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
          MTLPixelFormat pixelFormat = MTLPixelFormatR8Unorm;

          CVMetalTextureRef texture = NULL;
          CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
              kCFAllocatorDefault, _textureCache, pixelBuffer, NULL,
              pixelFormat, width, height, 1, &texture);
          if (status == kCVReturnSuccess) {
            textureU = CVMetalTextureGetTexture(texture);
            CFRelease(texture);
          }
        }
        {
          size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 2);
          size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 2);
          MTLPixelFormat pixelFormat = MTLPixelFormatR8Unorm;

          CVMetalTextureRef texture = NULL;
          CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
              kCFAllocatorDefault, _textureCache, pixelBuffer, NULL,
              pixelFormat, width, height, 2, &texture);
          if (status == kCVReturnSuccess) {
            textureV = CVMetalTextureGetTexture(texture);
            CFRelease(texture);
          }
        }
        // 传递纹理。
        if (textureY != nil && textureU != nil && textureV != nil) {
          [renderEncoder
              setFragmentTexture:textureY
                         atIndex:NebulaI420FragmentTextureIndexTextureY];
          [renderEncoder
              setFragmentTexture:textureU
                         atIndex:NebulaI420FragmentTextureIndexTextureU];
          [renderEncoder
              setFragmentTexture:textureV
                         atIndex:NebulaI420FragmentTextureIndexTextureV];
        }

        // 初始化 YUV 矩阵。
        if (!self.yuvMatrix) {
          CFTypeRef matrixKey;
          if (@available(iOS 15, *)) {
            matrixKey = CVBufferCopyAttachment(
                                              pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
          }else{
            matrixKey = CVBufferGetAttachment(
                                              pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
          }
          [self _setupYUVMatrix:[self _pixelBufferIsFullRange:pixelBuffer]
                     colorSpace:matrixKey];
          if (matrixKey) {
            CFRelease(matrixKey);
          }
        }
        // 传递 YUV 矩阵。
        [renderEncoder setFragmentBuffer:self.yuvMatrix
                                  offset:0
                                 atIndex:NebulaFragmentInputIndexMatrix];
      }else if(type == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange || type == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange){
        // 获取 y、uv 纹理。
        id<MTLTexture> textureY = nil;
        id<MTLTexture> textureUV = nil;
        {
          size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 0);
          size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 0);
          MTLPixelFormat pixelFormat = MTLPixelFormatR8Unorm;

          CVMetalTextureRef texture = NULL;
          CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
             kCFAllocatorDefault, _textureCache, pixelBuffer, NULL,
             pixelFormat, width, height, 0, &texture);
          if (status == kCVReturnSuccess) {
           textureY = CVMetalTextureGetTexture(texture);
           CFRelease(texture);
          }
        }

        {
          size_t width = CVPixelBufferGetWidthOfPlane(pixelBuffer, 1);
          size_t height = CVPixelBufferGetHeightOfPlane(pixelBuffer, 1);
          MTLPixelFormat pixelFormat = MTLPixelFormatRG8Unorm;

          CVMetalTextureRef texture = NULL;
          CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
             kCFAllocatorDefault, _textureCache, pixelBuffer, NULL,
             pixelFormat, width, height, 1, &texture);
          if (status == kCVReturnSuccess) {
           textureUV = CVMetalTextureGetTexture(texture);
           CFRelease(texture);
          }
        }
        // 传递纹理。
        if (textureY != nil && textureUV != nil) {
          [renderEncoder
              setFragmentTexture:textureY
                         atIndex:NebulaNV12FragmentTextureIndexTextureY];
          [renderEncoder
              setFragmentTexture:textureUV
                         atIndex:NebulaNV12FragmentTextureIndexTextureUV];
        }

        // 初始化 YUV 矩阵。
        if (!self.yuvMatrix) {
          CFTypeRef matrixKey;
          if (@available(iOS 15, *)) {
            matrixKey = CVBufferCopyAttachment(
                                              pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
          }else{
            matrixKey = CVBufferGetAttachment(
                                              pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
          }
          [self _setupYUVMatrix:[self _pixelBufferIsFullRange:pixelBuffer]
                     colorSpace:matrixKey];
          if (matrixKey) {
            CFRelease(matrixKey);
          }
        }
        // 传递 YUV 矩阵。
        [renderEncoder setFragmentBuffer:self.yuvMatrix
                                  offset:0
                                 atIndex:NebulaFragmentInputIndexMatrix];
      } else {
        // 生成 rgba 纹理。
        id<MTLTexture> textureRGB = nil;
        size_t width = CVPixelBufferGetWidth(pixelBuffer);
        size_t height = CVPixelBufferGetHeight(pixelBuffer);
        MTLPixelFormat pixelFormat = MTLPixelFormatBGRA8Unorm;

        CVMetalTextureRef texture = NULL;
        CVReturn status = CVMetalTextureCacheCreateTextureFromImage(
            kCFAllocatorDefault, _textureCache, pixelBuffer, NULL, pixelFormat,
            width, height, 0, &texture);
        if (status == kCVReturnSuccess) {
          textureRGB = CVMetalTextureGetTexture(texture);
          CFRelease(texture);
        }

        // 传递纹理。
        if (textureRGB) {
          [renderEncoder
              setFragmentTexture:textureRGB
                         atIndex:NebulaFragmentTextureIndexTextureRGB];
        }
      }

      // 绘制。
      [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip
                        vertexStart:0
                        vertexCount:self.numVertices];

      // 命令结束。
      [renderEncoder endEncoding];

      // 显示。
      [commandBuffer presentDrawable:view.currentDrawable];

      // 提交。
      [commandBuffer commit];
    }

    CFRelease(_pixelBuffer);
    _pixelBuffer = NULL;
  }
  dispatch_semaphore_signal(_semaphore);
}

#pragma mark - MTKViewDelegate
- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  self.viewportSize = size;
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  // Metal 视图回调，有数据情况下渲染视图。
  __weak typeof(self) weakSelf = self;
  dispatch_async(_renderQueue, ^{
    [weakSelf _drawInMTKView:view];
  });
}

@end
