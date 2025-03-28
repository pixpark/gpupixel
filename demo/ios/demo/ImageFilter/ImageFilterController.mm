/*
 * GPUPixelDemo
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "ImageFilterController.h"

#import <gpupixel/gpupixel.h>

using namespace gpupixel;

@interface ImageFilterController () <UICollectionViewDelegate, UICollectionViewDataSource>
{
	GPUPixelView *gpuPixelView;
	std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
	std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
	std::shared_ptr<gpupixel::FaceMakeupFilter> lipstick_filter_;
	std::shared_ptr<gpupixel::FaceMakeupFilter> blusher_filter_;
	std::shared_ptr<SourceImage> gpuSourceImage;
	CADisplayLink *_displayLink;
}

// Filter intensity
@property(nonatomic, assign) CGFloat sharpenValue;
@property(nonatomic, assign) CGFloat blurValue;
@property(nonatomic, assign) CGFloat whithValue;
@property(nonatomic, assign) CGFloat saturationValue;
@property(nonatomic, assign) CGFloat thinFaceValue;
@property(nonatomic, assign) CGFloat eyeValue;
@property(nonatomic, assign) CGFloat lipstickValue;
@property(nonatomic, assign) CGFloat blusherValue;

// UI
@property (nonatomic, strong) NSArray<NSString *> *dataSource;
@property (nonatomic, strong) UIView *toolbarView;
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) UICollectionViewFlowLayout *flowLayout;
@property (nonatomic, strong) UISwitch *effectSwitch;
@property (nonatomic, strong) UISlider *slider;

@end

static NSString * const reuseIdentifier = @"UICollectionViewCell";

@implementation ImageFilterController

#pragma mark - Lazy load
- (NSArray<NSString *> *)dataSource
{
	if (!_dataSource) {
		_dataSource = @[
			@"锐化",
			@"磨皮",
			@"美白",
			@"瘦脸",
			@"大眼",
			@"口红",
			@"腮红"
		];
	}
	return _dataSource;
}

- (UIView *)toolbarView
{
	if (!_toolbarView) {
		_toolbarView = [[UIView alloc] init];
		_toolbarView.backgroundColor = UIColor.blackColor;
		_toolbarView.translatesAutoresizingMaskIntoConstraints = NO;
	}
	return _toolbarView;
}

- (UICollectionView *)collectionView
{
	if (!_collectionView) {
		_collectionView = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:self.flowLayout];
		_collectionView.delegate = self;
		_collectionView.dataSource = self;
		_collectionView.contentInset = UIEdgeInsetsMake(0, 10, 0, 10);
		_collectionView.backgroundColor = UIColor.clearColor;
		_collectionView.translatesAutoresizingMaskIntoConstraints = NO;
		[_collectionView registerClass:[UICollectionViewCell class] forCellWithReuseIdentifier:reuseIdentifier];
	}
	return _collectionView;
}

- (UICollectionViewFlowLayout *)flowLayout
{
	if (!_flowLayout) {
		_flowLayout = [[UICollectionViewFlowLayout alloc] init];
		_flowLayout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
		_flowLayout.itemSize = CGSizeMake(80, 40);
		_flowLayout.minimumLineSpacing = 10;
		_flowLayout.minimumInteritemSpacing = 5;
	}
	return _flowLayout;
}

- (UISwitch *)effectSwitch
{
	if (_effectSwitch == nil) {
		_effectSwitch = [[UISwitch alloc] init];
		_effectSwitch.on = YES;
		_effectSwitch.translatesAutoresizingMaskIntoConstraints = NO;
		[_effectSwitch addTarget:self action:@selector(onFilterSwitchChange:) forControlEvents:UIControlEventValueChanged];
	}
	return _effectSwitch;
}

- (UISlider *)slider
{
	if (!_slider) {
		_slider = [[UISlider alloc] init];
		_slider.minimumValue = 0;
		_slider.maximumValue = 10;
		_slider.value = 0;
		_slider.translatesAutoresizingMaskIntoConstraints = NO;
		[_slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
	}
	return _slider;
}

#pragma mark - Life cycle
- (void)viewWillDisappear:(BOOL)animated
{
	_displayLink.paused = YES;
	[super viewWillDisappear:animated];
}

- (void)viewDidLoad
{
	[super viewDidLoad];
	[self.view setBackgroundColor:UIColor.whiteColor];
	// screen always light
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
	
	// setup filter
	[self setupFilter];
	[self setupUI];
	[self setupDisplayLink];
}

#pragma mark - Setup
- (void)setupFilter
{
	gpupixel::GPUPixelContext::getInstance()->runSync([&] {
		gpuPixelView = [[GPUPixelView alloc] initWithFrame:self.view.frame];
		gpuPixelView.translatesAutoresizingMaskIntoConstraints = NO;
		[self.view addSubview:gpuPixelView];
		
		lipstick_filter_ = LipstickFilter::create();
		blusher_filter_ = BlusherFilter::create();
		beauty_face_filter_ = BeautyFaceFilter::create();
		face_reshape_filter_ = FaceReshapeFilter::create();
		
		NSString *imagePath = [[NSBundle mainBundle] pathForResource:@"sample_face" ofType:@"png"];
		gpuSourceImage = SourceImage::create([imagePath UTF8String]);
		gpuSourceImage->RegLandmarkCallback([=](std::vector<float> landmarks) {
			lipstick_filter_->SetFaceLandmarks(landmarks);
			blusher_filter_->SetFaceLandmarks(landmarks);
			face_reshape_filter_->SetFaceLandmarks(landmarks);
		});
		
		// filter pipline
		gpuSourceImage
		->addSink(lipstick_filter_)
		->addSink(blusher_filter_)
		->addSink(face_reshape_filter_)
		->addSink(beauty_face_filter_)
		->addSink(gpuPixelView);
		
		[gpuPixelView setBackgroundColor:[UIColor grayColor]];
		[gpuPixelView setFillMode:(gpupixel::SinkRender::PreserveAspectRatioAndFill)];
	});
}

- (void)setupUI
{
	[self setupNavigation];
	[self.view addSubview:self.toolbarView];
	[self.view addSubview:self.effectSwitch];
	[self.toolbarView addSubview:self.slider];
	[self.toolbarView addSubview:self.collectionView];
	
	[self setupConstraints];
	
	[self.collectionView selectItemAtIndexPath:[NSIndexPath indexPathForItem:0 inSection:0] animated:NO scrollPosition:UICollectionViewScrollPositionNone];
	[self onFilterSelectChange:0];
}

- (void)setupNavigation
{
	UIBarButtonItem *backItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(backAction)];
	self.navigationItem.leftBarButtonItem = backItem;
	UIBarButtonItem *saveItem = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemSave target:self action:@selector(saveImageAction)];
	self.navigationItem.rightBarButtonItem = saveItem;
}

- (void)setupConstraints
{
	[NSLayoutConstraint activateConstraints:@[
		[gpuPixelView.topAnchor constraintEqualToAnchor:self.view.topAnchor],
		[gpuPixelView.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
		[gpuPixelView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
		
		[self.effectSwitch.bottomAnchor constraintEqualToAnchor:gpuPixelView.bottomAnchor constant:-10],
		[self.effectSwitch.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-10],
		
		[self.toolbarView.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
		[self.toolbarView.topAnchor constraintEqualToAnchor:gpuPixelView.bottomAnchor],
		[self.toolbarView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
		[self.toolbarView.bottomAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor],
		
		[self.slider.centerXAnchor constraintEqualToAnchor:self.toolbarView.centerXAnchor],
		[self.slider.topAnchor constraintEqualToAnchor:self.toolbarView.topAnchor constant:10],
		[self.slider.leadingAnchor constraintEqualToAnchor:self.toolbarView.leadingAnchor constant:30],
		
		[self.collectionView.heightAnchor constraintEqualToConstant:40],
		[self.collectionView.centerXAnchor constraintEqualToAnchor:self.toolbarView.centerXAnchor],
		[self.collectionView.topAnchor constraintEqualToAnchor:self.slider.bottomAnchor constant:10],
		[self.collectionView.leadingAnchor constraintEqualToAnchor:self.toolbarView.leadingAnchor],
		[self.collectionView.bottomAnchor constraintEqualToAnchor:self.toolbarView.bottomAnchor],
	]];
}

- (void)setupDisplayLink
{
	_displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(displayLinkDidFire:)];
	_displayLink.paused = YES;
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_10_0
	_displayLink.preferredFramesPerSecond = 30;
#else
	[_displayLink setFrameInterval:2];
#endif
	[_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
	
	_displayLink.paused = NO;
}

#pragma mark - Selectors
/// Back to home page and release all memory of gpupixel
- (void)backAction
{
	[_displayLink invalidate];
	_displayLink = nil;
	
	beauty_face_filter_ = nil;
	face_reshape_filter_ = nil;
	lipstick_filter_ = nil;
	blusher_filter_ = nil;
	[gpuPixelView removeFromSuperview];
	gpuPixelView = nil;
	gpuSourceImage = nil;
	gpupixel::GPUPixelContext::getInstance()->destroy();
	
	[self.navigationController popViewControllerAnimated:true];
}

/// Get image from gpupixel after render
- (void)saveImageAction
{
	int width = gpuSourceImage->getRotatedFramebufferWidth();
	int height = gpuSourceImage->getRotatedFramebufferHeight();
	// "beauty_face_filter_" is last filter in gpuSourceImage
	unsigned char *pixels = gpuSourceImage->captureAProcessedFrameData(beauty_face_filter_, width, height);
	size_t bitsPerComponent = 8; //R,G,B,A bits
	size_t bytesPerRow = width * 4; //per pixel include R,G,B,A
	CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB(); //Create RGB Color Space
	uint32_t bitmapInfo = kCGImageAlphaPremultipliedLast | kCGImageByteOrder32Big;
	CGContextRef context = CGBitmapContextCreate(pixels, width, height, bitsPerComponent, bytesPerRow, space, bitmapInfo);
	CGImageRef cgImage = CGBitmapContextCreateImage(context);
	CIImage *ciImage = [CIImage imageWithCGImage:cgImage];
	UIImage *resultImage = [UIImage imageWithCIImage:ciImage];
	NSLog(@"%@", resultImage);
	
	//Release memory
	free(pixels);
	CGContextRelease(context);
	CGImageRelease(cgImage);
}

- (void)onFilterSwitchChange:(UISwitch *)sender
{
	if (sender.isOn) {
		[self setSharpenValue:self.sharpenValue];
		[self setBlurValue: self.blurValue];
		[self setWhithValue:self.whithValue];
		[self setThinFaceValue:self.thinFaceValue];
		[self setEyeValue: self.eyeValue];
		[self setLipstickValue: self.lipstickValue];
		[self setBlusherValue: self.blusherValue];
	}
}

- (void)sliderValueChanged:(UISlider *)slider
{
	NSIndexPath *selectedIndexPath = [self.collectionView indexPathsForSelectedItems].firstObject;
	NSInteger selectedIndex = selectedIndexPath ? selectedIndexPath.row : 0;
	
	if (selectedIndex == 0) {  // 锐化
		[self setSharpenValue:slider.value];
	} else if (selectedIndex == 1) { // 磨皮
		[self setBlurValue: slider.value];
	} else if (selectedIndex == 2) {  // 美白
		[self setWhithValue: slider.value];
	} else if (selectedIndex == 3) {  // 瘦脸
		[self setThinFaceValue: slider.value];
	} else if (selectedIndex == 4) {  // 大眼
		[self setEyeValue: slider.value];
	} else if (selectedIndex == 5) {  // 口红
		[self setLipstickValue: slider.value];
	} else if (selectedIndex == 6) {  // 腮红
		[self setBlusherValue: slider.value];
	}
}

- (void)onFilterSelectChange:(NSInteger)selectedIndex
{
	if (selectedIndex == 0) {  // 锐化
		self.slider.value = _sharpenValue;
	} else if (selectedIndex == 1) {  // 磨皮
		self.slider.value = _blurValue;
	} else if (selectedIndex == 2) {  // 美白
		self.slider.value = _whithValue;
	} else if (selectedIndex == 3) {  // 瘦脸
		self.slider.value = _thinFaceValue;
	} else if (selectedIndex == 4) {  // 大眼
		self.slider.value = _eyeValue;
	} else if (selectedIndex == 5) {  // 口红
		self.slider.value = _lipstickValue;
	} else if (selectedIndex == 6) {  // 腮红
		self.slider.value = _blusherValue;
	}
}

- (void)displayLinkDidFire:(CADisplayLink *)displayLink
{
	if (!self.effectSwitch.isOn) {
		beauty_face_filter_->setSharpen(0);
		beauty_face_filter_->setBlurAlpha(0);
		beauty_face_filter_->setWhite(0);
		face_reshape_filter_->setFaceSlimLevel(0);
		face_reshape_filter_->setEyeZoomLevel(0);
		lipstick_filter_->setBlendLevel(0);
		blusher_filter_->setBlendLevel(0);
	}
	
	gpuSourceImage->Render();
}

#pragma mark - 属性赋值
- (void)setSharpenValue:(CGFloat)value
{
	_sharpenValue = value;
	beauty_face_filter_->setSharpen(value/2.5);
}

- (void)setBlurValue:(CGFloat)value
{
	_blurValue = value;
	beauty_face_filter_->setBlurAlpha(value/10);
}
- (void)setWhithValue:(CGFloat)value
{
	_whithValue = value;
	beauty_face_filter_->setWhite(value/20);
}

- (void)setSaturationValue:(CGFloat)value
{
	_saturationValue = value;
}

- (void)setThinFaceValue:(CGFloat)value
{
	_thinFaceValue = value;
	face_reshape_filter_->setFaceSlimLevel(value/100);
}

- (void)setEyeValue:(CGFloat)value
{
	_eyeValue = value;
	face_reshape_filter_->setEyeZoomLevel(value/50);
}

- (void)setLipstickValue:(CGFloat)value
{
	_lipstickValue = value;
	lipstick_filter_->setBlendLevel(value/10);
}

- (void)setBlusherValue:(CGFloat)value
{
	_blusherValue = value;
	blusher_filter_->setBlendLevel(value/10);
}

#pragma mark - UICollectionViewDataSource
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return self.dataSource.count;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    UICollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:reuseIdentifier forIndexPath:indexPath];
    UILabel *label = [[UILabel alloc] initWithFrame:cell.bounds];
    label.text = self.dataSource[indexPath.row];
    label.textAlignment = NSTextAlignmentCenter;
    [cell.contentView addSubview:label];
    return cell;
}

#pragma mark - UICollectionViewDelegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [self onFilterSelectChange:indexPath.row];
}

@end
