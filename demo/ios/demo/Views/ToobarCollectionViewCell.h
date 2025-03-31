//
//  ToobarCollectionViewCell.h
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface ToobarCollectionViewCell : UICollectionViewCell

@property (nonatomic, strong) NSString *title;

+ (NSString *)reuseIdentifier;

@end

NS_ASSUME_NONNULL_END
