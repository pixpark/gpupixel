/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelBeautyFaceView.h"
#import "PanelBeautySlider.h"
#import "PannelBeautyMenuCell.h"
#import "PanelBeautyParams.h"
#import "PannelBeautiesModel.h"
static NSString *OriginalImg = @"faceOrigin";
static NSString *BigEyeImg = @"bigEye";
static NSString *FaceImg = @"thinFace";
static NSString *MouthImg = @"mouse";
static NSString *NoseImg = @"thinNose";
static NSString *ChinImg = @"chin";
static NSString *ForeheadImg = @"forehead";
static NSString *Eyebrow = @"eyebrow";
static NSString *Canthus = @"canthus";
static NSString *EyeAlae = @"eyeAlea";
static NSString *EyeDistance = @"eyeDistance";
static NSString *ShaveFace = @"shaveFace";
static NSString *Longnose = @"longnose";

@interface PanelBeautyFaceView ()<UICollectionViewDataSource,UICollectionViewDelegate>
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) NSMutableArray *array;
@property (nonatomic, assign) NSInteger lastIndex;
@property (nonatomic, strong) PanelBeautySlider *slider;
@property (nonatomic, assign) NSInteger beautyType;
@end
@implementation PanelBeautyFaceView


- (instancetype)initWithFrame:(CGRect)frame {
  if (self = [super initWithFrame:frame]) {
    [self addSubview:self.collectionView];
    self.lastIndex = -1;
    
  }
  return self;
}

- (void)configureFaceData {
  if (IsArrayWithAnyItem(self.array)) {
    return;//保证只初始化加载一次
  }
  //    NSArray *arr = [[NSUserDefaults standardUserDefaults] objectForKey:@"kFaceWithVersion"];
  NSArray *arr = @[@"原图",
                   @"大眼",
                   @"瘦脸",
                   /* @"嘴型",
                    @"瘦鼻",
                    @"下巴",
                    @"额头",
                    @"眉毛",
                    @"眼角",
                    @"眼距",
                    @"开眼角",
                    @"削脸",
                    @"长鼻"*/];
  
  NSArray *originalValuesArr = @[@"0",
                                 @"30",
                                 @"30",
                                 @"50",
                                 @"50",
                                 @"50",
                                 @"50",
                                 @"0",
                                 @"0",
                                 @"0",
                                 @"0",
                                 @"0",
                                 @"0"];
  
  NSArray *imgArr = @[OriginalImg,BigEyeImg,FaceImg,MouthImg,NoseImg,ChinImg,ForeheadImg,Eyebrow,Canthus,EyeDistance,EyeAlae,ShaveFace,Longnose];
  _array = [NSMutableArray array];
  for (int i = 0; i<arr.count; i++) {
    PannelBeautiesModel *model = [[PannelBeautiesModel alloc] init];
    model.imgName = imgArr[i];
    model.beautyTitle = arr[i];
    model.menuType = PanelBeautyMenuType_Face;
    model.type = i;
    model.originalValue = originalValuesArr[i];
    NSString *faceKey = [NSString stringWithFormat:@"face_%ld",(long)i];
    NSInteger originalValue = model.originalValue.integerValue;
    [[NSUserDefaults standardUserDefaults] setInteger:originalValue forKey:faceKey];
    [self.array addObject:model];
  }
  [self.collectionView reloadData];
}

- (void)clearAllFaceEffects {
  for (int i = 0; i<self.array.count; i++) {
    NSString *faceKey = [NSString stringWithFormat:@"face_%ld",(long)i];
    [[NSUserDefaults standardUserDefaults] setInteger:0 forKey:faceKey];
  }
}

- (void)cancelSelectedFaceType:(NSInteger)type {
  for (int i = 0; i<self.array.count; i++) {
    PannelBeautiesModel *model = self.array[i];
    if (model.type == type) {
      model.isSelected = NO;
    }
  }
  self.lastIndex = -1;
  [self.collectionView reloadData];
}

#pragma mark - collectionView
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
  return self.array.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
  PannelBeautyMenuCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"PanelBeautyMenuCell" forIndexPath:indexPath];
  cell.menuModel = self.array[indexPath.row];
  return cell;
}

- (CGSize)collectionView:(UICollectionView *)collectionView
                  layout:(UICollectionViewLayout *)collectionViewLayout
  sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
  return CGSizeMake((window_width - 20 - 2*20)/3, PanelMeiyanMenusCellHeight);
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
  if (self.lastIndex == indexPath.row) {
    return;
  }
  PannelBeautiesModel *currentModel = self.array[indexPath.row];
  currentModel.isSelected = YES;
  if (self.lastIndex >= 0) {
    PannelBeautiesModel *lastModel = self.array[self.lastIndex];
    lastModel.isSelected = NO;
  }
  
  self.lastIndex = indexPath.row;
  [self.collectionView reloadData];
  self.beautyType = indexPath.row;
  if (indexPath.row == 0) {
    [self clearAllFaceEffects];
  }
  NSString *faceKey = [NSString stringWithFormat:@"face_%ld",(long)self.beautyType];
  NSInteger currentValue = [[NSUserDefaults standardUserDefaults] integerForKey:faceKey];
  if ([self.delegate respondsToSelector:@selector(handleFaceEffects:sliderValue:)]) {
    [self.delegate handleFaceEffects:self.beautyType  sliderValue:currentValue];
  }
}

#pragma mark - lazy
- (UICollectionView *)collectionView {
  if (!_collectionView) {
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    layout.minimumLineSpacing = 0;
    layout.minimumInteritemSpacing = 20;
    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    layout.sectionInset = UIEdgeInsetsMake(20, 20,20,20);
    _collectionView = [[UICollectionView alloc] initWithFrame:CGRectMake(0, 0, window_width, self.frame.size.height) collectionViewLayout:layout];
    _collectionView.showsHorizontalScrollIndicator = NO;
    _collectionView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:PanelBlackAlpha];
    _collectionView.delegate = self;
    _collectionView.dataSource = self;
    [_collectionView registerClass:[PannelBeautyMenuCell class] forCellWithReuseIdentifier:@"PanelBeautyMenuCell"];
  }
  return _collectionView;
}

- (NSMutableArray *)array {
  if (!_array) {
    _array = [NSMutableArray array];
  }
  return _array;
}

- (NSInteger)currentIndex
{
  return _lastIndex;
}

@end
