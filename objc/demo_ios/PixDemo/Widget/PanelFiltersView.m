/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelFiltersView.h"
#import "PannelBeautyMenuCell.h"
#import "PanelBeautyParams.h"
#import "PannelBeautiesModel.h"
#define kFilterName @"kFilterName"
@interface PanelFiltersView ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) NSMutableArray *array;
@property (nonatomic, assign) NSInteger lastIndex;

@end
@implementation PanelFiltersView

- (instancetype)initWithFrame:(CGRect)frame {
  if (self = [super initWithFrame:frame]) {
    [self addSubview:self.collectionView];
    self.lastIndex = 0;
  }
  return self;
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
  return self.array.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
  
  PannelBeautyMenuCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"PanelFilterCell" forIndexPath:indexPath];
  cell.menuModel = self.array[indexPath.row];
  return cell;
}

- (CGSize)collectionView:(UICollectionView *)collectionView
                  layout:(UICollectionViewLayout *)collectionViewLayout
  sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
  
  return CGSizeMake(([UIScreen mainScreen].bounds.size.width-20) /PanelFilterItemColumn, PanelFilterCellHeight);
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
  if (self.lastIndex == indexPath.row) {
    return;
  }
  PannelBeautiesModel *model = self.array[indexPath.row];
  model.isSelected = !model.isSelected;
  
  if ([self.delegate respondsToSelector:@selector(handleFiltersEffect: filterName:)]) {
    [self.delegate handleFiltersEffect:indexPath.row filterName:model.imgName];
  }
  
  PannelBeautiesModel *lastModel = self.array[self.lastIndex];
  lastModel.isSelected = !lastModel.isSelected;
  
  [self.collectionView reloadData];
  self.lastIndex = indexPath.row;
}

#pragma mark - lazy
/*
 @"source",@"hudson",@"hefe",@"anitque",@"whitecat",@"amaro"
 @"lomo",@"latte",,@"sunset",@"sakura",@"romance",@"earlybird",@"emerald"
 @"freud",@"healthy"
 */

-(NSMutableArray *)array {
  if (!_array) {
    //此处的恋爱对应的就是romance
    NSArray *arr = @[@"filterOrigin",@"langman2",@"qingxin2",@"weimei2",@"fennen2", @"huaijiu2", @"landiao2",@"qingliang2",@"rixi2",@"chengshi",@"chulian",@"chuxin",@"danse",@"fanchase",@"hupo",@"meiwei",@"mitaofen",@"naicha",@"pailide",@"wutuobang",@"xiyou",@"rizha",@"blackcat",@"blackwhite",@"brooklyn",@"calm",@"cool",@"kevin",@"romance"];
    NSArray *filtersArr = @[@"原图",@"浪漫",@"清新",@"唯美",@"粉嫩",@"怀旧",@"蓝调",@"清凉",@"日系",@"城市",@"初恋",@"初心",@"单色",@"反差色",@"琥珀",@"美味",@"蜜桃粉",@"奶茶",@"拍立得",@"乌托邦",@"西柚",@"日杂",@"黑猫",@"黑白",@"布鲁克林",@"平静",@"冷酷",@"凯文",@"恋爱"];
    _array = [NSMutableArray array];
    for (int i = 0; i<arr.count; i++) {
      PannelBeautiesModel *model = [PannelBeautiesModel new];
      model.imgName = arr[i];
      model.beautyTitle = filtersArr[i];
      model.isSelected = i == 0 ? YES : NO;
      model.type = i;
      model.menuType = PanelBeautyMenuType_Filter;
      [_array addObject:model];
    }
  }
  return _array;
}

- (UICollectionView *)collectionView {
  if (!_collectionView) {
    UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
    layout.minimumLineSpacing = 0;
    layout.minimumInteritemSpacing = 15;
    layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
    layout.sectionInset = UIEdgeInsetsMake(10, 10, 10, 10);
    _collectionView = [[UICollectionView alloc] initWithFrame:CGRectMake(0, 0, window_width,self.frame.size.height) collectionViewLayout:layout];
    
    _collectionView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:PanelBlackAlpha];
    _collectionView.showsHorizontalScrollIndicator = NO;
    _collectionView.delegate = self;
    _collectionView.dataSource = self;
    [_collectionView registerClass:[PannelBeautyMenuCell class] forCellWithReuseIdentifier:@"PanelFilterCell"];
  }
  return _collectionView;
}

@end
