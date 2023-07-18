/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelBeautyAssembleView.h"
#import "PanelBeautyFaceView.h"
#import "PanelFiltersView.h"
#import "PanelBeautyView.h"
#import "PanelSegmentControl.h"
#import "PanelBeautyParams.h"

#import "PanelBeautySlider.h"
#import "PannelBeautiesModel.h"
#import "PanelBottomView.h"

@interface PanelBeautyAssembleView()<PanelBeautyViewDelegate,PanelBeautyFaceViewDelegate,PanelFiltersViewDelegate>
@property (nonatomic, strong) PanelSegmentControl *segmentControl;
@property (nonatomic, strong) PanelBeautyView *beautyView;//美颜
@property (nonatomic, strong) PanelBeautyFaceView *faceView;//美型
//@property (nonatomic, strong) PanelCompleteBeautyView *completeView;//一键美颜
@property (nonatomic, strong) PanelFiltersView *filtersView;//滤镜
@property (nonatomic, strong) UIView *lineView;
@property (nonatomic, strong) NSArray *viewsArray;
@property (nonatomic, strong) UIView *lastView;
@property (nonatomic, strong) PanelBeautySlider *slider;
@property (nonatomic, assign) NSInteger beautyLevel;
@property (nonatomic, assign) NSInteger whiteLevel;
@property (nonatomic, assign) NSInteger ruddinessLevel;
@property (nonatomic, assign) NSInteger brightnessLevel;
@property (nonatomic, assign) PanelBeautyAssembleType assembleType;
@property (nonatomic, assign) PanelBeautyType beautyType;
@property (nonatomic, assign) PanelBeautyFaceType faceType;
@property (nonatomic, strong) PannelBeautiesModel *quickBeautyModel;

@property (nonatomic, strong) PanelBottomView * bottomView;

- (void)initValues;

@end
@implementation PanelBeautyAssembleView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor clearColor];
        //如果有默认初始值，可以在这里设置
//        self.beautyLevel = [sproutCommon getYBskin_smooth];
//        self.whiteLevel = [sproutCommon getYBskin_whiting];
//        self.ruddinessLevel = [sproutCommon getYBskin_tenderness];
        self.faceType = -1;//默认不选择状态
        self.beautyType = -1;//默认不选择状态
        [self addSubview:self.slider];
       
        [self initValues];
    }
    return self;
}

- (void)initValues
{
    self.beautyLevel = 5;
    self.whiteLevel = 5;
    self.ruddinessLevel = 7;
    self.brightnessLevel = 57;//默认
}

- (void)configureUI {
  
    NSArray *arr = @[@"美颜",@"美型"/*,@"滤镜"*/];
    if (_segmentControl) {
        return;
    }
    _segmentControl = [[PanelSegmentControl alloc] initWithTitles:arr];
    CGFloat bottom =  _slider.frame.origin.y + _slider.frame.size.height;
    _segmentControl.frame = CGRectMake(0, bottom+20, window_width, PanelStickerSectionHeight);
    
    _segmentControl.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:PanelBlackAlpha];
    [_segmentControl setTextAttributes:@{NSFontAttributeName: Font_12, NSForegroundColorAttributeName: FontColorBlackNormal}
                              forState:UIControlStateNormal];
    [_segmentControl setTextAttributes:@{NSFontAttributeName: Font_12, NSForegroundColorAttributeName: FontColorSelected}
                              forState:UIControlStateSelected];
    _segmentControl.selectedSegmentIndex = 0;
    _segmentControl.widthStyle = WNSegmentedControlWidthStyleFixed;
    [_segmentControl addTarget:self action:@selector(switchList:) forControlEvents:UIControlEventValueChanged];
    [self addSubview:_segmentControl];
    [self addSubview:self.lineView];
    [self addSubview:self.beautyView];
    self.lastView = self.beautyView;
    
    [self addSubview:self.bottomView];
    self.slider.maximumValue = 9;
    NSInteger currentIndex = [self.beautyView currentIndex];
    if(currentIndex == 0 || currentIndex == -1){
        self.slider.hidden = YES;
    }
    
    [self.faceView configureFaceData];
  
    _viewsArray =  @[self.beautyView,self.faceView,self.filtersView];
}
- (void)configureSlider{
   // self.slider.maximumValue = 9;
}
#pragma mark - Action
- (void)switchList:(PanelSegmentControl *)segment {
    UIView *view = [self.viewsArray objectAtIndex:segment.selectedSegmentIndex];
    self.slider.hidden = [view isEqual:self.filtersView];
    self.assembleType = segment.selectedSegmentIndex;
    if ([view isEqual:self.beautyView]) {
        NSInteger current = [self.beautyView currentIndex];
        if (current == 0 || current == -1){
            self.slider.hidden = YES;
        }else{
            self.slider.hidden = NO;
        }
        NSString *faceKey = [NSString stringWithFormat:@"beauty_%ld",(long)self.beautyType];
        NSInteger currentValue = [[NSUserDefaults standardUserDefaults] integerForKey:faceKey];
        [self.slider setValue:currentValue animated:YES];
        self.slider.maximumValue = 9;
    } else {
        self.slider.maximumValue = 100;
    }
    if ([view isEqual:self.faceView]) {
        NSInteger current = [self.faceView currentIndex];
        if (current == 0 || current == -1){
            self.slider.hidden = YES;
        }else{
            self.slider.hidden = NO;
        }
        [self.faceView configureFaceData];
        NSString *faceKey = [NSString stringWithFormat:@"face_%ld",(long)self.faceType];
        NSInteger currentValue = [[NSUserDefaults standardUserDefaults] integerForKey:faceKey];
        [self.slider setValue:currentValue animated:YES];
    }
   
    if (![view isEqual:self.lastView]) {
       [self.lastView removeFromSuperview];
    }
    [self addSubview:view];
    self.lastView = view;
    
    [self bringSubviewToFront:self.bottomView];
}
//slider 滑动修改对应的效果
- (void)handleBeautyAssembleEffectWithValue:(NSInteger)value {
    switch (self.assembleType) {
        case 0:{
          [self handleBeautyEffectsWithSliderValue:value];
        }
            
            break;
        case 1:
            [self handleFaceEffectsWithSliderValue:value];
            
            break;
        case 2:
            [self handleQuickBeautyWithSliderValue:value];
            break;
        default:
            break;
    }
}
#pragma mark - 底部按钮响应
- (void)cameraAction:(BOOL)isTakePhoto{
    NSLog(@"点击了拍照");
    if (isTakePhoto) {
        if ([self.delegate respondsToSelector:@selector(takePhoto)]) {
            [self.delegate takePhoto];
        }
    }else{
        if ([self.delegate respondsToSelector:@selector(clickPackUp)]) {
            [self.delegate clickPackUp];
        }
    }
    
    
}

#pragma mark - 美颜-腾讯直播用
- (void)handleBeautyEffectsOfTXWithSliderValue:(NSInteger)value {
    switch (self.beautyType) {
        case PanelBeautyType_Original:{
            self.beautyLevel = 0;
            self.whiteLevel = 0;
            self.ruddinessLevel = 0;
            self.brightnessLevel = 50;//默认
        }
            break;
        case PanelBeautyType_White:
            self.whiteLevel = value;

            break;
        case PanelBeautyType_Mopi:
            self.beautyLevel = value;
            break;
        case PanelBeautyType_Ruddiess:
            self.ruddinessLevel = value;
            break;
        case PanelBeautyType_Brightness:
            self.brightnessLevel = value*100/9.0;
            break;
        default:
            break;
    }
//    if (self.beautyType != PanelBeautyType_Brightness) {
//        self.brightnessLevel = 57;
//    }
    if (self.brightnessLevel > 90){
        self.brightnessLevel = 90;
    }
    if (self.brightnessLevel < 10){
        self.brightnessLevel = 10;
    }
    
    if ([self.delegate respondsToSelector:@selector(beautyLevel:whitenessLevel:ruddinessLevel: brightnessLevel:)]) {
        NSString *str = [[NSUserDefaults standardUserDefaults] objectForKey:@"hasSelectedQuickBeauty"];
        if ([str isEqualToString:@"YES"]) {
            //2020-07-04 现在相机有默认美颜效果，取消一键美颜时需要恢复默认的美颜效果
            //[self.delegate beautyLevel:0 whitenessLevel:0 ruddinessLevel:0 brightnessLevel:50];//为了取消一键美颜的效果
            [self.delegate beautyLevel:5 whitenessLevel:5 ruddinessLevel:7 brightnessLevel:57];
            [[NSUserDefaults standardUserDefaults] setObject:@"NO" forKey:@"hasSelectedQuickBeauty"];//保证只执行一次
        }
         
        [self.delegate beautyLevel:self.beautyLevel whitenessLevel:self.whiteLevel ruddinessLevel:self.ruddinessLevel brightnessLevel:self.brightnessLevel];
    }
    NSString *beautKey = [NSString stringWithFormat:@"beauty_%ld",(long)self.beautyType];
    [[NSUserDefaults standardUserDefaults] setInteger:value forKey:beautKey];
}

- (void)handleBeautyEffectsWithSliderValue:(NSInteger)value {
    if ([self.delegate respondsToSelector:@selector(handleBeautyWithType:level:)]) {
        NSString *str = [[NSUserDefaults standardUserDefaults] objectForKey:@"hasSelectedQuickBeauty"];
        if ([str isEqualToString:@"YES"]) {
            [self.delegate handleBeautyWithType:0 level:0];//为了取消一键美颜的效果
            [[NSUserDefaults standardUserDefaults] setObject:@"NO" forKey:@"hasSelectedQuickBeauty"];//保证只执行一次
        }
        if (self.beautyType == PanelBeautyType_Brightness) {
            [self.delegate handleBeautyWithType:self.beautyType level:value*10];
        }else{
            [self.delegate handleBeautyWithType:self.beautyType level:value/9.0];
        }
        
    }
    NSString *beautKey = [NSString stringWithFormat:@"beauty_%ld",(long)self.beautyType];
    [[NSUserDefaults standardUserDefaults] setInteger:value forKey:beautKey];
}

#pragma mark - 美型
- (void)handleFaceEffectsWithSliderValue:(NSInteger)value {
    if ([self.delegate respondsToSelector:@selector(handleFaceBeautyWithType:sliderValue:)]) {
        NSString *str = [[NSUserDefaults standardUserDefaults] objectForKey:@"hasSelectedQuickBeauty"];
        if ([str isEqualToString:@"YES"]) {
             [self.delegate handleFaceBeautyWithType:0 sliderValue:0];//为了取消一键美颜的效果
            [[NSUserDefaults standardUserDefaults] setObject:@"NO" forKey:@"hasSelectedQuickBeauty"];//保证只执行一次
        }
        [self.delegate handleFaceBeautyWithType:self.faceType sliderValue:value];
    }
    NSString *faceKey = [NSString stringWithFormat:@"face_%ld",(long)self.faceType];
    [[NSUserDefaults standardUserDefaults] setInteger:value forKey:faceKey];
}

#pragma mark - 一键美颜
- (void)handleQuickBeautyWithSliderValue:(NSInteger)value {
    if ([self.delegate respondsToSelector:@selector(handleQuickBeautyWithSliderValue: quickBeautyModel:)]) {
        [self.delegate handleQuickBeautyWithSliderValue:value quickBeautyModel:self.quickBeautyModel];
    }
}

#pragma mark - delegate
//美颜
- (void)handleBeautyEffects:(NSInteger)type sliderValue:(NSInteger)value {
    //点击原图时slider隐藏
    if (type == 0){
        _slider.hidden = YES;
    }else{
        _slider.hidden = NO;
    }
    self.beautyType = type;
    [self.slider setSliderValue:[NSString stringWithFormat:@"%ld",(long)value]];
    [self.slider setValue:(NSInteger)value animated:YES];
}
//美型
- (void)handleFaceEffects:(NSInteger)type sliderValue:(NSInteger)value {
    if (type == 0){
        _slider.hidden = YES;
    }else{
        _slider.hidden = NO;
    }
    self.faceType = type;
    [self.slider setSliderValue:[NSString stringWithFormat:@"%ld",(long)value]];
    [self.slider setValue:(NSInteger)value animated:YES];
}


//滤镜
- (void)handleFiltersEffect:(NSInteger)filterType filterName:(nonnull NSString *)filtetName {
    if ([self.delegate respondsToSelector:@selector(handleFiltersEffectWithType: withFilterName:)]) {
        [self.delegate handleFiltersEffectWithType:filterType withFilterName:filtetName];
    }
}



#pragma mark - lazy
- (PanelBeautyView *)beautyView {
    if (!_beautyView) {
        CGFloat bottom =  _lineView.frame.origin.y + _lineView.frame.size.height;
        _beautyView = [[PanelBeautyView alloc] initWithFrame:CGRectMake(0, bottom, window_width, PanelBeautyAssembleViewHeight -bottom - PanelBottomViewHeight)];
        _beautyView.delegate = self;
    }
    return _beautyView;
}

- (PanelBeautyFaceView *)faceView {
    if (!_faceView) {
        CGFloat bottom =  _lineView.frame.origin.y + _lineView.frame.size.height;
//        CGFloat bottom =  _segmentControl.frame.origin.y + _segmentControl.frame.size.height;
        _faceView = [[PanelBeautyFaceView alloc] initWithFrame:CGRectMake(0, bottom, window_width, PanelBeautyAssembleViewHeight-bottom-PanelBottomViewHeight)];
        _faceView.delegate = self;
    }
    return _faceView;
}

- (PanelFiltersView *)filtersView {
    if (!_filtersView) {
        CGFloat bottom =  _lineView.frame.origin.y + _lineView.frame.size.height;
        _filtersView = [[PanelFiltersView alloc] initWithFrame:CGRectMake(0, bottom, window_width, PanelBeautyAssembleViewHeight-bottom-PanelBottomViewHeight)];
        _filtersView.delegate = self;
    }
    return _filtersView;
}


- (UIView *)lineView {
    if (!_lineView) {
        CGFloat bottom =  _segmentControl.frame.origin.y + _segmentControl.frame.size.height;
        _lineView = [[UIView alloc] initWithFrame:CGRectMake(0, bottom, window_width, 0.5)];
        _lineView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:PanelBlackAlpha];
        UIView * view = [[UIView alloc] initWithFrame:_lineView.bounds];
        [_lineView addSubview:view];
        view.backgroundColor = LineColor;
    }
    return _lineView;
}

- (PanelBottomView*)bottomView{
    
    if (!_bottomView) {
        __weak typeof(self) weakSelf = self;
        CGFloat bottom =  _beautyView.frame.origin.y + _beautyView.frame.size.height;
        _bottomView = [[PanelBottomView alloc] initWithFrame:CGRectMake(0, bottom, window_width, PanelBottomViewHeight)];
        _bottomView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.5];
        _bottomView.clickBtn = ^(BOOL isTakePhoto) {
            [weakSelf cameraAction:isTakePhoto];
        };
    }
    return _bottomView;
}


- (PanelBeautySlider *)slider {
    if (!_slider) {
        _slider = [[PanelBeautySlider alloc] initWithFrame:CGRectMake(60, PanelSliderwTop, self.frame.size.width - 60 * 2, PanelSliderwHeight)];
        _slider.minimumValue = 0;
        _slider.maximumValue = 100;
        UIImage *minImg = BundleImg(@"wire")
        [_slider setMinimumTrackImage:minImg forState:UIControlStateNormal];
        UIImage *maxImg = BundleImg(@"wire drk");
        [_slider setMaximumTrackImage:maxImg forState:UIControlStateNormal];
        UIImage *pointImg = BundleImg(@"sliderButton");
        [_slider setThumbImage:pointImg forState:UIControlStateNormal];
        _slider.continuous = YES;
        __weak typeof(self) weakSelf = self;
        _slider.valueChanged = ^(PanelBeautySlider * _Nonnull slider) {
            [weakSelf handleBeautyAssembleEffectWithValue:slider.value];
            weakSelf.slider.sliderValue = [NSString stringWithFormat:@"%ld", (long)slider.value];
        };
    }
    return _slider;
}


@end

