//
//  ToobarCollectionViewCell.m
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import "ToobarCollectionViewCell.h"

@interface ToobarCollectionViewCell ()

@property (nonatomic, strong) UILabel *titleLabel;

@end

@implementation ToobarCollectionViewCell

static NSString * const kToobarCollectionViewCell = @"ToobarCollectionViewCell";

+ (NSString *)reuseIdentifier
{
    return kToobarCollectionViewCell;
}

- (UILabel *)titleLabel
{
    if (!_titleLabel) {
        _titleLabel = [[UILabel alloc] init];
        _titleLabel.font = [UIFont systemFontOfSize:16 weight:UIFontWeightSemibold];
        _titleLabel.textColor = UIColor.grayColor;
        _titleLabel.textAlignment = NSTextAlignmentCenter;
        _titleLabel.translatesAutoresizingMaskIntoConstraints = NO;
    }
    return _titleLabel;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self setupUI];
    }
    return self;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    // Initialization code
    [self setupUI];
}

- (void)setupUI
{
    [self addSubview:self.titleLabel];
    [NSLayoutConstraint activateConstraints:@[
        [self.titleLabel.centerXAnchor constraintEqualToAnchor:self.centerXAnchor],
        [self.titleLabel.centerYAnchor constraintEqualToAnchor:self.centerYAnchor],
        [self.titleLabel.topAnchor constraintEqualToAnchor:self.topAnchor],
        [self.titleLabel.leadingAnchor constraintEqualToAnchor:self.leadingAnchor]
    ]];
}

- (void)setSelected:(BOOL)selected
{
    [super setSelected:selected];
    
    self.titleLabel.textColor = selected ? UIColor.whiteColor : UIColor.darkGrayColor;
}

- (void)setTitle:(NSString *)title
{
    _title = title;
    
    self.titleLabel.text = title;
}

@end
