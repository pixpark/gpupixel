//
/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>
#import "PanelBeautyParams.h"
NS_ASSUME_NONNULL_BEGIN

@interface PanelBottomView : UIView
@property(nonatomic, strong) void (^clickBtn)(BOOL isTakePhoto);
@property(nonatomic, assign) BOOL isSticker;
@end

NS_ASSUME_NONNULL_END
