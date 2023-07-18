/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */


#import "PanelSegmentControl.h"
#import "PanelSegmentItem.h"


#define IsArray(__array) ([(__array) isKindOfClass:[NSArray class]])
#define IsArrayWithAnyItem(__array) (IsArray(__array) && ([((NSArray *)__array) count] > 0))
#define SCREENWidth     [UIScreen mainScreen].bounds.size.width
#define SCREENHeight [UIScreen mainScreen].bounds.size.height

typedef PanelSegmentItem Item;

typedef NS_ENUM(NSUInteger, WNSegmentedControlContentType) {
    WNSegmentedControlContentTypeText,
    WNSegmentedControlContentTypeImage,
    WNSegmentedControlContentTypeAttributeText,
    
};

static const NSTimeInterval kAnimationDuration = 0.25;
static const CGFloat        kIndicatorDefaultHeight = 3.0;
static const CGFloat        kSegmentWidthMinmum = 48.0;
static const NSUInteger     WNSegmentTag = 666;

CGFloat kSeparatorDefaultHeight() {
    return 0.5;
}

@interface WNSegmentedScrollView : UIScrollView
@end

@interface PanelSegmentControl ()<UIScrollViewDelegate>
@property (nonatomic, weak) WNSegmentedScrollView *contentContainer;
@property (nonatomic, weak) CALayer *separatorTop;
@property (nonatomic, weak) CALayer *separatorBottom;
@property (nonatomic, weak) CALayer *bottomShadow;
@property (nonatomic, weak) CAGradientLayer *gradientLayerLeft;
@property (nonatomic, weak) CAGradientLayer *gradientLayerRight;

@property (nonatomic, strong, readwrite) UIView *indicator;
@property (nonatomic, readwrite) CGFloat segmentWidth;

@property (nonatomic, copy) NSDictionary *attributesNormal;
@property (nonatomic, copy) NSDictionary *attributesSelected;

@property (nonatomic, assign) WNSegmentedControlContentType contentType;
@property (nonatomic, copy) NSArray *contents;                      // text or images as content
@property (nonatomic, copy) NSArray<UIImage *> *selectedImages;
@property (nonatomic, strong) NSMutableArray<Item *> *items;        // labels or imageViews as elements

@property (nonatomic, strong) NSMutableArray<NSNumber *> *segmenetWidths;
@property (nonatomic, strong) NSMutableArray<NSLayoutConstraint *> *widthConstraints;
@end

@implementation PanelSegmentControl

- (instancetype)initWithTitles:(NSArray <NSString *> *)titles {
    self = [super initWithFrame:CGRectZero];
    if (self) {
        [self copyTitles:titles];
        _contentType = WNSegmentedControlContentTypeText;
        _attributesNormal = @{NSFontAttributeName: [UIFont systemFontOfSize:16.0],
                              NSForegroundColorAttributeName: [UIColor blackColor]};
        _attributesSelected = [_attributesNormal copy];
        
        [self commonInit];
    }
    return self;
}
- (instancetype)initWithAttributedTitles:(NSArray <NSAttributedString *> *)titles {
    self = [super initWithFrame:CGRectZero];
    if (self) {
        [self copyAttributeTitles:titles];
        _contentType = WNSegmentedControlContentTypeAttributeText;
        _attributesNormal = @{NSFontAttributeName: [UIFont systemFontOfSize:16.0],
                              NSForegroundColorAttributeName: [UIColor blackColor]};
        _attributesSelected = [_attributesNormal copy];
        
        [self commonInit];
    }
    return self;
}

- (instancetype)initWithImages:(NSArray <UIImage *> *)images {
    return [self initWithImages:images selectedImages:nil];
}

- (instancetype)initWithImages:(NSArray <UIImage *> *)images
                selectedImages:(NSArray <UIImage *> *)selectedImages {
    self = [super initWithFrame:CGRectZero];
    if (self) {
        _contents = [images copy];
        _contentType = WNSegmentedControlContentTypeImage;
        if (selectedImages) {
            NSAssert([images count] == [selectedImages count], @"[Segment]: ERROR the count of parameter images is not equal to the count of parameter selectedImages.");
            _selectedImages = [selectedImages copy];
        }
        
        [self commonInit];
    }
    return self;
}

- (CGSize)intrinsicContentSize {
    return UILayoutFittingExpandedSize;
}

- (void)commonInit {
    self.backgroundColor = [UIColor clearColor];
    self.showsTopSeparator = NO;
    self.showsBottomSeparator = NO;
    self.showsIndicator = NO;
    _indicatorHeight = kIndicatorDefaultHeight;
    _indicatorLocate = WNSegmentedControlIndicatorLocateBottom;
    _indicatorWidthStyle = WNSegmentedControlIndicatorWidthStyleFull;
    _widthStyle = WNSegmentedControlWidthStyleFixed;
    _showsVerticalDivider = NO;
    _selectedSegmentIndex = 0;
    _horizontalPadding = 0.0;
    _segmentEdgeInset = UIEdgeInsetsMake(0, 5, 0, 5);
    _numberOfSegments = [_contents count];
    _items = [NSMutableArray array];
    _widthConstraints = @[].mutableCopy;
    _indicatorAnimate = NO;
    _textAnimate = NO;
    _showGradient = NO;
    _segmentWidth = 0;
    
    // setup views
    [self setupViews];
}

#pragma mark - Layout

- (void)layoutSubviews {
    [super layoutSubviews];
    if (!IsArrayWithAnyItem(self.contents)) {
        return;
    }
    // separator
    if (_showsTopSeparator) {
        CGRect frame = (CGRect){0, 0, CGRectGetWidth(self.bounds), kSeparatorDefaultHeight()};
        _separatorTop.frame = frame;
    }
    if (_showsBottomSeparator) {
        CGRect frame = (CGRect){0, CGRectGetHeight(self.bounds) - kSeparatorDefaultHeight(), CGRectGetWidth(self.bounds), kSeparatorDefaultHeight()};
        _separatorBottom.frame = frame;
    }
    if (_showBottomShadow) {
        _bottomShadow.frame = self.bounds;
    }
    if (_showGradient) {
        _gradientLayerLeft.frame = (CGRect){CGPointZero, CGSizeMake(28, self.frame.size.height)};
        _gradientLayerRight.frame = CGRectMake(self.frame.size.width - 28, 0, 28, self.frame.size.height);
    }
    
    // indicator
    if (!_indicator) {
        return;
    }
    self.indicator.frame = [self indicatorFrame];
    self.indicator.layer.cornerRadius = self.indicatorHeight / 2;
    
    if (self.indicator.superview == nil && self.showsIndicator) {
        [self.contentContainer addSubview:self.indicator];
    }
}

#pragma mark - Private

- (void)copyTitles:(NSArray <NSString *> *)titles {
    NSMutableArray *contents = [NSMutableArray array];
    for (NSString *title in titles) {
        [contents addObject:[title copy]];
    }
    self.contents = contents;
}

- (void)copyAttributeTitles:(NSArray <NSAttributedString *> *)titles {
    NSMutableArray *contents = [NSMutableArray array];
    for (NSAttributedString *title in titles) {
        [contents addObject:[title copy]];
    }
    self.contents = contents;
}

- (void)segmentDidSelectAtIndex:(NSUInteger)newIndex didDeselectAtIndex:(NSUInteger)oldIndex ignoreAction:(BOOL)ignoreAction {
    _selectedSegmentIndex = newIndex;
    
    if (!ignoreAction) {
        [self sendActionsForControlEvents:UIControlEventValueChanged];
    }
    if (newIndex == oldIndex) {
        return;
    }
    
    [self scrollToSelectedSegmentIndex];
    // update UI
    if (_contentType == WNSegmentedControlContentTypeText) {
        UILabel *selectedLabel = _items[newIndex].label;
        NSMutableAttributedString *mutableAttributed = [[NSMutableAttributedString alloc] initWithString:_contents[newIndex]  attributes:_attributesSelected];
        
        selectedLabel.attributedText = mutableAttributed;
        
        UILabel *deselectedLabel = _items[oldIndex].label;
        NSMutableAttributedString *deselectedMutableAttributed = [[NSMutableAttributedString alloc] initWithString:_contents[oldIndex]  attributes:_attributesNormal];
       
        deselectedLabel.attributedText = deselectedMutableAttributed;
        
        if (self.textAnimate) {
            [self textAnimationFromIndex:oldIndex toIndex:newIndex];
        }
    } else if (_contentType == WNSegmentedControlContentTypeAttributeText){
        UILabel *selectedLabel = _items[newIndex].label;
        selectedLabel.attributedText = _contents[newIndex];
        if (self.textAnimate) {
            [self textAnimationFromIndex:oldIndex toIndex:newIndex];
        }
    }
    else {
        if (_selectedImages) {
            UIImageView *selectedImageView = _items[newIndex].imageView;
            selectedImageView.image = _selectedImages[newIndex];
            UIImageView *deselectedImageView = _items[oldIndex].imageView;
            deselectedImageView.image = _contents[oldIndex];
        }
    }
    
    [self moveIndicatorFromIndex:oldIndex toIndex:newIndex];
}

- (void)moveIndicatorFromIndex:(NSInteger)fromIndex toIndex:(NSInteger)toIndex {
    CGRect frame = [self indicatorFrame];
    if (!self.indicatorAnimate) {
        _indicator.frame = frame;
        return;
    }
    
    // indicator animate
    __weak typeof(self) weakSelf = self;
    [UIView animateWithDuration:kAnimationDuration
                          delay:0.0
         usingSpringWithDamping:0.66
          initialSpringVelocity:3.0
                        options:UIViewAnimationOptionCurveLinear
                     animations:^{
                         weakSelf.indicator.frame = frame;
                     } completion:^(BOOL finished) {
                     }];
}

- (void)textAnimationFromIndex:(NSUInteger)fromIndex toIndex:(NSUInteger)toIndex {
    UIFont *normalFont = self.attributesNormal[NSFontAttributeName];
    UIFont *selectedFont = self.attributesSelected[NSFontAttributeName];
    CGFloat scale = selectedFont.pointSize / normalFont.pointSize;
    UILabel *selectedButton = self.items[toIndex].label;
    UILabel *preButton = self.items[fromIndex].label;
    
    CGAffineTransform oldTransform = selectedButton.transform;
    CGAffineTransform preTansform = preButton.transform;
    selectedButton.transform = CGAffineTransformScale(selectedButton.transform, 1 / scale, 1 / scale);
    preButton.transform = CGAffineTransformScale(preButton.transform, scale, scale);
    [UIView animateWithDuration:0.15 animations:^{
        selectedButton.transform = oldTransform;
        preButton.transform = preTansform;
    } completion:^(BOOL finished) {
        
    }];
}

#pragma mark - Scroll

- (void)scrollToSelectedSegmentIndex {
    CGRect rectForSelectedIndex = CGRectZero;
    CGFloat selectedSegmentOffset = 0;
    
    if (self.widthStyle == WNSegmentedControlWidthStyleFixed) {
        rectForSelectedIndex = CGRectMake(self.segmentWidth * self.selectedSegmentIndex,
                                          0,
                                          self.segmentWidth,
                                          self.frame.size.height);
        
        
        selectedSegmentOffset = CGRectGetWidth(self.frame) / 2 - self.segmentWidth / 2;
    } else {
        NSInteger i = 0;
        CGFloat offsetter = 0;
        for (NSNumber *width in self.segmenetWidths) {
            if (self.selectedSegmentIndex == i)
                break;
            offsetter = offsetter + width.floatValue;
            i++;
        }
        
        rectForSelectedIndex = CGRectMake(offsetter,
                                          0,
                                          self.segmenetWidths[self.selectedSegmentIndex].floatValue,
                                          self.frame.size.height);
        
        selectedSegmentOffset = CGRectGetWidth(self.frame) / 2 - self.segmenetWidths[self.selectedSegmentIndex].floatValue / 2;
    }
    
    CGRect rectToScrollTo = rectForSelectedIndex;
    rectToScrollTo.origin.x -= selectedSegmentOffset;
    rectToScrollTo.size.width += selectedSegmentOffset * 2;
    [self.contentContainer scrollRectToVisible:rectToScrollTo animated:YES];
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView {
    CGFloat offsetX = scrollView.contentOffset.x;
    CGFloat gradientWidth = self.gradientLayerLeft.frame.size.width;
    CGFloat leftRadius = offsetX / gradientWidth;
    CGFloat rightRadius = (scrollView.contentSize.width - offsetX - self.frame.size.width) / gradientWidth;
    
    self.gradientLayerLeft.opacity = MIN(leftRadius, 1);
    self.gradientLayerRight.opacity = MIN(rightRadius, 1);
}

#pragma mark -

- (NSString *)titleAtIndex:(NSUInteger)index {
    if (_contentType == WNSegmentedControlContentTypeImage) {
        NSLog(@"[Segment]: WARN use -imageAtIndex: instead.");
        return nil;
    } else {
        NSAssert(index < _numberOfSegments, @"[Segment]: ERROR Index must be a number between 0 and the number of segments (numberOfSegments) minus 1.");
        return (NSString *)_contents[index];
    }
}

- (UIImage *)imageAtIndex:(NSUInteger)index {
    if (_contentType == WNSegmentedControlContentTypeText) {
        NSLog(@"[Segment]: WARN use -titleAtIndex: instead.");
        return nil;
    } else {
        NSAssert(index < _numberOfSegments, @"[Segment]: ERROR Index must be a number between 0 and the number of segments (numberOfSegments) minus 1.");
        return (UIImage *)_contents[index];
    }
}

- (void)setTitle:(NSString *)title atIndex:(NSUInteger)index {
    if (_contentType == WNSegmentedControlContentTypeImage) {
        NSLog(@"[Segment]: WARN use setImage:atIndex: instead.");
        return;
    }
    NSAssert(title, @"[Segment]: ERROR Title cannot be nil.");
    NSAssert(index < _numberOfSegments, @"[Segment]: ERROR Index must be a number between 0 and the number of segments (numberOfSegments) minus 1.");
    // update contents
    NSMutableArray *contents = [_contents mutableCopy];
    contents[index] = [title copy];
    self.contents = contents;
    
    // update UI
    UILabel *label = _items[index].label;
    NSMutableAttributedString *string = [label.attributedText mutableCopy];
    [string.mutableString setString:title];
    label.attributedText = string;
}

- (void)setImage:(UIImage *)image atIndex:(NSUInteger)index {
    [self setImage:image selectedImage:nil atIndex:index];
}

- (void)setImage:(UIImage *)image selectedImage:(UIImage *)selectedImage atIndex:(NSUInteger)index {
    if (_contentType == WNSegmentedControlContentTypeText) {
        NSLog(@"[Segment]: WARN use setTitle:atIndex: instead.");
        return;
    }
    NSAssert(image, @"[Segment]: ERROR image cannot be nil.");
    NSAssert(index < _numberOfSegments, @"[Segment]: ERROR Index must be a number between 0 and the number of segments (numberOfSegments) minus 1.");
    
    NSMutableArray *contents = [_contents mutableCopy];
    contents[index] = image;
    self.contents = contents;
    
    UIImageView *imageView = _items[index].imageView;
    
    if (selectedImage) {
        NSMutableArray *selectedImages = [_selectedImages mutableCopy];
        selectedImages[index] = selectedImage;
        self.selectedImages = selectedImages;
        
        if (_selectedSegmentIndex == index) {
            imageView.image = selectedImage;
        } else {
            imageView.image = image;
        }
    }
    else {
        imageView.image = image;
    }
}

- (void)insertTitle:(NSString *)title atIndex:(NSUInteger)index {
    if (index >= _numberOfSegments) {
        return;
    }
    if (!title) {
        return;
    }
    
    NSMutableArray *titles = self.contents.mutableCopy;
    [titles insertObject:title atIndex:index];
    self.contents = titles.copy;
    [self reloadData];
}

- (void)removeTitleAtIndex:(NSUInteger)index {
    if (index >= _numberOfSegments) {
        return;
    }
    
    NSMutableArray *titles = self.contents.mutableCopy;
    [titles removeObjectAtIndex:index];
    self.contents = titles.copy;
    [self reloadData];
}

- (void)setTitles:(NSArray<NSString *> *)titles {
    self.contents = titles.copy;
}


- (void)reloadData {
    _numberOfSegments = self.contents.count;
    _selectedSegmentIndex = 0;
   // [_contentContainer removeAllSubviews];
    [_contentContainer removeFromSuperview];
    _contentContainer = nil;
    [_widthConstraints removeAllObjects];
    [_items removeAllObjects];
    [self setupViews];
    [self updateRects];
    [self setNeedsDisplay];
    [self setNeedsLayout];
    [self layoutIfNeeded];
}

- (void)setTextAttributes:(NSDictionary *)attributes forState:(UIControlState)state {
    if (state == UIControlStateNormal) {
        self.attributesNormal = attributes;
    } else {
        self.attributesSelected = attributes;
    }
    [self updateRects];
}


#pragma mark - add extra view

- (void)addTipViewWithGenerator:(UIView *(^)(NSUInteger index))viewGenerator {
    if (viewGenerator != nil) {
        for (NSUInteger index = 0; index < _items.count; index++) {
            UIView *view = viewGenerator(index);
            if (view != nil) {
                Item *item = _items[index];
                [item addTipView:view];
            }
        }
    }
}

#pragma mark - Touch

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    CGPoint touchLocation = [touches.anyObject locationInView:self];
    
    if (CGRectContainsPoint(self.bounds, touchLocation)) {
        
        NSInteger toIndex = 0;
        if (self.widthStyle == WNSegmentedControlWidthStyleFixed) {
            toIndex = (touchLocation.x + self.contentContainer.contentOffset.x) / self.segmentWidth;
        } else if (self.widthStyle == WNSegmentedControlWidthStyleDynamic) {
            
            CGFloat widthLeft = (touchLocation.x + self.contentContainer.contentOffset.x);
            for (NSNumber *width in self.segmenetWidths) {
                widthLeft = widthLeft - width.floatValue;
                
                if (widthLeft <= 0)
                    break;
                toIndex++;
            }
        }
        
        if (toIndex != NSNotFound && toIndex < self.numberOfSegments) {
            if (_selectedSegmentIndex != toIndex) {
                [self segmentDidSelectAtIndex:toIndex didDeselectAtIndex:_selectedSegmentIndex ignoreAction:NO];
            } else {
                [self sendActionsForControlEvents:UIControlEventTouchUpInside];
            }
        }
    }
}

#pragma mark - Draw

- (void)drawRect:(CGRect)rect {
    if (_contentType == WNSegmentedControlContentTypeText) {
        for (int i = 0; i < _numberOfSegments; i++) {
            UILabel *label = _items[i].label;
            if (i == _selectedSegmentIndex) {
                NSMutableAttributedString *mutableAttributed = [[NSMutableAttributedString alloc] initWithString:_contents[i]  attributes:_attributesSelected];
               
                label.attributedText = mutableAttributed;
            } else {
                NSMutableAttributedString *mutableAttributed = [[NSMutableAttributedString alloc] initWithString:_contents[i]  attributes:_attributesNormal];
               
                label.attributedText = mutableAttributed;
            }
        }
    }
    else {
        if (_selectedImages) {
            UIImageView *imageView = _items[_selectedSegmentIndex].imageView;
            imageView.image = _selectedImages[_selectedSegmentIndex];
        }
    }
}

- (CGFloat)totalSegmentedControlWidth {
    if (self.widthStyle == WNSegmentedControlWidthStyleFixed) {
        return self.numberOfSegments * self.segmentWidth;
    } else if (self.contentType == WNSegmentedControlContentTypeText) {
        return [[self.segmenetWidths valueForKeyPath:@"@sum.self"] floatValue];
    } else {
        return self.numberOfSegments * self.segmentWidth;
    }
}

- (CGRect)indicatorFrame {
    CGFloat x = 0, y = 0, width = 0;
    CGFloat height = self.indicatorHeight;
    
    if (self.contentType == WNSegmentedControlContentTypeText &&
        (self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleText ||
         self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort)) {
            
            width = self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort ? 20 :  [self measureTitleAtIndex:self.selectedSegmentIndex].width;
            
            if (self.widthStyle == WNSegmentedControlWidthStyleDynamic) {
                __block CGFloat left = 0;
                [self.segmenetWidths enumerateObjectsUsingBlock:^(NSNumber * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
                    if (idx < self.selectedSegmentIndex) {
                        left += obj.floatValue;
                    } else {
                        *stop = YES;
                    }
                }];
                if (self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort) {
                    x = left + (self.segmenetWidths[self.selectedSegmentIndex].floatValue - width) / 2;
                } else {
                    x = left + self.segmentEdgeInset.left;
                }
            } else {
                CGFloat segmentWidth = self.segmentWidth;
                x = _horizontalPadding + (segmentWidth - width) / 2 + segmentWidth * self.selectedSegmentIndex;
            }
        }
    else {
        if (self.widthStyle == WNSegmentedControlWidthStyleDynamic) {
            width = self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort ? 20 : self.segmenetWidths[self.selectedSegmentIndex].floatValue;
            
            __block CGFloat left = 0;
            [self.segmenetWidths enumerateObjectsUsingBlock:^(NSNumber * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
                if (idx < self.selectedSegmentIndex) {
                    left += obj.floatValue;
                } else {
                    *stop = YES;
                }
            }];
            
            x = left + _horizontalPadding;
            if (self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort) {
                x += (self.segmenetWidths[self.selectedSegmentIndex].floatValue - width) / 2;
            }
        } else {
            width = self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort ? 20 : self.segmentWidth;
            x = _horizontalPadding + width * _selectedSegmentIndex;
            if (self.indicatorWidthStyle == WNSegmentedControlIndicatorWidthStyleShort) {
                x += (self.segmentWidth - width) / 2;
            }
        }
    }
    
    switch (self.indicatorLocate) {
        case WNSegmentedControlIndicatorLocateBottom: {
            y = CGRectGetHeight(self.contentContainer.frame) - height;
            if (_showsBottomSeparator) y -= kSeparatorDefaultHeight();
            break;
        }
        case WNSegmentedControlIndicatorLocateTop: {
            if (_showsTopSeparator) y += kSeparatorDefaultHeight();
            break;
        }
    }
    
    return (CGRect){x, y, width, height};
}

- (CGSize)measureTitleAtIndex:(NSUInteger)index {
    if (index >= self.contents.count) {
        return CGSizeZero;
    }
    NSString *title = self.contents[index];
    
    BOOL selected = index == self.selectedSegmentIndex;
    NSDictionary *titleAttributes = selected ? self.attributesSelected : self.attributesNormal;
    CGSize size = [title sizeWithAttributes:titleAttributes];
    return CGRectIntegral((CGRect){CGPointZero, size}).size;
}

- (void)updateRects {
    if (self.numberOfSegments == 0) {
        return;
    }
    if (self.widthStyle == WNSegmentedControlWidthStyleFixed) {
        self.segmentWidth = MAX(CGRectGetWidth(self.bounds) / self.numberOfSegments, kSegmentWidthMinmum);
        for (NSLayoutConstraint *constraint in self.widthConstraints) {
            constraint.constant = self.segmentWidth;
        }
    } else if (self.contentType == WNSegmentedControlContentTypeText) {
        NSMutableArray *temp = @[].mutableCopy;
        [self.contents enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            CGFloat width = [self measureTitleAtIndex:idx].width + self.segmentEdgeInset.left + self.segmentEdgeInset.right;
            [temp addObject:@(width)];
            self.widthConstraints[idx].constant = width;
        }];
        self.segmenetWidths = [NSMutableArray arrayWithArray:temp];
    }
    self.contentContainer.contentSize = CGSizeMake([self totalSegmentedControlWidth], CGRectGetHeight(self.bounds));
}

#pragma mark - Setter

- (void)setFrame:(CGRect)frame {
    [super setFrame:frame];
    if (CGRectIsEmpty(frame)) {
        return;
    }
    [self updateRects];
}

- (void)setWidthStyle:(WNSegmentedControlWidthStyle)widthStyle {
    _widthStyle = widthStyle;
    [self updateRects];
}

- (void)setShowsVerticalDivider:(BOOL)showsVerticalDivider {
    
    if (_showsVerticalDivider != showsVerticalDivider) {
        _showsVerticalDivider = showsVerticalDivider;
        
        // update divider
        if (showsVerticalDivider) {
            for (int i = 1; i < _numberOfSegments; i++) {
                [_items[i] showVerticalDivider];
            }
        }
        else {
            for (int i = 1; i < _numberOfSegments; i++) {
                [_items[i] hideVerticalDivider];
            }
        }
    }
}

- (void)setShowsTopSeparator:(BOOL)showsTopSeparator {
    
    if (_showsTopSeparator != showsTopSeparator) {
        _showsTopSeparator = showsTopSeparator;
        
        // setup separator top
        if (showsTopSeparator) {
            CALayer *separatorTop = ({
                CALayer *layer = [CALayer layer];
                layer.backgroundColor = [UIColor blueColor].CGColor;
                [self.layer addSublayer:layer];
                
                layer;
            });
            self.separatorTop = separatorTop;
            [self setNeedsLayout];
        } else {
            if (_separatorTop) {
                [_separatorTop removeFromSuperlayer];
            }
        }
    }
}

- (void)setShowsBottomSeparator:(BOOL)showsBottomSeparator {
    
    if (_showsBottomSeparator != showsBottomSeparator) {
        _showsBottomSeparator = showsBottomSeparator;
        
        // setup separator bottom
        if (showsBottomSeparator) {
            CALayer *separatorBottom = ({
                CALayer *layer = [CALayer layer];
                layer.backgroundColor = [UIColor whiteColor].CGColor;
                [self.layer addSublayer:layer];
                
                //                layer.shadowColor = UIColorWithRGBA(0x7F8DA0, 0.06).CGColor;
                layer.shadowOpacity = 1;
                layer.shadowRadius = 5;
                layer.shadowOffset = CGSizeMake(0, 3);
                
                layer;
            });
            self.separatorBottom = separatorBottom;
            [self setNeedsLayout];
        } else {
            if (_separatorBottom) {
                [_separatorBottom removeFromSuperlayer];
            }
        }
    }
}

- (void)setShowBottomShadow:(BOOL)showBottomShadow {
    if (_showBottomShadow != showBottomShadow) {
        _showBottomShadow = showBottomShadow;
        
        if (showBottomShadow) {
            CALayer *bottomShadow = ({
                CALayer *layer = [CALayer layer];
                layer.backgroundColor = [UIColor whiteColor].CGColor;
                [self.layer insertSublayer:layer atIndex:0];
                
                //layer.shadowColor = UIColorWithRGBA(0x7F8DA0, 0.06).CGColor;
                layer.shadowOpacity = 1;
                layer.shadowRadius = 5;
                layer.shadowOffset = CGSizeMake(0, 3);
                
                layer;
            });
            self.bottomShadow = bottomShadow;
            [self setNeedsLayout];
        } else {
            if (_bottomShadow) {
                [_bottomShadow removeFromSuperlayer];
            }
        }
        
    }
}

- (void)setShowsIndicator:(BOOL)showsIndicator {
    
    if (_showsIndicator != showsIndicator) {
        _showsIndicator = showsIndicator;
        
        // setup indicator
        if (showsIndicator) {
            _indicator = ({
                UIView *indicator = [UIView new];
                indicator.backgroundColor = [UIColor redColor];
                [self.contentContainer addSubview:indicator];
                
                indicator;
            });
            [self setNeedsLayout];
        } else {
            if (_indicator) {
                [_indicator removeFromSuperview];
                _indicator = nil;
            }
        }
    }
}

- (void)setShowGradient:(BOOL)showGradient {
    if (_showGradient == showGradient) {
        return;
    }
    _showGradient = showGradient;
    if (showGradient) {
        CAGradientLayer *left = ({
            CAGradientLayer *left = [CAGradientLayer layer];
            left.colors = @[(__bridge id)[UIColor whiteColor].CGColor,
                            (__bridge id)[[UIColor whiteColor] colorWithAlphaComponent:0].CGColor];
            left.locations = @[@.5, @1];
            left.startPoint = CGPointMake(0, 0.5);
            left.endPoint = CGPointMake(1, 0.5);
            left.opacity = 0;
            [self.layer addSublayer:left];
            left;
        });
        CAGradientLayer *right = ({
            CAGradientLayer *right = [CAGradientLayer layer];
            right.colors = @[(__bridge id)[UIColor whiteColor].CGColor,
                             (__bridge id)[[UIColor whiteColor] colorWithAlphaComponent:0].CGColor];
            right.locations = @[@.5, @1];
            right.startPoint = CGPointMake(1, 0.5);
            right.endPoint = CGPointMake(0, 0.5);
            right.opacity = 1;
            [self.layer addSublayer:right];
            right;
        });
        _gradientLayerLeft = left;
        _gradientLayerRight = right;
        [self setNeedsLayout];
    } else {
        if (_gradientLayerLeft) {
            [_gradientLayerLeft removeFromSuperlayer];
            [_gradientLayerRight removeFromSuperlayer];
            _gradientLayerLeft = nil;
            _gradientLayerRight = nil;
        }
    }
}

- (void)setHorizontalPadding:(CGFloat)horizontalPadding {
    _horizontalPadding = horizontalPadding;
    // update constraints
    for (NSLayoutConstraint *constraint in self.constraints) {
        if (constraint.firstAttribute == NSLayoutAttributeLeading ||
            constraint.firstAttribute == NSLayoutAttributeTrailing) {
            constraint.constant = horizontalPadding;
        }
    }
}

- (void)setIndicatorWidthStyle:(WNSegmentedControlIndicatorWidthStyle)indicatorWidthStyle {
    _indicatorWidthStyle = indicatorWidthStyle;
    [self setNeedsLayout];
}

- (void)setIndicatorBackgroundColor:(UIColor *)indicatorBackgroundColor {
    _indicatorBackgroundColor = indicatorBackgroundColor;
    self.indicator.backgroundColor = indicatorBackgroundColor;
}

- (void)setSelectedSegmentIndex:(NSUInteger)selectedSegmentIndex {
    [self setSelectedSegmentIndex:selectedSegmentIndex ignoreAction:YES];
}

- (void)setSelectedSegmentIndex:(NSUInteger)selectedSegmentIndex ignoreAction:(BOOL)ignoreAction {
    if (selectedSegmentIndex >= self.contents.count) {
        return;
    }
    [self segmentDidSelectAtIndex:selectedSegmentIndex didDeselectAtIndex:_selectedSegmentIndex ignoreAction:ignoreAction];
}

#pragma mark - Getter

- (UIView *)indicator {
    if (_indicator) {
        return _indicator;
    }
    NSLog(@"[Segment]: WARN indicator is nil.");
    
    return nil;
}

#pragma mark - Setup views

- (void)setupViews {
    
    // prepare content views
    if (_contentType == WNSegmentedControlContentTypeText || _contentType == WNSegmentedControlContentTypeAttributeText) {
        // content of segmented control is text
        for (int i = 0; i < _numberOfSegments; i++) {
            UILabel *label = [UILabel new];
            label.textAlignment = NSTextAlignmentCenter;
            label.numberOfLines = 1;
            label.translatesAutoresizingMaskIntoConstraints = NO;
            [_items addObject:[[Item alloc] initWithView:label]];
        }
    }
    else {
        // content of segmented control is image
        for (int i = 0; i < _numberOfSegments; i++) {
            UIImageView *imageView = [[UIImageView alloc] initWithImage:_contents[i]];
            imageView.contentMode = UIViewContentModeCenter;
            imageView.translatesAutoresizingMaskIntoConstraints = NO;
            [_items addObject:[[Item alloc] initWithView:imageView]];
        }
    }
    
    // setup container
   WNSegmentedScrollView *container = ({
        WNSegmentedScrollView *container = [[WNSegmentedScrollView alloc] initWithFrame:CGRectMake(0, 0, SCREENWidth, 30)];
        container.clipsToBounds = NO;
        container.scrollsToTop = NO;
        container.showsVerticalScrollIndicator = NO;
        container.showsHorizontalScrollIndicator = NO;
        container.translatesAutoresizingMaskIntoConstraints = NO;
        container.delegate = self;
        [self addSubview:container];
        
        // layout view
        NSDictionary *views = @{@"container": container};
        NSArray *h = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[container]|" options:0 metrics:nil views:views];
        for (NSLayoutConstraint *constraint in h) {
            constraint.constant = self.horizontalPadding;
        }
        NSArray *w = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[container]|" options:0 metrics:nil views:views];
        [self addConstraints:h];
        [self addConstraints:w];
        container;
    });
    self.contentContainer = container;
    
    // setup segment views
    id lastView;
    NSMutableArray *constrains = @[].mutableCopy;
    for (int i = 0; i < _numberOfSegments; i++) {
        Item *view = _items[i];
        [_contentContainer addSubview:view];
        
        NSLayoutConstraint *top = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeTop relatedBy:NSLayoutRelationEqual toItem:_contentContainer attribute:NSLayoutAttributeTop multiplier:1 constant:0];
        NSLayoutConstraint *height = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:_contentContainer attribute:NSLayoutAttributeHeight multiplier:1 constant:0];
        [constrains addObject:top];
        [constrains addObject:height];
        if (lastView) {
            NSLayoutConstraint *leading = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeLeading relatedBy:NSLayoutRelationEqual toItem:lastView attribute:NSLayoutAttributeTrailing multiplier:1.0 constant:0.0];
            [constrains addObject:leading];
            
            NSLayoutConstraint *width = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:50];
            [self.widthConstraints addObject:width];
        }
        else {
            NSLayoutConstraint *leading = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeLeading relatedBy:NSLayoutRelationEqual toItem:_contentContainer attribute:NSLayoutAttributeLeading multiplier:1.0 constant:0.0];
            [constrains addObject:leading];
            
            NSLayoutConstraint *constraint = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1 constant:50];
            [self.widthConstraints addObject:constraint];
        }
        lastView = view;
    }
    [constrains addObjectsFromArray:self.widthConstraints];
    [_contentContainer addConstraints:constrains];
}

+ (BOOL)requiresConstraintBasedLayout {
    return YES;
}

@end

@implementation WNSegmentedScrollView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!self.dragging) {
        [self.nextResponder touchesBegan:touches withEvent:event];
    } else {
        [super touchesBegan:touches withEvent:event];
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event{
    if (!self.dragging) {
        [self.nextResponder touchesMoved:touches withEvent:event];
    } else{
        [super touchesMoved:touches withEvent:event];
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!self.dragging) {
        [self.nextResponder touchesEnded:touches withEvent:event];
    } else {
        [super touchesEnded:touches withEvent:event];
    }
}

@end
