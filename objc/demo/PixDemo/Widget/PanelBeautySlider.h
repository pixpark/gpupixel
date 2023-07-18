/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface PanelBeautySlider : UISlider
@property(nonatomic, copy) NSString* sliderValue;
@property(nonatomic, strong) UIFont* sliderFont;
@property(nonatomic, strong) UIColor* valueColor;
@property(nonatomic, copy) void (^touchDown)(PanelBeautySlider* slider);
@property(nonatomic, copy) void (^valueChanged)(PanelBeautySlider* slider);
@property(nonatomic, copy) void (^touchUpInside)(PanelBeautySlider* slider);

@end

NS_ASSUME_NONNULL_END
