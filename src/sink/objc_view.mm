/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "sink/objc_view.h"
#import <AVFoundation/AVFoundation.h>
#include "core/gpupixel_context.h"
#include "core/gpupixel_program.h"
#include "utils/util.h"

using namespace gpupixel;

@interface ObjcView () {
  std::shared_ptr<gpupixel::GPUPixelFramebuffer> inputFramebuffer;
  gpupixel::RotationMode inputRotation;
  uint32_t displayFramebuffer;
  uint32_t displayRenderbuffer;
  gpupixel::GPUPixelGLProgram* displayProgram;
  uint32_t positionAttribLocation;
  uint32_t texCoordAttribLocation;
  uint32_t colorMapUniformLocation;

  float displayVertices[8];
  GLint framebufferWidth, framebufferHeight;
  CGSize lastBoundsSize;

  float backgroundColorRed, backgroundColorGreen, backgroundColorBlue,
      backgroundColorAlpha;
}

@end

@implementation ObjcView

@synthesize sizeInPixels = _sizeInPixels;
#if defined(GPUPIXEL_IOS)
@synthesize currentlayer;
@synthesize currentFrame;
#endif
- (id)initWithFrame:(CGRect)frame {
  if (!(self = [super initWithFrame:frame])) {
    return nil;
  }

  [self commonInit];

  return self;
}

- (id)initWithCoder:(NSCoder*)coder {
  if (!(self = [super initWithCoder:coder])) {
    return nil;
  }

  [self commonInit];

  return self;
}

- (void)commonInit;
{
  inputRotation = gpupixel::NoRotation;
#if defined(GPUPIXEL_IOS)
  self.opaque = YES;
  self.hidden = NO;
  CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
  eaglLayer.opaque = YES;
  eaglLayer.drawableProperties = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                   kEAGLDrawablePropertyRetainedBacking,
                                   kEAGLColorFormatRGBA8,
                                   kEAGLDrawablePropertyColorFormat, nil];
  currentlayer = (CAEAGLLayer*)self.layer;
  currentlayer.opaque = YES;
  currentlayer.drawableProperties = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO],
                                   kEAGLDrawablePropertyRetainedBacking,
                                   kEAGLColorFormatRGBA8,
                                   kEAGLDrawablePropertyColorFormat, nil];
#else
  [self setOpenGLContext:gpupixel::GPUPixelContext::GetInstance()
                             ->GetOpenGLContext()];
  if ([self
          respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:)]) {
    [self setWantsBestResolutionOpenGLSurface:YES];
  }
  //    inputRotation = kGPUImageNoRotation;
  self.hidden = NO;
#endif
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    displayProgram = gpupixel::GPUPixelGLProgram::CreateWithShaderString(
        gpupixel::kDefaultVertexShader, gpupixel::kDefaultFragmentShader);

    positionAttribLocation = displayProgram->GetAttribLocation("position");
    texCoordAttribLocation =
        displayProgram->GetAttribLocation("inputTextureCoordinate");
    colorMapUniformLocation =
        displayProgram->GetUniformLocation("inputImageTexture");

    gpupixel::GPUPixelContext::GetInstance()->SetActiveGlProgram(
        displayProgram);
    glEnableVertexAttribArray(positionAttribLocation);
    glEnableVertexAttribArray(texCoordAttribLocation);

    [self setBackgroundColorRed:0.0 green:0.0 blue:0.0 alpha:0.0];
    _fillMode = gpupixel::SinkRender::FillMode::PreserveAspectRatio;
    [self createDisplayFramebuffer];
  });
}

#if defined(GPUPIXEL_IOS)
- (void)layoutSubviews {
  [super layoutSubviews];
  if (!CGSizeEqualToSize(self.bounds.size, lastBoundsSize) &&
      !CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
    self.currentFrame = self.bounds;

    [self destroyDisplayFramebuffer];
    [self createDisplayFramebuffer];
  }
}

+ (Class)layerClass {
  return [CAEAGLLayer class];
}
#else
- (void)reshape {
  [super reshape];
  CGSize viewSize = self.bounds.size;
  if ([self respondsToSelector:@selector(convertSizeToBacking:)]) {
    viewSize = [self convertSizeToBacking:self.bounds.size];
  }

  if ((_sizeInPixels.width == viewSize.width) &&
      (_sizeInPixels.height == viewSize.height)) {
    return;
  }

  _sizeInPixels = viewSize;

  [self updateDisplayVertices];
}

#endif

- (void)dealloc {
  [self destroyDisplayFramebuffer];
}

- (void)createDisplayFramebuffer;
{
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
#if defined(GPUPIXEL_IOS)
    glGenRenderbuffers(1, &displayRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
    [gpupixel::GPUPixelContext::GetInstance()->GetEglContext()
        renderbufferStorage:GL_RENDERBUFFER
               fromDrawable:(CAEAGLLayer*)currentlayer];
    lastBoundsSize = currentFrame.size;

    glGenFramebuffers(1, &displayFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                              GL_RENDERBUFFER, displayRenderbuffer);

    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,
                                 &framebufferWidth);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT,
                                 &framebufferHeight);

    [self updateDisplayVertices];
#else
    // Perhaps I'll use an FBO at some time later, but for now will render
    // directly to the screen
    if ([self respondsToSelector:@selector(convertSizeToBacking:)]) {
      _sizeInPixels = [self convertSizeToBacking:self.bounds.size];
    } else {
      _sizeInPixels = self.bounds.size;
    }
#endif
  });
}

- (void)destroyDisplayFramebuffer;
{
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
#if defined(GPUPIXEL_IOS)
    if (displayFramebuffer) {
      glDeleteFramebuffers(1, &displayFramebuffer);
      displayFramebuffer = 0;
    }

    if (displayRenderbuffer) {
      glDeleteRenderbuffers(1, &displayRenderbuffer);
      displayRenderbuffer = 0;
    }
#else
    [self.openGLContext makeCurrentContext];
#endif
  });
}

- (void)setDisplayFramebuffer;
{
#if defined(GPUPIXEL_IOS)
  if (!displayFramebuffer) {
    [self createDisplayFramebuffer];
  }

  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
      glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
      glViewport(0, 0, framebufferWidth, framebufferHeight);
  });
#else
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glViewport(0, 0, self.sizeInPixels.width, self.sizeInPixels.height);
  });
#endif
}

- (void)presentFramebuffer {
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
#if defined(GPUPIXEL_IOS)
    glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
    gpupixel::GPUPixelContext::GetInstance()->PresentBufferForDisplay();
#else
    [self.openGLContext flushBuffer];
#endif
  });
}

- (void)setBackgroundColorRed:(float)redComponent
                        green:(float)greenComponent
                         blue:(float)blueComponent
                        alpha:(float)alphaComponent;
{
  backgroundColorRed = redComponent;
  backgroundColorGreen = greenComponent;
  backgroundColorBlue = blueComponent;
  backgroundColorAlpha = alphaComponent;
}

- (void)DoRender {
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    gpupixel::GPUPixelContext::GetInstance()->SetActiveGlProgram(
        displayProgram);
    [self setDisplayFramebuffer];
    glClearColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue,
                 backgroundColorAlpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if defined(GPUPIXEL_MAC)
    // Re-render onscreen, flipped to a normal orientation
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
#endif
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, inputFramebuffer->GetTexture()));
    GL_CALL(glUniform1i(colorMapUniformLocation, 0));

    GL_CALL(glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, 0, 0,
                                  displayVertices));
    GL_CALL(glVertexAttribPointer(
        texCoordAttribLocation, 2, GL_FLOAT, 0, 0,
        [self textureCoordinatesForRotation:inputRotation]));
#if defined(GPUPIXEL_IOS)
    GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    [self presentFramebuffer];
#else
    [[self openGLContext] makeCurrentContext];
    GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    [self presentFramebuffer];
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
#endif
  });
}

- (void)SetInputFramebuffer:
            (std::shared_ptr<gpupixel::GPUPixelFramebuffer>)newInputFramebuffer
               withRotation:(gpupixel::RotationMode)rotation
                    atIndex:(int)texIdx {
  std::shared_ptr<gpupixel::GPUPixelFramebuffer> lastFramebuffer =
      inputFramebuffer;
  gpupixel::RotationMode lastInputRotation = inputRotation;

  inputRotation = rotation;
  inputFramebuffer = newInputFramebuffer;

  if (lastFramebuffer != newInputFramebuffer && newInputFramebuffer &&
      (!lastFramebuffer ||
       !(lastFramebuffer->GetWidth() == newInputFramebuffer->GetWidth() &&
         lastFramebuffer->GetHeight() == newInputFramebuffer->GetHeight() &&
         lastInputRotation == rotation))) {
    [self updateDisplayVertices];
  }
}

- (void)setFillMode:(gpupixel::SinkRender::FillMode)newValue;
{
  if (_fillMode != newValue) {
    _fillMode = newValue;
    [self updateDisplayVertices];
  }
}

- (void)updateDisplayVertices;
{
  if (inputFramebuffer == 0) {
    return;
  }

  CGFloat scaledWidth = 1.0;
  CGFloat scaledHeight = 1.0;

  int rotatedFramebufferWidth = inputFramebuffer->GetWidth();
  int rotatedFramebufferHeight = inputFramebuffer->GetHeight();
  if (rotationSwapsSize(inputRotation)) {
    rotatedFramebufferWidth = inputFramebuffer->GetHeight();
    rotatedFramebufferHeight = inputFramebuffer->GetWidth();
  }

  CGRect insetRect = AVMakeRectWithAspectRatioInsideRect(
      CGSizeMake(rotatedFramebufferWidth, rotatedFramebufferHeight),
      self.bounds);

  if (_fillMode == gpupixel::SinkRender::FillMode::PreserveAspectRatio) {
    scaledWidth = insetRect.size.width / self.bounds.size.width;
    scaledHeight = insetRect.size.height / self.bounds.size.height;
  } else if (_fillMode ==
             gpupixel::SinkRender::FillMode::PreserveAspectRatioAndFill) {
    scaledWidth = self.bounds.size.height / insetRect.size.height;
    scaledHeight = self.bounds.size.width / insetRect.size.width;
  }

  displayVertices[0] = -scaledWidth;
  displayVertices[1] = -scaledHeight;
  displayVertices[2] = scaledWidth;
  displayVertices[3] = -scaledHeight;
  displayVertices[4] = -scaledWidth;
  displayVertices[5] = scaledHeight;
  displayVertices[6] = scaledWidth;
  displayVertices[7] = scaledHeight;
}

- (const float*)textureCoordinatesForRotation:
    (gpupixel::RotationMode)rotation_mode;
{
  static const float noRotationTextureCoordinates[] = {
      0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  };

  static const float rotateRightTextureCoordinates[] = {
      1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  };

  static const float rotateLeftTextureCoordinates[] = {
      0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
  };

  static const float verticalFlipTextureCoordinates[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  static const float horizontalFlipTextureCoordinates[] = {
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
  };

  static const float rotateRightVerticalFlipTextureCoordinates[] = {
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
  };

  static const float rotateRightHorizontalFlipTextureCoordinates[] = {
      0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
  };

  static const float rotate180TextureCoordinates[] = {
      1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };

  switch (inputRotation) {
    case gpupixel::NoRotation:
      return noRotationTextureCoordinates;
    case gpupixel::RotateLeft:
      return rotateLeftTextureCoordinates;
    case gpupixel::RotateRight:
      return rotateRightTextureCoordinates;
    case gpupixel::FlipVertical:
      return verticalFlipTextureCoordinates;
    case gpupixel::FlipHorizontal:
      return horizontalFlipTextureCoordinates;
    case gpupixel::RotateRightFlipVertical:
      return rotateRightVerticalFlipTextureCoordinates;
    case gpupixel::RotateRightFlipHorizontal:
      return rotateRightHorizontalFlipTextureCoordinates;
    case gpupixel::Rotate180:
      return rotate180TextureCoordinates;
  }
}

- (BOOL)IsReady {
  return (inputFramebuffer != nullptr);
}

- (void)unPrepared {
  inputFramebuffer = nullptr;
}

@end
