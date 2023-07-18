/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>

@class PannelBeautiesModel;
NS_ASSUME_NONNULL_BEGIN
@protocol PanelBeautyAssembleViewDelegate <NSObject>
@required
/**
 
 美颜-腾讯直播SDK专用
 @param beauty 磨皮 0-9，数值越大，效果越明显
 @param white 美白 0-9，数值越大，效果越明显
 @param ruddiness 红润 0-9，数值越大，效果越明显
 */
- (void)beautyLevel:(NSInteger)beauty
     whitenessLevel:(NSInteger)white
     ruddinessLevel:(NSInteger)ruddiness
    brightnessLevel:(NSInteger)brightness;

/// 美颜
/// @param type 美颜类型：美白，磨皮，红润
/// @param beautyLevel 美颜参数，0-1，数值越大，效果越明显
- (void)handleBeautyWithType:(NSInteger)type level:(CGFloat)beautyLevel;

/**
 一键美颜
 
 @param model 一键美颜模型
 */
- (void)handleQuickBeautyValue:(PannelBeautiesModel*)model;

- (void)handleQuickBeautyWithSliderValue:(NSInteger)value
                        quickBeautyModel:(PannelBeautiesModel*)model;

/**
 美型
 
 @param type 美型类型
 @param value 数值
 */
- (void)handleFaceBeautyWithType:(NSInteger)type sliderValue:(NSInteger)value;

/**
 滤镜
 
 @param filter 滤镜
 */
- (void)handleFiltersEffectWithType:(NSInteger)filter
                     withFilterName:(NSString*)filterName;

- (void)takePhoto;
// 底部收起按钮
- (void)clickPackUp;

@end

@interface PanelBeautyAssembleView : UIView

@property(nonatomic, weak) id<PanelBeautyAssembleViewDelegate> delegate;

- (void)configureUI;
- (void)configureSlider;

@end

NS_ASSUME_NONNULL_END
