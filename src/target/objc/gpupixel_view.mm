/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "gpupixel_view.h"
#include "gpupixel_context.h"
#include "gl_program.h"
#include "filter.h"

#import <AVFoundation/AVFoundation.h>

@interface GPUPixelView()
{
    std::shared_ptr<GPUPixel::Framebuffer> inputFramebuffer;
    GPUPixel::RotationMode inputRotation;
    GLuint displayFramebuffer;
    GLuint displayRenderbuffer;
    GPUPixel::GLProgram* displayProgram;
    GLuint positionAttribLocation;
    GLuint texCoordAttribLocation;
    GLuint colorMapUniformLocation;
    
    GLfloat displayVertices[8];
    GLint framebufferWidth, framebufferHeight;
    CGSize lastBoundsSize;
    
    GLfloat backgroundColorRed, backgroundColorGreen, backgroundColorBlue, backgroundColorAlpha;
}

@end

@implementation GPUPixelView

@synthesize sizeInPixels = _sizeInPixels;

- (id)initWithFrame:(CGRect)frame
{
    if (!(self = [super initWithFrame:frame]))
    {
        return nil;
    }
    
    [self commonInit];
    
    return self;
}

-(id)initWithCoder:(NSCoder *)coder
{
    if (!(self = [super initWithCoder:coder]))
    {
        return nil;
    }
    
    [self commonInit];
    
    return self;
}

- (void)commonInit;
{
    inputRotation = GPUPixel::NoRotation;
#if defined(GPUPIXEL_IOS)
    self.opaque = YES;
    self.hidden = NO;
    CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
    
#else
    [self setOpenGLContext:GPUPixel::GPUPixelContext::getInstance()->getOpenGLContext()];
    if ([self respondsToSelector:@selector(setWantsBestResolutionOpenGLSurface:)])
    {
        [self  setWantsBestResolutionOpenGLSurface:YES];
    }
    //    inputRotation = kGPUImageNoRotation;
    self.hidden = NO;
#endif
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
        displayProgram = GPUPixel::GLProgram::createByShaderString(GPUPixel::kDefaultVertexShader, GPUPixel::kDefaultFragmentShader);
        
        positionAttribLocation = displayProgram->getAttribLocation("position");
        texCoordAttribLocation = displayProgram->getAttribLocation("inputTextureCoordinate");
        colorMapUniformLocation = displayProgram->getUniformLocation("inputImageTexture");
        
        GPUPixel::GPUPixelContext::getInstance()->setActiveShaderProgram(displayProgram);
        glEnableVertexAttribArray(positionAttribLocation);
        glEnableVertexAttribArray(texCoordAttribLocation);
        
        [self setBackgroundColorRed:0.0 green:0.0 blue:0.0 alpha:0.0];
        _fillMode = GPUPixel::TargetView::FillMode::PreserveAspectRatio;
        [self createDisplayFramebuffer];
    });
}
 
#if defined(GPUPIXEL_IOS)
- (void)layoutSubviews {
    [super layoutSubviews];
    if (!CGSizeEqualToSize(self.bounds.size, lastBoundsSize) &&
        !CGSizeEqualToSize(self.bounds.size, CGSizeZero)) {
        [self destroyDisplayFramebuffer];
        [self createDisplayFramebuffer];
    }
}

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
#else
- (void)reshape {
    [super reshape];
    CGSize viewSize = self.bounds.size;
    if ([self respondsToSelector:@selector(convertSizeToBacking:)])
    {
        viewSize = [self convertSizeToBacking:self.bounds.size];
    }
    
    if ( (_sizeInPixels.width == viewSize.width) && (_sizeInPixels.height == viewSize.height) )
    {
        return;
    }
   
    _sizeInPixels = viewSize;
    
    [self updateDisplayVertices];
}
 
#endif


- (void)dealloc
{
    [self destroyDisplayFramebuffer];
}

- (void)createDisplayFramebuffer;
{
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
#if defined(GPUPIXEL_IOS)
        glGenRenderbuffers(1, &displayRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
        
        [GPUPixel::GPUPixelContext::getInstance()->getEglContext() renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
        
        glGenFramebuffers(1, &displayFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER, displayRenderbuffer);
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
        
        lastBoundsSize = self.bounds.size;
        [self updateDisplayVertices];
#else
        // Perhaps I'll use an FBO at some time later, but for now will render directly to the screen
        if ([self respondsToSelector:@selector(convertSizeToBacking:)])
        {
            _sizeInPixels = [self convertSizeToBacking:self.bounds.size];
        }
        else
        {
            _sizeInPixels = self.bounds.size;
        }
#endif
    });
}

- (void)destroyDisplayFramebuffer;
{
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
#if defined(GPUPIXEL_IOS)
        if (displayFramebuffer)
        {
            glDeleteFramebuffers(1, &displayFramebuffer);
            displayFramebuffer = 0;
        }
        
        if (displayRenderbuffer)
        {
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
    if (!displayFramebuffer)
    {
        [self createDisplayFramebuffer];
    }
    
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
        glBindFramebuffer(GL_FRAMEBUFFER, displayFramebuffer);
        glViewport(0, 0, framebufferWidth, framebufferHeight);
    });
#else
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glViewport(0, 0, self.sizeInPixels.width, self.sizeInPixels.height);
    });
#endif
}
 
- (void)presentFramebuffer {
    GPUPixel::GPUPixelContext::getInstance()->runSync([&] {
#if defined(GPUPIXEL_IOS)
        glBindRenderbuffer(GL_RENDERBUFFER, displayRenderbuffer);
        GPUPixel::GPUPixelContext::getInstance()->presentBufferForDisplay();
#else
        [self.openGLContext flushBuffer];
#endif
    });
}

- (void)setBackgroundColorRed:(GLfloat)redComponent green:(GLfloat)greenComponent blue:(GLfloat)blueComponent alpha:(GLfloat)alphaComponent;
{
    backgroundColorRed = redComponent;
    backgroundColorGreen = greenComponent;
    backgroundColorBlue = blueComponent;
    backgroundColorAlpha = alphaComponent;
}

- (void)update:(float)frameTime {
    GPUPixel::GPUPixelContext::getInstance()->runSync([&]{
        GPUPixel::GPUPixelContext::getInstance()->setActiveShaderProgram(displayProgram);
        [self setDisplayFramebuffer];
        glClearColor(backgroundColorRed, backgroundColorGreen, backgroundColorBlue, backgroundColorAlpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#if defined(GPUPIXEL_MAC)
        // Re-render onscreen, flipped to a normal orientation
        CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
#endif
        CHECK_GL(glActiveTexture(GL_TEXTURE0));
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, inputFramebuffer->getTexture()));
        CHECK_GL(glUniform1i(colorMapUniformLocation, 0));

        CHECK_GL(glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, 0, 0, displayVertices));
        CHECK_GL(glVertexAttribPointer(texCoordAttribLocation,
                                    2,
                                    GL_FLOAT,
                                    0,
                                    0,
                                    [self textureCoordinatesForRotation:inputRotation]));
#if defined(GPUPIXEL_IOS)
                 CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
                 [self presentFramebuffer];
#else
                 BOOL canLockFocus = YES;
                 if ([self respondsToSelector:@selector(lockFocusIfCanDraw)])
                 {
            canLockFocus = [self lockFocusIfCanDraw];
        }
                 else
                 {
            [self lockFocus];
        }
                 
                 if (canLockFocus)
                 {
            CHECK_GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
            
            [self presentFramebuffer];
            CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
            [self unlockFocus];
        }
#endif
    });
}

- (void)setInputFramebuffer:(std::shared_ptr<GPUPixel::Framebuffer>)newInputFramebuffer
               withRotation:(GPUPixel::RotationMode)rotation
                    atIndex:(NSInteger)texIdx {
    std::shared_ptr<GPUPixel::Framebuffer> lastFramebuffer = inputFramebuffer;
    GPUPixel::RotationMode lastInputRotation = inputRotation;
    
    inputRotation = rotation;
    inputFramebuffer = newInputFramebuffer;
    
    if (lastFramebuffer != newInputFramebuffer && newInputFramebuffer &&
        ( !lastFramebuffer ||
         !(lastFramebuffer->getWidth() == newInputFramebuffer->getWidth() &&
           lastFramebuffer->getHeight() == newInputFramebuffer->getHeight() &&
           lastInputRotation == rotation)
         ))
    {
        [self performSelectorOnMainThread:@selector(updateDisplayVertices) withObject:nil waitUntilDone:YES];
    }
}

- (void)setFillMode:(GPUPixel::TargetView::FillMode)newValue;
{
    if (_fillMode != newValue) {
        _fillMode = newValue;
        [self updateDisplayVertices];
    }
}

- (void)updateDisplayVertices;
{
    if (inputFramebuffer == 0) return;
    
    CGFloat scaledWidth = 1.0;
    CGFloat scaledHeight = 1.0;
    
    int rotatedFramebufferWidth = inputFramebuffer->getWidth();
    int rotatedFramebufferHeight = inputFramebuffer->getHeight();
    if (rotationSwapsSize(inputRotation))
    {
        rotatedFramebufferWidth = inputFramebuffer->getHeight();
        rotatedFramebufferHeight = inputFramebuffer->getWidth();
    }
    
    CGRect insetRect = AVMakeRectWithAspectRatioInsideRect(CGSizeMake(rotatedFramebufferWidth, rotatedFramebufferHeight), self.bounds);
    
    if (_fillMode == GPUPixel::TargetView::FillMode::PreserveAspectRatio) {
        scaledWidth = insetRect.size.width / self.bounds.size.width;
        scaledHeight = insetRect.size.height / self.bounds.size.height;
    } else if (_fillMode == GPUPixel::TargetView::FillMode::PreserveAspectRatioAndFill) {
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


- (const GLfloat *)textureCoordinatesForRotation:(GPUPixel::RotationMode)rotationMode;
{
    static const GLfloat noRotationTextureCoordinates[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat rotateRightTextureCoordinates[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateLeftTextureCoordinates[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat verticalFlipTextureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
    static const GLfloat horizontalFlipTextureCoordinates[] = {
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };
    
    static const GLfloat rotateRightVerticalFlipTextureCoordinates[] = {
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
    };
    
    static const GLfloat rotateRightHorizontalFlipTextureCoordinates[] = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };
    
    static const GLfloat rotate180TextureCoordinates[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    switch(inputRotation)
    {
        case GPUPixel::NoRotation: return noRotationTextureCoordinates;
        case GPUPixel::RotateLeft: return rotateLeftTextureCoordinates;
        case GPUPixel::RotateRight: return rotateRightTextureCoordinates;
        case GPUPixel::FlipVertical: return verticalFlipTextureCoordinates;
        case GPUPixel::FlipHorizontal: return horizontalFlipTextureCoordinates;
        case GPUPixel::RotateRightFlipVertical: return rotateRightVerticalFlipTextureCoordinates;
        case GPUPixel::RotateRightFlipHorizontal: return rotateRightHorizontalFlipTextureCoordinates;
        case GPUPixel::Rotate180: return rotate180TextureCoordinates;
    }
}

@end
