/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN
@protocol PanelFiltersViewDelegate <NSObject>
@required
- (void)handleFiltersEffect:(NSInteger)filterType
                 filterName:(NSString*)filtetName;

@end
@interface PanelFiltersView : UIView
@property(nonatomic, weak) id<PanelFiltersViewDelegate> delegate;
@end

NS_ASSUME_NONNULL_END
