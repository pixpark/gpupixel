/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import <UIKit/UIKit.h>
/// 指示器位置
typedef NS_ENUM(NSUInteger, WNSegmentedControlIndicatorLocate) {
  WNSegmentedControlIndicatorLocateTop,
  WNSegmentedControlIndicatorLocateBottom,
};

/// 指示器宽度风格
typedef NS_ENUM(NSUInteger, WNSegmentedControlIndicatorWidthStyle) {
  WNSegmentedControlIndicatorWidthStyleFull,
  WNSegmentedControlIndicatorWidthStyleText,
  WNSegmentedControlIndicatorWidthStyleShort,
};

typedef NS_ENUM(NSInteger, WNSegmentedControlWidthStyle) {
  WNSegmentedControlWidthStyleFixed,    // 平均分割
  WNSegmentedControlWidthStyleDynamic,  // 同字体宽度
};

NS_ASSUME_NONNULL_BEGIN

@interface PanelSegmentControl : UIControl

///---------------------
/// @name Initialization
///---------------------
- (instancetype)initWithTitles:(NSArray<NSString*>*)titles;
- (instancetype)initWithImages:(NSArray<UIImage*>*)images;
- (instancetype)initWithImages:(NSArray<UIImage*>*)images
                selectedImages:(nullable NSArray<UIImage*>*)selectedImages;
- (instancetype)initWithAttributedTitles:(NSArray<NSAttributedString*>*)titles;
- (nullable NSString*)titleAtIndex:(NSUInteger)index;
- (nullable UIImage*)imageAtIndex:(NSUInteger)index;
- (void)setTitle:(NSString*)title atIndex:(NSUInteger)index;
- (void)setImage:(UIImage*)image atIndex:(NSUInteger)index;
- (void)setImage:(UIImage*)image
   selectedImage:(nullable UIImage*)selectedImage
         atIndex:(NSUInteger)index;

- (void)insertTitle:(NSString*)title atIndex:(NSUInteger)index;
- (void)removeTitleAtIndex:(NSUInteger)index;
- (void)setTitles:(NSArray<NSString*>*)titles;

- (void)setTextAttributes:(nullable NSDictionary*)attributes
                 forState:(UIControlState)state;

- (void)setSelectedSegmentIndex:(NSUInteger)selectedSegmentIndex
                   ignoreAction:(BOOL)ignoreAction;

- (void)addTipViewWithGenerator:(UIView* (^)(NSUInteger index))viewGenerator;

- (void)reloadData;

@property(nonatomic, assign, readonly) NSUInteger numberOfSegments;

@property(nonatomic, assign) NSUInteger selectedSegmentIndex;

@property(nonatomic, assign) WNSegmentedControlWidthStyle widthStyle;

@property(nonatomic, readwrite) UIEdgeInsets segmentEdgeInset;

/**
 The padding(including left and right) at which the origin of the contents is
 offset from the origin of the segmented control. Default is 0.
 */
@property(nonatomic, assign) CGFloat horizontalPadding;

/**
 A Boolean value that controls whether the top separator is visible. Default is
 `NO`.
 */
@property(nonatomic, assign, getter=isShowsTopSeparator) BOOL showsTopSeparator;

/**
 A Boolean value that controls whether the item separator is visible. Default is
 `NO`.
 */
@property(nonatomic, assign, getter=isShowsItemSeparator)
BOOL showsItemSeparator;

/**
 A Boolean value that controls whether the bottom separator is visible. Default
 is `NO`.
 */
@property(nonatomic, assign, getter=isShowsBottomSeparator)
BOOL showsBottomSeparator;

/**
 A Boolean value that controls whether the bottom shadow is visible. Default is
 `NO`.
 */
@property(nonatomic, assign, getter=isShowBottomShadow) BOOL showBottomShadow;

/**
 A Boolean value that controls whether the divider is visible between each
 segment. Default is `NO`.
 */
@property(nonatomic, assign, getter=isShowsVerticalDivider)
BOOL showsVerticalDivider;

/**
 A Boolean value that controls whether the indicator is visible. Default is
 `YES`.
 */
@property(nonatomic, assign, getter=isShowsIndicator) BOOL showsIndicator;

/**
 The width style of indicator. Default is full.
 */
@property(nonatomic, assign)
WNSegmentedControlIndicatorWidthStyle indicatorWidthStyle;

/**
 * Default is YES. Set to NO to disable animation during user selection.
 */
@property(nonatomic) BOOL indicatorAnimate;

/**
 * Default is NO. Set to NO to disable animation during user selection.
 */
@property(nonatomic) BOOL textAnimate;

/**
 The height of the indicator. The default is 2.0.
 */
@property(nonatomic, assign) CGFloat indicatorHeight;

/**
 The background color of the `YCSegmentedControl`. The default is yche blue.
 */
@property(nonatomic, strong) UIColor* indicatorBackgroundColor;

/**
 The location of indicator. Could be top or bottom. Default is bottom.
 */
@property(nonatomic, assign) WNSegmentedControlIndicatorLocate indicatorLocate;

/**
 滑动时显示左右两侧渐变效果
 */
@property(nonatomic, assign, getter=isShowGradient) BOOL showGradient;

@end

NS_ASSUME_NONNULL_END
