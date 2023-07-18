/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>
@class PannelBeautiesModel;
NS_ASSUME_NONNULL_BEGIN

@interface PannelBeautyMenuCell : UICollectionViewCell

@property(nonatomic, strong) PannelBeautiesModel* menuModel;
@property(nonatomic, assign) BOOL isSimplification;  ///< 精简版
- (void)switchBeautyEffect:(BOOL)isSelected;
- (void)takePhotoAnimation;
- (void)changeRecordState:(BOOL)isRecording;
@end

NS_ASSUME_NONNULL_END
