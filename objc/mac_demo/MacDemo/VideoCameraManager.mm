#import "VideoCameraManager.h"

#pragma mark -
#pragma mark Private methods and instance variables
 
@interface VideoCameraManager ()
{
	AVCaptureDeviceInput *audioInput;
	AVCaptureAudioDataOutput *audioOutput;
    NSDate *startingCaptureTime;
	
	NSInteger _frameRate;
    
    dispatch_queue_t cameraProcessingQueue, audioProcessingQueue;
    
//    GLProgram *yuvConversionProgram;
    GLint yuvConversionPositionAttribute, yuvConversionTextureCoordinateAttribute;
    GLint yuvConversionLuminanceTextureUniform, yuvConversionChrominanceTextureUniform;
    
    int imageBufferWidth, imageBufferHeight;
}

- (void)updateOrientationSendToTargets;
- (void)convertYUVToRGBOutput;

@end

@implementation VideoCameraManager

@synthesize captureSessionPreset = _captureSessionPreset;
@synthesize captureSession = _captureSession;
@synthesize inputCamera = _inputCamera;
@synthesize runBenchmark = _runBenchmark;
@synthesize delegate = _delegate;
@synthesize horizontallyMirrorFrontFacingCamera = _horizontallyMirrorFrontFacingCamera, horizontallyMirrorRearFacingCamera = _horizontallyMirrorRearFacingCamera;

#pragma mark -
#pragma mark Initialization and teardown

+ (NSArray *)connectedCameraDevices;
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    return devices;
}

- (id)init;
{
    if (!(self = [self initWithSessionPreset:AVCaptureSessionPreset640x480 cameraDevice:nil]))
    {
		return nil;
    }
    
    return self;
}

- (id)initWithDeviceUniqueID:(NSString *)deviceUniqueID;
{
    if (!(self = [self initWithSessionPreset:AVCaptureSessionPreset640x480 deviceUniqueID:deviceUniqueID]))
    {
		return nil;
    }
    
    return self;
}

- (id)initWithSessionPreset:(NSString *)sessionPreset deviceUniqueID:(NSString *)deviceUniqueID;
{
    if (!(self = [self initWithSessionPreset:sessionPreset cameraDevice:[AVCaptureDevice deviceWithUniqueID:deviceUniqueID]]))
    {
		return nil;
    }
    
    return self;
}

- (id)initWithSessionPreset:(NSString *)sessionPreset cameraDevice:(AVCaptureDevice *)cameraDevice;
{
	if (!(self = [super init]))
    {
		return nil;
    }
    
	cameraProcessingQueue = dispatch_queue_create("com.sunsetlakesoftware.GPUImage.cameraProcessingQueue", NULL);
	audioProcessingQueue = dispatch_queue_create("com.sunsetlakesoftware.GPUImage.audioProcessingQueue", NULL);
    frameRenderingSemaphore = dispatch_semaphore_create(1);

	_frameRate = 0; // This will not set frame rate unless this value gets set to 1 or above
    _runBenchmark = NO;
    capturePaused = NO;
    outputRotation = kGPUImageNoRotation;
//    captureAsYUV = YES;
    captureAsYUV = NO;
 
	// Grab the back-facing or front-facing camera
    _inputCamera = nil;
    
    if (cameraDevice == nil)
    {
        _inputCamera = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    else
    {
        _inputCamera = cameraDevice;
    }
    
    if (!_inputCamera) {
        return nil;
    }
    
	// Create the capture session
	_captureSession = [[AVCaptureSession alloc] init];
	
    [_captureSession beginConfiguration];
    
	// Add the video input	
	NSError *error = nil;
	videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:_inputCamera error:&error];
	if ([_captureSession canAddInput:videoInput]) 
	{
		[_captureSession addInput:videoInput];
	}
	
	// Add the video frame output	
	videoOutput = [[AVCaptureVideoDataOutput alloc] init];
	[videoOutput setAlwaysDiscardsLateVideoFrames:NO];
    {
        // Despite returning a longer list of supported pixel formats, only RGB, RGBA, BGRA, and the YUV 4:2:2 variants seem to return cleanly
        [videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
//        [videoOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    }

    
    [videoOutput setSampleBufferDelegate:self queue:cameraProcessingQueue];
	if ([_captureSession canAddOutput:videoOutput])
	{
		[_captureSession addOutput:videoOutput];
	}
	else
	{
		NSLog(@"Couldn't add video output");
        return nil;
	}
    
	_captureSessionPreset = sessionPreset;
    [_captureSession setSessionPreset:_captureSessionPreset];
 
    [_captureSession commitConfiguration];
    
	return self;
}

- (void)dealloc 
{
    [self stopCameraCapture];
    [videoOutput setSampleBufferDelegate:nil queue:dispatch_get_main_queue()];
    [audioOutput setSampleBufferDelegate:nil queue:dispatch_get_main_queue()];
    
    [self removeInputsAndOutputs];
    
// ARC forbids explicit message send of 'release'; since iOS 6 even for dispatch_release() calls: stripping it out in that case is required.
//#if ( (__IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_6_0) || (!defined(__IPHONE_6_0)) )
#if __MAC_OS_X_VERSION_MAX_ALLOWED <= __MAC_10_7
    if (cameraProcessingQueue != NULL)
    {
        dispatch_release(cameraProcessingQueue);
    }

    if (audioProcessingQueue != NULL)
    {
        dispatch_release(audioProcessingQueue);
    }
    
    if (frameRenderingSemaphore != NULL)
    {
        dispatch_release(frameRenderingSemaphore);
    }
#endif
}

- (void)removeInputsAndOutputs;
{
    [_captureSession removeInput:videoInput];
    [_captureSession removeOutput:videoOutput];
    if (_microphone != nil)
    {
        [_captureSession removeInput:audioInput];
        [_captureSession removeOutput:audioOutput];
    }
}
 

#pragma mark -
#pragma mark Manage the camera video stream

- (void)startCameraCapture;
{
    if (![_captureSession isRunning])
	{
        startingCaptureTime = [NSDate date];
		[_captureSession startRunning];
	};
}

- (void)stopCameraCapture;
{
    if ([_captureSession isRunning])
    {
        [_captureSession stopRunning];
    }
}

- (void)pauseCameraCapture;
{
    capturePaused = YES;
}

- (void)resumeCameraCapture;
{
    capturePaused = NO;
}

- (void)rotateCamera
{
	if (self.frontFacingCameraPresent == NO)
		return;
	
    NSError *error;
    AVCaptureDeviceInput *newVideoInput;
    AVCaptureDevicePosition currentCameraPosition = [[videoInput device] position];
    
    if (currentCameraPosition == AVCaptureDevicePositionBack)
    {
        currentCameraPosition = AVCaptureDevicePositionFront;
    }
    else
    {
        currentCameraPosition = AVCaptureDevicePositionBack;
    }
    
    AVCaptureDevice *backFacingCamera = nil;
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
	for (AVCaptureDevice *device in devices) 
	{
		if ([device position] == currentCameraPosition)
		{
			backFacingCamera = device;
		}
	}
    newVideoInput = [[AVCaptureDeviceInput alloc] initWithDevice:backFacingCamera error:&error];
    
    if (newVideoInput != nil)
    {
        [_captureSession beginConfiguration];
        
        [_captureSession removeInput:videoInput];
        if ([_captureSession canAddInput:newVideoInput])
        {
            [_captureSession addInput:newVideoInput];
            videoInput = newVideoInput;
        }
        else
        {
            [_captureSession addInput:videoInput];
        }
        //captureSession.sessionPreset = oriPreset;
        [_captureSession commitConfiguration];
    }
    
    _inputCamera = backFacingCamera;
}

- (AVCaptureDevicePosition)cameraPosition 
{
    return [[videoInput device] position];
}

- (BOOL)isFrontFacingCameraPresent;
{
	NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
	
	for (AVCaptureDevice *device in devices)
	{
		if ([device position] == AVCaptureDevicePositionFront)
			return YES;
	}
	
	return NO;
}

- (void)setCaptureSessionPreset:(NSString *)captureSessionPreset;
{
	[_captureSession beginConfiguration];
	
	_captureSessionPreset = captureSessionPreset;
	[_captureSession setSessionPreset:_captureSessionPreset];
	
	[_captureSession commitConfiguration];
}

- (void)setFrameRate:(NSInteger)frameRate;
{
	_frameRate = frameRate;
	
	if (_frameRate > 0)
	{
		for (AVCaptureConnection *connection in videoOutput.connections)
		{
			if ([connection respondsToSelector:@selector(setVideoMinFrameDuration:)])
				connection.videoMinFrameDuration = CMTimeMake(1, (int32_t)_frameRate);
			
		}
	}
	else
	{
		for (AVCaptureConnection *connection in videoOutput.connections)
		{
			if ([connection respondsToSelector:@selector(setVideoMinFrameDuration:)])
				connection.videoMinFrameDuration = kCMTimeInvalid; // This sets videoMinFrameDuration back to default
		}
	}
}

- (NSInteger)frameRate;
{
	return _frameRate;
}

- (AVCaptureConnection *)videoCaptureConnection {
    for (AVCaptureConnection *connection in [videoOutput connections] ) {
		for ( AVCaptureInputPort *port in [connection inputPorts] ) {
			if ( [[port mediaType] isEqual:AVMediaTypeVideo] ) {
				return connection;
			}
		}
	}
    
    return nil;
}


#define INITIALFRAMESTOIGNOREFORBENCHMARK 5
#if 0
- (void)updateTargetsForVideoCameraUsingCacheTextureAtWidth:(int)bufferWidth height:(int)bufferHeight time:(CMTime)currentTime;
{
    // First, update all the framebuffers in the targets
    for (id<GPUImageInput> currentTarget in targets)
    {
        if ([currentTarget enabled])
        {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
            
            if (currentTarget != self.targetToIgnoreForUpdates)
            {
                [currentTarget setInputRotation:outputRotation atIndex:textureIndexOfTarget];
                [currentTarget setInputSize:CGSizeMake(bufferWidth, bufferHeight) atIndex:textureIndexOfTarget];
                
                if ([currentTarget wantsMonochromeInput] && captureAsYUV)
                {
                    [currentTarget setCurrentlyReceivingMonochromeInput:YES];
                    // TODO: Replace optimization for monochrome output
                    [currentTarget setInputFramebuffer:outputFramebuffer atIndex:textureIndexOfTarget];
                }
                else
                {
                    [currentTarget setCurrentlyReceivingMonochromeInput:NO];
                    [currentTarget setInputFramebuffer:outputFramebuffer atIndex:textureIndexOfTarget];
                }
            }
            else
            {
                [currentTarget setInputRotation:outputRotation atIndex:textureIndexOfTarget];
                [currentTarget setInputFramebuffer:outputFramebuffer atIndex:textureIndexOfTarget];
            }
        }
    }
    
    // Then release our hold on the local framebuffer to send it back to the cache as soon as it's no longer needed
    [outputFramebuffer unlock];
    
    // Finally, trigger rendering as needed
    for (id<GPUImageInput> currentTarget in targets)
    {
        if ([currentTarget enabled])
        {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
            
            if (currentTarget != self.targetToIgnoreForUpdates)
            {
                [currentTarget newFrameReadyAtTime:currentTime atIndex:textureIndexOfTarget];
            }
        }
    }
}

#endif

- (void)processVideoSampleBuffer:(CMSampleBufferRef)sampleBuffer;
{
    if (capturePaused)
    {
        return;
    }
    
    CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
    CVImageBufferRef cameraFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
    GLsizei bufferWidth = (GLsizei)CVPixelBufferGetWidth(cameraFrame);
    GLsizei bufferHeight = (GLsizei)CVPixelBufferGetHeight(cameraFrame);
    
	CMTime currentTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);

#if 0
    [GPUImageContext useImageProcessingContext];

    CVPixelBufferLockBaseAddress(cameraFrame, 0);
    
    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:CGSizeMake(bufferWidth, bufferHeight) onlyTexture:YES];
    
    glBindTexture(GL_TEXTURE_2D, [outputFramebuffer texture]);
    
    //        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(cameraFrame));
    
    // Using BGRA extension to pull in video frame data directly
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bytesPerRow / 3, bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(cameraFrame));
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, CVPixelBufferGetBaseAddress(cameraFrame));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bufferWidth, bufferHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, CVPixelBufferGetBaseAddress(cameraFrame));

    [self updateTargetsForVideoCameraUsingCacheTextureAtWidth:bufferWidth height:bufferHeight time:currentTime];
#endif
//    for (id<GPUImageInput> currentTarget in targets)
//    {
//        if ([currentTarget enabled])
//        {
//            if (currentTarget != self.targetToIgnoreForUpdates)
//            {
//                NSInteger indexOfObject = [targets indexOfObject:currentTarget];
//                NSInteger textureIndexOfTarget = [[targetTextureIndices objectAtIndex:indexOfObject] integerValue];
//                
//                [currentTarget setInputSize:CGSizeMake(bufferWidth, bufferHeight) atIndex:textureIndexOfTarget];
//                [currentTarget newFrameReadyAtTime:currentTime atIndex:textureIndexOfTarget];
//            }
//        }
//    }
    
    CVPixelBufferUnlockBaseAddress(cameraFrame, 0);
    
    if (_runBenchmark)
    {
        numberOfFramesCaptured++;
        if (numberOfFramesCaptured > INITIALFRAMESTOIGNOREFORBENCHMARK)
        {
            CFAbsoluteTime currentFrameTime = (CFAbsoluteTimeGetCurrent() - startTime);
            totalFrameTimeDuringCapture += currentFrameTime;
            NSLog(@"Average frame time : %f ms", [self averageFrameDurationDuringCapture]);
            NSLog(@"Current frame time : %f ms", 1000.0 * currentFrameTime);
        }
    }
}
#if 0
- (void)processAudioSampleBuffer:(CMSampleBufferRef)sampleBuffer;
{
    [self.audioEncodingTarget processAudioBuffer:sampleBuffer]; 
}

- (void)convertYUVToRGBOutput;
{
    [GPUImageContext setActiveShaderProgram:yuvConversionProgram];

    outputFramebuffer = [[GPUImageContext sharedFramebufferCache] fetchFramebufferForSize:CGSizeMake(imageBufferWidth, imageBufferHeight) textureOptions:self.outputTextureOptions onlyTexture:NO];
    [outputFramebuffer activateFramebuffer];
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f,
    };
    
    static const GLfloat textureCoordinates[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };
    
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, luminanceTexture);
	glUniform1i(yuvConversionLuminanceTextureUniform, 4);

    glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, chrominanceTexture);
	glUniform1i(yuvConversionChrominanceTextureUniform, 5);

    glVertexAttribPointer(yuvConversionPositionAttribute, 2, GL_FLOAT, 0, 0, squareVertices);
	glVertexAttribPointer(yuvConversionTextureCoordinateAttribute, 2, GL_FLOAT, 0, 0, textureCoordinates);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
#endif
#pragma mark -
#pragma mark Benchmarking

- (CGFloat)averageFrameDurationDuringCapture;
{
    return (totalFrameTimeDuringCapture / (CGFloat)(numberOfFramesCaptured - INITIALFRAMESTOIGNOREFORBENCHMARK)) * 1000.0;
}

#pragma mark -
#pragma mark AVCaptureVideoDataOutputSampleBufferDelegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    //Feature Detection Hook.
    if (self.delegate && [self.delegate respondsToSelector:@selector(willOutputSampleBuffer:)])
    {
        [self.delegate willOutputSampleBuffer:sampleBuffer];
    }
   
}

#pragma mark -
#pragma mark Accessors
#if 0
- (void)setAudioEncodingTarget:(GPUImageMovieWriter *)newValue;
{
    runSynchronouslyOnVideoProcessingQueue(^{
        [_captureSession beginConfiguration];
        
        if (newValue == nil)
        {
            if (audioOutput)
            {
                [_captureSession removeInput:audioInput];
                [_captureSession removeOutput:audioOutput];
                audioInput = nil;
                audioOutput = nil;
                _microphone = nil;
            }
        }
        else
        {
            _microphone = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
            audioInput = [AVCaptureDeviceInput deviceInputWithDevice:_microphone error:nil];
            if ([_captureSession canAddInput:audioInput])
            {
                [_captureSession addInput:audioInput];
            }
            audioOutput = [[AVCaptureAudioDataOutput alloc] init];
            
            if ([_captureSession canAddOutput:audioOutput])
            {
                [_captureSession addOutput:audioOutput];
            }
            else
            {
                NSLog(@"Couldn't add audio output");
            }
            [audioOutput setSampleBufferDelegate:self queue:audioProcessingQueue];
        }
        
        [_captureSession commitConfiguration];
        
        [super setAudioEncodingTarget:newValue];
    });
}

- (void)updateOrientationSendToTargets;
{
    runSynchronouslyOnVideoProcessingQueue(^{
        
        //    From the iOS 5.0 release notes:
        //    In previous iOS versions, the front-facing camera would always deliver buffers in AVCaptureVideoOrientationLandscapeLeft and the back-facing camera would always deliver buffers in AVCaptureVideoOrientationLandscapeRight.
                
        outputRotation = kGPUImageNoRotation;
        for (id<GPUImageInput> currentTarget in targets)
        {
            NSInteger indexOfObject = [targets indexOfObject:currentTarget];
            [currentTarget setInputRotation:outputRotation atIndex:[[targetTextureIndices objectAtIndex:indexOfObject] integerValue]];
        }
    });
}

- (void)setHorizontallyMirrorFrontFacingCamera:(BOOL)newValue
{
    _horizontallyMirrorFrontFacingCamera = newValue;
    [self updateOrientationSendToTargets];
}

- (void)setHorizontallyMirrorRearFacingCamera:(BOOL)newValue
{
    _horizontallyMirrorRearFacingCamera = newValue;
    [self updateOrientationSendToTargets];
}
#endif
 

@end
