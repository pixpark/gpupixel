//
//  FilterToolbarView.h
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class FilterToolbarView;

@protocol FilterToolbarViewDelegate <NSObject>

@optional
- (void)filterToolbarView:(FilterToolbarView *)toolbarView didSelectFilterAtIndex:(NSInteger)index;
- (void)filterToolbarView:(FilterToolbarView *)toolbarView didChangeSliderValue:(CGFloat)value;
- (void)filterToolbarView:(FilterToolbarView *)toolbarView didChangeEffectSwitchState:(BOOL)isOn;

@end

@interface FilterToolbarView : UIView

@property (nonatomic, strong, readonly) UISwitch *effectSwitch;
@property (nonatomic, weak) id<FilterToolbarViewDelegate> delegate;
@property (nonatomic, readonly) NSInteger selectedFilterIndex;

- (instancetype)initWithFrame:(CGRect)frame filterTitles:(NSArray<NSString *> *)filterTitles;

- (void)selectFilterAtIndex:(NSInteger)index animated:(BOOL)animated;
- (void)updateSliderValue:(CGFloat)value;

@end

NS_ASSUME_NONNULL_END
