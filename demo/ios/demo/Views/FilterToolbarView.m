//
//  FilterToolbarView.m
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import "FilterToolbarView.h"
#import "ToobarCollectionViewCell.h"

@interface FilterToolbarView () <UICollectionViewDelegate, UICollectionViewDataSource, UICollectionViewDelegateFlowLayout>

@property (nonatomic, strong) NSArray<NSString *> *filterTitles;
@property (nonatomic, assign) NSInteger selectedFilterIndex;

// UI Components
@property (nonatomic, strong, readwrite) UISwitch *effectSwitch;
@property (nonatomic, strong) UISlider *intensitySlider;
@property (nonatomic, strong) UICollectionView *collectionView;
@property (nonatomic, strong) UICollectionViewFlowLayout *flowLayout;

@end

@implementation FilterToolbarView

#pragma mark - Initialization
- (instancetype)initWithFrame:(CGRect)frame filterTitles:(NSArray<NSString *> *)filterTitles
{
    self = [super initWithFrame:frame];
    if (self) {
        _filterTitles = filterTitles;
        _selectedFilterIndex = 0;
        [self setupUI];
    }
    return self;
}

#pragma mark - UI Setup
- (void)setupUI
{
    self.backgroundColor = UIColor.clearColor;
    
    [self addSubview:self.effectSwitch];
    [self addSubview:self.intensitySlider];
    [self addSubview:self.collectionView];
    
    [self setupConstraints];
}

- (void)setupConstraints
{
    [NSLayoutConstraint activateConstraints:@[
        [self.effectSwitch.topAnchor constraintEqualToAnchor:self.topAnchor constant:16],
        [self.effectSwitch.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-16],
        
        [self.intensitySlider.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
        [self.intensitySlider.topAnchor constraintEqualToAnchor:self.effectSwitch.bottomAnchor constant:16],
        [self.intensitySlider.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:30],
        [self.intensitySlider.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-30],
        
        [self.collectionView.heightAnchor constraintEqualToConstant:50],
        [self.collectionView.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
        [self.collectionView.topAnchor constraintEqualToAnchor:self.intensitySlider.bottomAnchor constant:20],
        [self.collectionView.leadingAnchor constraintEqualToAnchor:self.leadingAnchor],
        [self.collectionView.trailingAnchor constraintEqualToAnchor:self.trailingAnchor],
        [self.collectionView.bottomAnchor constraintEqualToAnchor:self.bottomAnchor],
    ]];
}

#pragma mark - Public Methods
- (void)selectFilterAtIndex:(NSInteger)index animated:(BOOL)animated
{
    if (index >= 0 && index < self.filterTitles.count) {
        self.selectedFilterIndex = index;
        NSIndexPath *indexPath = [NSIndexPath indexPathForItem:index inSection:0];
        [self.collectionView selectItemAtIndexPath:indexPath animated:animated scrollPosition:UICollectionViewScrollPositionCenteredHorizontally];
        
        if ([self.delegate respondsToSelector:@selector(filterToolbarView:didSelectFilterAtIndex:)]) {
            [self.delegate filterToolbarView:self didSelectFilterAtIndex:index];
        }
    }
}

- (void)updateSliderValue:(CGFloat)value
{
    self.intensitySlider.value = value;
}

#pragma mark - Lazy Loading
- (UISwitch *)effectSwitch
{
    if (!_effectSwitch) {
        _effectSwitch = [[UISwitch alloc] init];
        _effectSwitch.on = YES;
        _effectSwitch.translatesAutoresizingMaskIntoConstraints = NO;
        [_effectSwitch addTarget:self action:@selector(effectSwitchValueChanged:) forControlEvents:UIControlEventValueChanged];
    }
    return _effectSwitch;
}

- (UISlider *)intensitySlider
{
    if (!_intensitySlider) {
        _intensitySlider = [[UISlider alloc] init];
        _intensitySlider.minimumValue = 0;
        _intensitySlider.maximumValue = 10;
        _intensitySlider.value = 0;
        _intensitySlider.translatesAutoresizingMaskIntoConstraints = NO;
        [_intensitySlider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    }
    return _intensitySlider;
}

- (UICollectionViewFlowLayout *)flowLayout
{
    if (!_flowLayout) {
        _flowLayout = [[UICollectionViewFlowLayout alloc] init];
        _flowLayout.scrollDirection = UICollectionViewScrollDirectionHorizontal;
        _flowLayout.minimumLineSpacing = 10;
        _flowLayout.minimumInteritemSpacing = 5;
    }
    return _flowLayout;
}

- (UICollectionView *)collectionView
{
    if (!_collectionView) {
        _collectionView = [[UICollectionView alloc] initWithFrame:CGRectZero collectionViewLayout:self.flowLayout];
        _collectionView.delegate = self;
        _collectionView.dataSource = self;
        _collectionView.contentInset = UIEdgeInsetsMake(0, 10, 0, 10);
        _collectionView.backgroundColor = UIColor.clearColor;
        _collectionView.showsHorizontalScrollIndicator = NO;
        _collectionView.translatesAutoresizingMaskIntoConstraints = NO;
        [_collectionView registerClass:[ToobarCollectionViewCell class] forCellWithReuseIdentifier:[ToobarCollectionViewCell reuseIdentifier]];
    }
    return _collectionView;
}

#pragma mark - Actions

- (void)effectSwitchValueChanged:(UISwitch *)sender
{
    if ([self.delegate respondsToSelector:@selector(filterToolbarView:didChangeEffectSwitchState:)]) {
        [self.delegate filterToolbarView:self didChangeEffectSwitchState:sender.isOn];
    }
}

- (void)sliderValueChanged:(UISlider *)slider
{
    if ([self.delegate respondsToSelector:@selector(filterToolbarView:didChangeSliderValue:)]) {
        [self.delegate filterToolbarView:self didChangeSliderValue:slider.value];
    }
}

#pragma mark - UICollectionViewDataSource
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return self.filterTitles.count;
}

- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    ToobarCollectionViewCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:[ToobarCollectionViewCell reuseIdentifier] forIndexPath:indexPath];
    cell.title = self.filterTitles[indexPath.row];
    return cell;
}

#pragma mark - UICollectionViewDelegate
- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    self.selectedFilterIndex = indexPath.row;
    [collectionView scrollToItemAtIndexPath:indexPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
    
    if ([self.delegate respondsToSelector:@selector(filterToolbarView:didSelectFilterAtIndex:)]) {
        [self.delegate filterToolbarView:self didSelectFilterAtIndex:indexPath.row];
    }
}

#pragma mark - UICollectionViewDelegateFlowLayout
- (CGSize)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout *)collectionViewLayout sizeForItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *title = self.filterTitles[indexPath.row];
    UIFont *font = [UIFont systemFontOfSize:16 weight:UIFontWeightSemibold];
    CGSize textSize = [title sizeWithAttributes:@{
        NSFontAttributeName: font
    }];
    CGFloat cellWidth = textSize.width + 20;
    
    return CGSizeMake(cellWidth, 50);
}

@end
