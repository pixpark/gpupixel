/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */


#import "PanelSegmentItem.h"

@interface PanelSegmentItem ()

@property (nonatomic, weak) CALayer *divider;

@end


@implementation PanelSegmentItem

- (instancetype)initWithView:(UIView *)view {
  self = [super initWithFrame:CGRectZero];
  if (self) {
    self.userInteractionEnabled = YES;
    self.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:view];
    if ([view isKindOfClass:UILabel.self]) {
      _label = (UILabel *)view;
    }
    if ([view isKindOfClass:UIImageView.self]) {
      _imageView = (UIImageView *)view;
    }
    _view = view;
    
    // layout
    NSDictionary *views = @{@"view": view};
    NSArray *h = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|[view]|" options:0 metrics:nil views:views];
    NSArray *w = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|[view]|" options:0 metrics:nil views:views];
    [self addConstraints:w];
    [self addConstraints:h];
  }
  return self;
}

- (void)layoutSubviews {
  [super layoutSubviews];
  
  CALayer *divider = self.divider;
  if (divider) {
    divider.frame = (CGRect){0, 0, 1.0, CGRectGetHeight(self.bounds)};
  }
}

- (void)showVerticalDivider {
  CALayer *divider = ({
    CALayer *layer = [CALayer layer];
    layer.backgroundColor = [UIColor yellowColor].CGColor;
    [self.layer addSublayer:layer];
    
    layer;
  });
  self.divider = divider;
}

- (void)hideVerticalDivider {
  [_divider removeFromSuperlayer];
}

- (void)addTipView:(UIView *)tipView {
  [self addSubview:tipView];
}


@end
