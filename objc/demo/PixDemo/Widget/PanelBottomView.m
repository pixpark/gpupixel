/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelBottomView.h"
@interface PanelBottomView ()
@property (nonatomic, strong) UIButton * takePhotoBtn;
@property (nonatomic, strong) UIButton * packUpBtn;
@property (nonatomic, strong) UIButton * bigPackUpBtn;
@end
@implementation PanelBottomView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
        [self createSubviews];

    }
    return self;
}


#pragma mark - 穿透点击
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
    UIView *hitView = [super hitTest:point withEvent:event];

        if (hitView == self) {
            return nil;
        }
        return hitView;
}

#pragma mark - 创建子视图
- (void)createSubviews {
    CGFloat width = self.frame.size.width;
    CGFloat height = self.frame.size.height;
    CGFloat takePhotoWidth = 50;
    CGFloat takePhotoHeight = takePhotoWidth;
    CGFloat packUpWidth = 15;
    CGFloat packUpHeight = packUpWidth/11*6;
    CGFloat leftMargin = 45;
    _takePhotoBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    _takePhotoBtn.frame = CGRectMake((width-takePhotoWidth)/2, 0, takePhotoWidth, takePhotoHeight);

    _takePhotoBtn.tag = 1000;
    UIImage * image  = BundleImg(@"beautyCamera");
    [_takePhotoBtn setBackgroundImage:image forState:UIControlStateNormal];
    [_takePhotoBtn addTarget:self action:@selector(cameraAction:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:_takePhotoBtn];
    
 
    _packUpBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    _packUpBtn.frame = CGRectMake(leftMargin, (takePhotoHeight - packUpHeight)/2, packUpWidth, packUpHeight);
    _packUpBtn.tag = 1001;
    UIImage * image1  = BundleImg(@"packUp");
    [_packUpBtn setBackgroundImage:image1 forState:UIControlStateNormal];
    [_packUpBtn addTarget:self action:@selector(cameraAction:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:_packUpBtn];
    
    _bigPackUpBtn = [UIButton buttonWithType:UIButtonTypeCustom];
    _bigPackUpBtn.frame = CGRectMake(leftMargin-10, 0, takePhotoWidth, takePhotoWidth);
    _bigPackUpBtn.tag = 1002;
    [_bigPackUpBtn addTarget:self action:@selector(cameraAction:) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:_bigPackUpBtn];
}

- (void)setIsSticker:(BOOL)isSticker{
    _isSticker = isSticker;
    _bigPackUpBtn.hidden = _isSticker;
    _packUpBtn.hidden = _isSticker;
}


- (void)cameraAction:(UIButton*)sender{
    if (sender.tag == 1000) {
        _clickBtn(YES);
    }else{
        _clickBtn(NO);
    }
}

@end
