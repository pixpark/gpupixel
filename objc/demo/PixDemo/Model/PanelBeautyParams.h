/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifndef PanelBeautyParams_h
#define PanelBeautyParams_h
#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, PanelBeautyAssembleType) {
  PanelBeautyAssembleType_Beauty = 0,          // 美颜
  PanelBeautyAssembleType_Face = 1,            // 美型
  PanelBeautyAssembleType_CompleteBeauty = 2,  // 一键美颜
};

typedef NS_ENUM(NSInteger, PanelBeautyType) {
  PanelBeautyType_Original = 0,   // 原图
  PanelBeautyType_White = 1,      // 美白
  PanelBeautyType_Mopi = 2,       // 磨皮
  PanelBeautyType_Ruddiess = 3,   // 红润
  PanelBeautyType_Brightness = 4  // 亮度

};

typedef NS_ENUM(NSInteger, PanelBeautyFaceType) {
  PanelBeautyFaceType_Original = 0,  // 原图
  PanelBeautyFaceType_BigEyes = 1,   // 大眼
  PanelBeautyFaceType_ThinFace = 2,  // 瘦脸
  PanelBeautyFaceType_Mouth,         // 嘴型
  PanelBeautyFaceType_Nose,          // 鼻子
  PanelBeautyFaceType_Chin,          // 下巴
  PanelBeautyFaceType_Forehead,      // 额头
  PanelBeautyFaceType_Eyebrow,       // 眉毛
  PanelBeautyFaceType_Canthus,       // 眼角
  PanelBeautyFaceType_EyeDistance,   // 眼距
  PanelBeautyFaceType_EyeAlae,       // 开眼角
  PanelBeautyFaceType_ShaveFace,     // 削脸
  PanelBeautyFaceType_LongNose       // 长鼻
};

static const BOOL isNeedBottom = NO;
// 各个子view的frame值
static const CGFloat PanelMeiyanMenuHeight = 139.f;
static const CGFloat PanelMeiyanMenusCellHeight = 90.f;
static const CGFloat PanelBeautyViewHeight = 140.f;
static const CGFloat PanelFaceViewHeight = 140.f;
static const CGFloat PanelStickersViewHeight = 250.f;
static const CGFloat PanelFiltersViewHeight = 140.f;
/// 原来是80
static const CGFloat PanelFilterCellHeight = 100.f;
static const CGFloat PanelFilterItemColumn = 6;
static const CGFloat PanelStickerItemWidth = 55;
static const CGFloat PanelStickerItemHeight = 55;
static const CGFloat PanelStickerSectionHeight = 40;

static const CGFloat PanelBottomViewHeight = isNeedBottom ? 70.f : 0.0f;
static const CGFloat PanelBottomViewHeight1 = 70.f;
// 原来是120
static const CGFloat PanelMagnifyViewHeight =
    isNeedBottom ? 250.f : 250.f - PanelBottomViewHeight1;
static const CGFloat PanelSpecificCellHeight = 100.f;
static const CGFloat PanelSpecificViewHeight = 140.f;

// 原来是260.5
static const CGFloat PanelBeautyAssembleViewHeight =
    isNeedBottom ? 330.5f : 330.5f - PanelBottomViewHeight1;
// 原来是180
static const CGFloat PanelSpecificAssembleViewHeight =
    isNeedBottom ? 250.f : 250.f - PanelBottomViewHeight1;
static const CGFloat PanelSliderwHeight = 50.f;
static const CGFloat PanelSliderwTop = 10.f;
// 透明度属性取值
static const CGFloat PanelAlpha = 0.8f;
static const CGFloat PanelBlackAlpha = 0.5f;
#define window_width [UIScreen mainScreen].bounds.size.width
#define window_height [UIScreen mainScreen].bounds.size.height

#define ImageBundlePath \
  [[NSBundle mainBundle] pathForResource:@"PIXEL" ofType:@"bundle"]
#define Bundle [NSBundle bundleWithPath:ImageBundlePath]
#define BundleImg(Name) \
  [UIImage imageNamed:Name inBundle:Bundle compatibleWithTraitCollection:nil];
#define FontColorNormal \
  [UIColor colorWithRed:100 / 255.f green:100 / 255.f blue:100 / 255.f alpha:1]
// 黑色模式下字体颜色
#define FontColorBlackNormal \
  [UIColor colorWithRed:220 / 255.f green:220 / 255.f blue:220 / 255.f alpha:1]
#define FontColorSelected \
  [UIColor colorWithRed:255 / 255.f green:85 / 255.f blue:10 / 255.f alpha:1]
// 原来是240
#define LineColor                   \
  [UIColor colorWithRed:255 / 255.f \
                  green:255 / 255.f \
                   blue:255 / 255.f \
                  alpha:0.2]
#define Font_12 [UIFont systemFontOfSize:12]
#define Font_10 [UIFont systemFontOfSize:10]

#define iPhoneX                                        \
  (window_width == 375.f && window_height == 812.f) || \
      (window_width == 414.f && window_height == 896.f)
#define BottomIndicatorHeight (iPhoneX ? 80 : 0)

#define IsString(__string) ([(__string) isKindOfClass:[NSString class]])
#define IsStringWithAnyText(__string)                            \
  (IsString(__string) && ([((NSString*)__string) length] > 0) && \
   (![(__string) isKindOfClass:[NSNull class]]) &&               \
   (![(__string) isEqualToString:@"(null)"]))
// #define IsNullString(__string) (IsString(__string) && ([((NSString
// *)__string) length] == 0) && (![(__string) isKindOfClass:[NSNull class]]) &&
// (![(NSString *)__string) isEqualToString:@"(null)"])

#define IsArray(__array) ([(__array) isKindOfClass:[NSArray class]])
#define IsArrayWithAnyItem(__array) \
  (IsArray(__array) && ([((NSArray*)__array) count] > 0))

#define IsDictionary(__dict) ([(__dict) isKindOfClass:[NSDictionary class]])
#define IsDictionaryWithAnyKeyValue(__dict) \
  (IsDictionary(__dict) && ([[((NSDictionary*)__dict) allKeys] count] > 0))

#endif /* PanelBeautyParams_h */
