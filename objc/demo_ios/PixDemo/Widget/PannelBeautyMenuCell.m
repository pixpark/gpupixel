/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */


#import "PannelBeautyMenuCell.h"
#import "PanelBeautyParams.h"
#import "PannelBeautiesModel.h"
@interface PannelBeautyMenuCell()
@property (nonatomic, strong) UIImageView *imgView;
@property (nonatomic, strong) UILabel *beautyLabel;
@property (nonatomic, strong) UIImageView *animationView;
@property (nonatomic, strong) UIImageView *selectedImgView;
@property (nonatomic, strong) UIButton *effectBtn;
@end
@implementation PannelBeautyMenuCell

- (instancetype)initWithFrame:(CGRect)frame {
  if (self = [super initWithFrame:frame]) {
    [self addSubview:self.imgView];
    [self addSubview:self.beautyLabel];
  }
  return self;
}

- (void)setMenuModel:(PannelBeautiesModel *)menuModel {
  if (!menuModel) {
    return;
  }
  _menuModel = menuModel;
  self.beautyLabel.text = menuModel.beautyTitle;
  if (menuModel.menuType == PanelBeautyMenuType_Menu) {
    if ([menuModel.beautyTitle isEqualToString:@""]) {//仅限菜单页，@""的时候是相机功能
      self.beautyLabel.hidden = YES;
      //            UIImage *img = BundleImg(@"cameraBG");
      UIImage * img  = BundleImg(@"video_record")
      self.imgView.image = img;
      self.imgView.frame = CGRectMake((self.frame.size.width - 60)/2, (self.frame.size.height - 60)/2, 60, 60);
      //            [self.imgView addSubview:self.animationView];
      //            self.animationView.frame = CGRectMake(5, 5, 40, 40);
    }
    //短视频拍摄
    else if([menuModel.beautyTitle isEqualToString:@"单击拍"]){
      UIImage *img = [UIImage imageNamed:menuModel.imgName];
      self.imgView.image = img;
      self.imgView.frame = CGRectMake((self.frame.size.width - 60)/2, (self.frame.size.height - 60)/2, 60, 60);
      CGFloat bottom =  self.imgView.frame.origin.y + self.imgView.frame.size.height;
      CGRect rect = self.beautyLabel.frame;
      self.beautyLabel.frame = CGRectMake(rect.origin.x, bottom + 10, rect.size.width, rect.size.height);
      self.beautyLabel.text = @"";
      self.beautyLabel.hidden = YES;
    }
    else {
      for (UIView *subview in self.imgView.subviews){
        [subview removeFromSuperview];
      }
      self.imgView.image = BundleImg(menuModel.imgName);
      self.imgView.frame = CGRectMake((self.frame.size.width - 35)/2, self.isSimplification?(self.frame.size.height - 35)/2:15, 35, 35);
      if (!self.isSimplification) {
        CGFloat bottom =  self.imgView.frame.origin.y + self.imgView.frame.size.height;
        CGRect rect = self.beautyLabel.frame;
        self.beautyLabel.frame = CGRectMake(rect.origin.x, bottom + 10, rect.size.width, rect.size.height);
        self.beautyLabel.hidden = NO;
      }else{
        self.beautyLabel.hidden = YES;
      }
    }
  } else if (menuModel.menuType == PanelBeautyMenuType_QuickBeauty){
    self.imgView.frame = CGRectMake((self.frame.size.width - 50)/2,(self.frame.size.height - 60 - 23)/2, 50, 60);
    self.selectedImgView.frame = self.imgView.frame;
    CGFloat bottom =  _imgView.frame.origin.y + _imgView.frame.size.height;
    self.beautyLabel.frame = CGRectMake(3, bottom + 8, self.frame.size.width - 6, 15);
    self.beautyLabel.textColor = menuModel.isSelected ? FontColorSelected : FontColorBlackNormal;
    self.selectedImgView.hidden = !menuModel.isSelected;
    self.imgView.image = BundleImg(menuModel.imgName);
  } else if (menuModel.menuType == PanelBeautyMenuType_Specify || menuModel.menuType == PanelBeautyMenuType_Filter ) {
    if (menuModel.menuType == PanelBeautyMenuType_Specify) {
      self.imgView.frame = CGRectMake((self.frame.size.width - 50)/2,(self.frame.size.height - 60 - 23)/2, 50, 60);
    } else {
      self.imgView.frame = CGRectMake((self.frame.size.width - 50 )/2,(self.frame.size.height - 60 - 23)/2, 50, 60);
    }
    CGFloat bottom =  _imgView.frame.origin.y + _imgView.frame.size.height;
    self.beautyLabel.frame = CGRectMake(3, bottom+8, self.frame.size.width - 6, 15);
    self.beautyLabel.textColor = menuModel.isSelected ? FontColorSelected : FontColorBlackNormal;
    self.selectedImgView.hidden = !menuModel.isSelected;
    self.selectedImgView.frame = self.imgView.frame;
    self.imgView.image = BundleImg(menuModel.imgName);
  }
  else if (menuModel.menuType == PanelBeautyMenuType_Beauty || menuModel.menuType == PanelBeautyMenuType_Face){
    self.imgView.frame = CGRectMake((self.frame.size.width - 40)/2, (self.frame.size.height - 40-23)/2, 40, 40);
    self.selectedImgView.hidden = YES;
    self.beautyLabel.textColor = menuModel.isSelected ? FontColorSelected : FontColorBlackNormal;
    if (menuModel.isSelected) {
      NSString *name = [NSString stringWithFormat:@"%@_selected",menuModel.imgName];
      UIImage *img = BundleImg(name);
      self.imgView.image = img;
    } else {
      self.imgView.image = BundleImg(menuModel.imgName);
    }
  } else if (menuModel.menuType == PanelBeautyMenuType_Magnify){
    
    
  } else if (menuModel.menuType == PanelBeautyMenuType_Watermark){
    self.imgView.frame = CGRectMake((self.frame.size.width - 40)/2, (self.frame.size.height - 40-23)/2, 40, 40);
    self.selectedImgView.frame = CGRectMake(5, 5, self.frame.size.width - 10, self.frame.size.height - 10);
    self.beautyLabel.textColor = menuModel.isSelected ? FontColorSelected : FontColorBlackNormal;
    if (menuModel.isSelected) {
      NSString *selectedImg = [NSString stringWithFormat:@"%@_selected",menuModel.imgName];
      [self.imgView setImage:[UIImage imageNamed:selectedImg]];
    } else {
      [self.imgView setImage:[UIImage imageNamed:menuModel.imgName]];
    }
  }
}
- (void)switchBeautyEffect:(BOOL)isSelected {
  self.beautyLabel.textColor = isSelected ? FontColorSelected : FontColorBlackNormal;
}


- (void)takePhotoAnimation {
  [UIView animateWithDuration:0.2 animations:^{
    self.animationView.transform = CGAffineTransformMakeScale(0.9, 0.9);
  } completion:^(BOOL finished) {
    [UIView animateWithDuration:0.2 animations:^{
      self.animationView.transform = CGAffineTransformMakeScale(1, 1);
    }];
  }];
}
#pragma mark - lazy
- (UIImageView *)imgView {
  if (!_imgView) {
    _imgView = [[UIImageView alloc] initWithFrame:CGRectMake((self.frame.size.width - 40)/2, (self.frame.size.height - 40 -23)/2, 40, 40)];
  }
  return _imgView;
}
- (UILabel *)beautyLabel {
  if (!_beautyLabel) {
    CGFloat bottom =  _imgView.frame.origin.y + _imgView.frame.size.height;
    _beautyLabel = [[UILabel alloc] initWithFrame:CGRectMake(3, bottom+8, self.frame.size.width - 6, 15)];
    _beautyLabel.font = Font_10;
    _beautyLabel.textColor = [UIColor whiteColor];
    _beautyLabel.textAlignment = NSTextAlignmentCenter;
  }
  return _beautyLabel;
}

- (UIImageView *)animationView {
  if (!_animationView) {
    UIImage *img = BundleImg(@"cameraPoint");
    _animationView = [[UIImageView alloc] initWithImage:img];
    _animationView.contentMode = UIViewContentModeScaleAspectFit;
  }
  return _animationView;
}

- (UIImageView *)selectedImgView {
  if (!_selectedImgView) {
    UIImage *img = BundleImg(@"filter_selected2");
    _selectedImgView = [[UIImageView alloc] initWithImage:img];
    _selectedImgView.hidden = YES;
    [self addSubview:_selectedImgView];
  }
  return _selectedImgView;
}

- (void)changeRecordState:(BOOL)isRecording
{
  if (isRecording){
    self.imgView.image = [UIImage imageNamed:@"record_pause"];
    self.beautyLabel.hidden = YES;
  }else{
    self.imgView.image = [UIImage imageNamed:@"record_start"];
    self.beautyLabel.hidden = NO;
  }
}

@end
