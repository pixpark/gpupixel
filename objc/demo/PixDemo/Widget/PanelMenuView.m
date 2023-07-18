/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#import "PanelMenuView.h"
#import "PannelBeautyMenuCell.h"
#import "PanelBeautyParams.h"

#import "PanelBeautyAssembleView.h"

#import "PannelBeautiesModel.h"

static NSString *StickerImg = @"stickerFace";
static NSString *BeautyImg = @"beauty1";
static NSString *FaceImg = @"face";
static NSString *CameraImg = @"beautyCamera";
static NSString *FilterImg = @"filter";
static NSString *SpecificImg = @"specific";
static NSString *HahaImg = @"haha";


@interface PanelMenuView()<UICollectionViewDelegate,UICollectionViewDataSource,PanelBeautyAssembleViewDelegate>
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) UIView *currentView;
@property (nonatomic, assign) NSInteger lastIndex;
@property (nonatomic, strong) UIView *superView;
@property (nonatomic, strong) PanelBeautyAssembleView *beautyAssembleView;//美颜集合

@property (nonatomic, assign) BOOL menuHidden;

@end
@implementation PanelMenuView

- (instancetype)initWithFrame:(CGRect)frame
                    superView:(UIView *)superView
                     delegate:(id<FilterMenuPanelDelegate>)delegate
               viewController:(CameraFilterController*)vc {
    if (self = [super initWithFrame:frame]) {
        self.superView = superView;
        self.delegate = delegate;
        self.menuHidden = NO;
        self.mRender = vc;
        [self addSubview:self.collectionView];
        self.lastIndex = -1;
        [self getSticks];
    }
    return self;
}

- (void)showMenuView:(BOOL)show {
    if (self.currentView) {
        [UIView animateWithDuration:0.3 animations:^{
                
            self.currentView.frame = CGRectMake(0, window_height, window_width, self.currentView.frame.size.height);
            } completion:^(BOOL finished) {
                [self.currentView removeFromSuperview];
                self.show = YES;
                self.currentView = nil;
                self.hidden = NO;
                return;
            }];
        
        
    }
    if (show) {
        if (![self isDescendantOfView:self.superView]) {
               [self.superView addSubview:self];
            
        }
    } else {
        [self removeFromSuperview];
    }
    self.show = show;
}
- (void)showMenusWithoutRecord:(BOOL)show
{
    self.menuHidden = !show;
    [self.collectionView reloadData];
}
 
#pragma mark - delegate

- (void)handleBeautyWithType:(NSInteger)type level:(CGFloat)beautyLevel {
    switch (type) {
            case PanelBeautyType_Original:{
                [self.mRender setBeautyValue:0];
                [self.mRender setWhithValue:0];
                [self.mRender setSaturationValue:0];
            }
                break;

            case PanelBeautyType_Mopi:

            [self.mRender setBeautyValue:beautyLevel];

            break;
            case PanelBeautyType_White:
            [self.mRender setWhithValue:beautyLevel];
            break;
            case PanelBeautyType_Ruddiess:
            [self.mRender setSaturationValue:beautyLevel];
            break;
            case PanelBeautyType_Brightness:
//            [_beautyManager setBrightnessLift:beautyLevel];
            break;

        default:
            break;
    }
}

//美型
-(void)handleFaceBeautyWithType:(NSInteger)type sliderValue:(NSInteger)value {
   
    switch (type) {
        case PanelBeautyFaceType_Original:{
            //原图-->人脸识别设置
            [self.mRender setThinFaceValue:0];
            [self.mRender setEyeValue:0];
            
        }
            break;
        case PanelBeautyFaceType_ThinFace:
            [self.mRender setThinFaceValue:(int)value];

            break;
        case PanelBeautyFaceType_BigEyes:
            [self.mRender setEyeValue:(int)value];
            break;

        default:
            break;
    }
}



//滤镜
- (void)handleFiltersEffectWithType:(NSInteger)filter  withFilterName:(nonnull NSString *)filterName{
 
}

- (void)takePhoto{
    if ([self.delegate respondsToSelector:@selector(cameraAction)]) {
        [self.delegate cameraAction];
    }
}

- (void)clickPackUp{
    [self showMenuView:YES];
}
 
#pragma mark
- (void)getSticks {

}


#pragma mark - collectionView
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section {
  return self.array.count;
}

- (__kindof UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath {
    PannelBeautyMenuCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"PanelBeautyMenuCell" forIndexPath:indexPath];
    cell.menuModel = self.array[indexPath.row];
    if (![cell.menuModel.beautyTitle isEqualToString:@"单击拍"]){
        cell.hidden = self.menuHidden;
    }else{
        [cell changeRecordState:self.menuHidden];
    }
    return cell;
}

- (CGSize)collectionView:(UICollectionView *)collectionView
                  layout:(UICollectionViewLayout *)collectionViewLayout
  sizeForItemAtIndexPath:(NSIndexPath *)indexPath {
    
    return CGSizeMake((window_width-40)/self.array.count, PanelMeiyanMenusCellHeight);
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath {
    PannelBeautiesModel *currentModel = self.array[indexPath.row];
    if ([currentModel.beautyTitle isEqual:@""]) {
        if ([self.delegate respondsToSelector:@selector(cameraAction)]) {
            [self.delegate cameraAction];
        }
        return;
    }
    else if([currentModel.beautyTitle isEqual:@"单击拍"]){
        if ([self.delegate respondsToSelector:@selector(recordAction)]) {
            [self.delegate recordAction];
        }
        return;
    }else if ([currentModel.beautyTitle isEqual:@"贴纸"]){
//        [self showBeautyViews:self.stickersView];
    }else if ([currentModel.beautyTitle isEqual:@"美颜"]){
        [self.beautyAssembleView configureUI];
        [self showBeautyViews:self.beautyAssembleView];
    }
    
    currentModel.isSelected = YES;
    if (self.lastIndex >= 0) {
        PannelBeautiesModel *lastModel = self.array[self.lastIndex];
        lastModel.isSelected = NO;
    }
    self.lastIndex = indexPath.row;
    [self.collectionView reloadData];
}

#pragma mark - 切换美颜效果分类
- (void)showBeautyViews:(UIView *)currentView {
    
    [self.superView addSubview:currentView];
    CGRect rect = currentView.frame;
    rect.origin.y = window_height - currentView.frame.size.height - BottomIndicatorHeight;
    [currentView setFrame:rect];
    self.currentView = currentView;
    self.currentView.transform = CGAffineTransformMakeTranslation(0.00,  currentView.frame.size.height);
        [UIView animateWithDuration:0.3 animations:^{
           
            self.currentView.transform = CGAffineTransformMakeTranslation(0.00, 0.00);

        }];
    if ([currentView isEqual:self.beautyAssembleView]) {
        [self.beautyAssembleView configureUI];
        //[self.beautyAssembleView configureSlider];
    }
    self.hidden = YES;
}
- (void)animationOfTakingPhoto{
    PannelBeautyMenuCell *cell = (PannelBeautyMenuCell*)[self.collectionView cellForItemAtIndexPath:[NSIndexPath indexPathWithIndex:2]];
    [cell takePhotoAnimation];
}
#pragma mark - lazy
- (PanelBeautyAssembleView *)beautyAssembleView {
    if (!_beautyAssembleView) {
        _beautyAssembleView = [[PanelBeautyAssembleView alloc] initWithFrame:CGRectMake(0,
                                                                                     window_height-PanelBeautyAssembleViewHeight-BottomIndicatorHeight,
                                                                                     window_width,
                                                                                     PanelBeautyAssembleViewHeight)];
        _beautyAssembleView.delegate = self;
        
    }
    return _beautyAssembleView;
}

 
-(NSMutableArray *)array {
    if (!_array) {
        NSArray *arr = @[@"美颜", @"录制", @"贴纸"];
        NSArray *imgArr = @[BeautyImg,CameraImg,StickerImg];
        _array = [NSMutableArray array];
        for (int i = 0; i<arr.count; i++) {
           PannelBeautiesModel *model = [[PannelBeautiesModel alloc] init];
            model.imgName = imgArr[i];
            model.beautyTitle = arr[i];
            model.menuType = PanelBeautyMenuType_Menu;
            [_array addObject:model];
        }
    }
    return _array;
}

- (UICollectionView *)collectionView {
    if (!_collectionView) {
        UICollectionViewFlowLayout *layout = [[UICollectionViewFlowLayout alloc] init];
        layout.minimumLineSpacing = 0;
        layout.minimumInteritemSpacing = 10;
        layout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
        layout.sectionInset = UIEdgeInsetsMake(0, 20, 0,20);
        _collectionView = [[UICollectionView alloc] initWithFrame:CGRectMake(0, 0, window_width, self.frame.size.height) collectionViewLayout:layout];
        _collectionView.backgroundColor = [UIColor clearColor];
        _collectionView.delegate = self;
        _collectionView.dataSource = self;
        [_collectionView registerClass:[PannelBeautyMenuCell class] forCellWithReuseIdentifier:@"PanelBeautyMenuCell"];
        [_collectionView setBackgroundColor:[UIColor colorWithRed:1/123 green:1/23 blue:1/233 alpha:0.3]];
    }
    return _collectionView;
}

@end
