/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
@protocol PanelBeautyFaceViewDelegate <NSObject>
@required
- (void)handleFaceEffects:(NSInteger)beautyType sliderValue:(NSInteger)value;
@end

@interface PanelBeautyFaceView : UIView
@property(nonatomic, weak) id<PanelBeautyFaceViewDelegate> delegate;
// 根据版本类型设置美型UI界面
- (void)configureFaceData;

/**
 选中一键美颜功能的后，取消美型选中状态

 @param type 选中的美型类型
 */
- (void)cancelSelectedFaceType:(NSInteger)type;

/**
 获取当前选中的类型索引
 */
- (NSInteger)currentIndex;

@end

NS_ASSUME_NONNULL_END
