/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */


#import "PanelBeautySlider.h"

@interface PanelBeautySlider ()
/*! @brief slider的thumbView */
@property (nonatomic, strong) UIView *thumbView;
/*! @brief 显示value的label */
@property (nonatomic, strong) UILabel *valueLabel;

@end
@implementation PanelBeautySlider

- (instancetype)initWithFrame:(CGRect)frame {
  
  if (self = [super initWithFrame:frame]) {
    //self.backgroundColor = [UIColor purpleColor];
    [self addTarget:self action:@selector(sliderTouchDown:) forControlEvents:UIControlEventTouchDown];
    [self addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    [self addTarget:self action:@selector(sliderTouchUpInside:) forControlEvents:UIControlEventTouchUpInside];
    [self.thumbView addSubview:self.valueLabel];
    
  }
  return self;
}

#pragma mark - Overwrite functions

- (CGRect)trackRectForBounds:(CGRect)bounds {
  /*! @brief 重写方法-返回进度条的bounds-修改进度条的高度 */
  bounds = [super trackRectForBounds:bounds];
  return CGRectMake(bounds.origin.x, bounds.origin.y + (bounds.size.height - 3.0) / 2, bounds.size.width, 3.0);
}

- (void)setValue:(float)value animated:(BOOL)animated {
  
  [super setValue:value animated:animated];
  [self sliderValueChanged:self];
}

- (void)setValue:(float)value {
  
  [super setValue:value];
  [self sliderValueChanged:self];
}

#pragma mark - Setter functions

- (void)setSliderValue:(NSString *)sliderValue {
  
  if (![_sliderValue isEqualToString:sliderValue]) {
    _sliderValue = sliderValue;
    
    self.valueLabel.text = sliderValue;
    [self.valueLabel sizeToFit];
    
    self.valueLabel.center = CGPointMake(self.thumbView.bounds.size.width / 2, -self.valueLabel.bounds.size.height / 2);
    if (!self.valueLabel.superview) {
      [self.thumbView addSubview:self.valueLabel];
    }
  }
}

#pragma mark - Getter functions

- (UIView *)thumbView {
  if (!_thumbView && self.subviews.count > 2) {
    _thumbView = self.subviews[2];
  }
  if (@available(iOS 14, *)) {
    if (!_thumbView && self.subviews[0].subviews.count > 2) {
      _thumbView = self.subviews[0].subviews[2];
    }
  }
  
  return _thumbView;
}

- (UILabel *)valueLabel {
  if (!_valueLabel) {
    _valueLabel = [[UILabel alloc] initWithFrame:CGRectZero];
    _valueLabel.textColor = [UIColor whiteColor];
    _valueLabel.font = [UIFont systemFontOfSize:14.0];
    // _valueLabel.backgroundColor = [UIColor yellowColor];
    _valueLabel.textAlignment = NSTextAlignmentCenter;
  }
  return _valueLabel;
}

#pragma mark - Action functions

- (void)sliderTouchDown:(PanelBeautySlider *)sender {
  
  if (_touchDown) {
    _touchDown(sender);
  }
}

- (void)sliderValueChanged:(PanelBeautySlider *)sender {
  
  if (_valueChanged) {
    _valueChanged(sender);
  } else {
    [sender setSliderValue: [NSString stringWithFormat:@"%ld", (long)sender.value]];
  }
}

- (void)sliderTouchUpInside:(PanelBeautySlider *)sender {
  
  if (_touchUpInside) {
    _touchUpInside(sender);
  }
}


#pragma mark -

- (void)dealloc {
  
  NSLog(@"%s", __FUNCTION__);
}


@end
