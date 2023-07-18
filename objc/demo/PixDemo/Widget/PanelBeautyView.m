/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelBeautyView.h"
#import "PannelBeautyMenuCell.h"
#import "PanelBeautyParams.h"
#import "PannelBeautiesModel.h"

static NSString *OriginalImg = @"beautyOrigin";
static NSString *WhiteImg = @"beautyWhite";
static NSString *MopiImg = @"beautyMopi";
static NSString *RuddinessImg = @"beautyRuddise";
static NSString *BrightnessImg = @"brightness";


@interface PanelBeautyView ()<UICollectionViewDelegate,UICollectionViewDataSource>
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) NSMutableArray *array;
@property (nonatomic, assign) NSInteger lastIndex;
@property (nonatomic, assign) NSInteger beautyType;
@property (nonatomic, strong) NSMutableArray *arr;
@end
@implementation PanelBeautyView
//#ifdef SAVEEFFECTMODE
//NSInteger currentValue = [[NSUserDefaults standardUserDefaults] integerForKey:title];
//[self beautyEffect:indexPath.row sliderValue:currentValue];
//#endif

- (instancetype)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        [self addSubview:self.collectionView];
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:PanelBlackAlpha];
        self.lastIndex = -1;
    }
    return self;
}

- (void)clearAllBeautyEffects {
    for (int i = 0; i<self.array.count; i++) {
        NSString *beautKey = [NSString stringWithFormat:@"beauty_%ld",(long)i];
        [[NSUserDefaults standardUserDefaults] setInteger:0 forKey:beautKey];
    }
}

- (void)cancelSelectedBeautyType:(NSInteger)type {
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
    
    return CGSizeMake((window_width-40)/4, PanelMeiyanMenusCellHeight);
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
    if (indexPath.row == 0) {
        //2020-07-04点击原图不能清除之前的磨砂美白等数据的缓存
        //[self clearAllBeautyEffects];
    }
    
    self.lastIndex = indexPath.row;
    [self.collectionView reloadData];
    self.beautyType = indexPath.row;
    
    NSString *faceKey = [NSString stringWithFormat:@"beauty_%ld",(long)self.beautyType];
    NSInteger currentValue = [[NSUserDefaults standardUserDefaults] integerForKey:faceKey];
    
    if ([self.delegate respondsToSelector:@selector(handleBeautyEffects:sliderValue:)]) {
        [self.delegate handleBeautyEffects:self.beautyType sliderValue:currentValue];
    }
}

#pragma mark - lazy
- (NSMutableArray *)array {
    if (!_array) {
        NSArray *titleArr = @[@"原图",@"美白",@"磨皮",@"红润"/*,@"亮度"*/];
        
        NSArray *originalValuesArr = @[@"0",@"5",@"5",@"7",@"5.7"];
        NSArray *imgArr = @[OriginalImg,WhiteImg,MopiImg,RuddinessImg,BrightnessImg];
        _array = [NSMutableArray array];
        for (int i = 0; i<titleArr.count; i++) {
            PannelBeautiesModel *model = [[PannelBeautiesModel alloc] init];
            model.imgName = imgArr[i];
            model.beautyTitle = titleArr[i];
            model.menuType = PanelBeautyMenuType_Beauty;
            model.type = i;
            model.originalValue =  originalValuesArr[i];
            NSString *beautKey = [NSString stringWithFormat:@"beauty_%ld",(long)i];
            NSInteger originalValue = model.originalValue.integerValue;
            [[NSUserDefaults standardUserDefaults] setInteger:originalValue forKey:beautKey];
            [_array addObject:model];
        }
    }
    return _array;
}

- (UICollectionView *)collectionView {
    if (!_collectionView) {
        UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
        layout.minimumLineSpacing = 0;
        layout.minimumInteritemSpacing = 0;
        layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
        layout.sectionInset = UIEdgeInsetsMake(20, 20,20,20);
        _collectionView = [[UICollectionView alloc] initWithFrame:CGRectMake(0, 0, window_width,self.frame.size.height) collectionViewLayout:layout];
        
        _collectionView.backgroundColor = [UIColor clearColor];
        _collectionView.delegate = self;
        _collectionView.dataSource = self;
        [_collectionView registerClass:[PannelBeautyMenuCell class] forCellWithReuseIdentifier:@"PanelBeautyMenuCell"];
    }
    return _collectionView;
}

- (NSInteger)currentIndex{
    return _lastIndex;
}



@end

