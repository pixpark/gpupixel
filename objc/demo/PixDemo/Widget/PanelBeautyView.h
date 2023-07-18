/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
@protocol PanelBeautyViewDelegate <NSObject>
@required

/**
 美颜

 @param beautyType 美颜类型，详情见PanelFiltersContants.h
 @param value 美颜参数
 */
- (void)handleBeautyEffects:(NSInteger)beautyType sliderValue:(NSInteger)value;
@end

@interface PanelBeautyView : UIView
@property(nonatomic, weak) id<PanelBeautyViewDelegate> delegate;

/**
 选中一键美颜功能的后，取消美颜选中状态

 @param type 选中的美颜类型
 */
- (void)cancelSelectedBeautyType:(NSInteger)type;

/**
 获取当前选中的类型索引
 */
- (NSInteger)currentIndex;

@end

NS_ASSUME_NONNULL_END
