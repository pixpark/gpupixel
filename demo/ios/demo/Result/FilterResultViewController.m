//
//  FilterResultViewController.m
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import "FilterResultViewController.h"

@interface FilterResultViewController ()

@property (nonatomic, strong) UIImageView *resultImageView;
@property (nonatomic, strong) UIButton *saveButton;
@property (nonatomic, strong) UIImage *resultImage;

@end

@implementation FilterResultViewController

#pragma mark - Initialization
- (instancetype)initWithImage:(UIImage *)image {
    self = [super init];
    if (self) {
        self.resultImage = image;
    }
    return self;
}

#pragma mark - Life cycle
- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = UIColor.whiteColor;
    self.title = @"滤镜结果";
    UIBarButtonItem* backItem = [[UIBarButtonItem alloc] initWithTitle:@"Home" style:UIBarButtonItemStylePlain target:self action:@selector(backToHomePage)];
    self.navigationItem.leftBarButtonItem = backItem;
    
    [self setupUI];
}

#pragma mark - Setup
- (void)setupUI {
    // 添加图片视图
    self.resultImageView = [[UIImageView alloc] initWithImage:self.resultImage];
    self.resultImageView.contentMode = UIViewContentModeScaleAspectFit;
    self.resultImageView.backgroundColor = [UIColor colorWithWhite:0.95 alpha:1.0];
    self.resultImageView.layer.cornerRadius = 8.0;
    self.resultImageView.layer.masksToBounds = YES;
    self.resultImageView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:self.resultImageView];
    
    // 添加保存按钮
    self.saveButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.saveButton setTitle:@"保存到相册" forState:UIControlStateNormal];
    [self.saveButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    self.saveButton.backgroundColor = [UIColor systemBlueColor];
    self.saveButton.layer.cornerRadius = 22.0;
    self.saveButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self.saveButton addTarget:self action:@selector(saveButtonTapped) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.saveButton];
    
    // 设置约束
    [NSLayoutConstraint activateConstraints:@[
        // 图片视图约束
        [self.resultImageView.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor constant:20],
        [self.resultImageView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [self.resultImageView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        
        // 保存按钮约束
        [self.saveButton.topAnchor constraintEqualToAnchor:self.resultImageView.bottomAnchor constant:30],
        [self.saveButton.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
        [self.saveButton.widthAnchor constraintEqualToConstant:200],
        [self.saveButton.heightAnchor constraintEqualToConstant:44],
        [self.saveButton.bottomAnchor constraintLessThanOrEqualToAnchor:self.view.safeAreaLayoutGuide.bottomAnchor constant:-20],
    ]];
    
    // 设置图片视图高度，使其与宽度成比例但不超过可用空间
    CGFloat screenHeight = UIScreen.mainScreen.bounds.size.height;
    CGFloat maxHeight = screenHeight - 200; // 预留顶部和底部空间
    [self.resultImageView.heightAnchor constraintLessThanOrEqualToConstant:maxHeight].active = YES;
}

#pragma mark - Actions
- (void)backToHomePage {
    [self.navigationController popToRootViewControllerAnimated:YES];
}

- (void)saveButtonTapped {
    if (self.resultImage) {
        UIImageWriteToSavedPhotosAlbum(self.resultImage, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
    } else {
        [self showAlertWithMessage:@"没有可保存的图片"];
    }
}

#pragma mark - Callbacks
- (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo {
    NSString *message = nil;
    if (error) {
        message = [NSString stringWithFormat:@"保存图片失败: %@", error.localizedDescription];
    } else {
        message = @"图片已保存到相册";
    }
    
    [self showAlertWithMessage:message];
}

- (void)showAlertWithMessage:(NSString *)message {
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:nil
                                                                message:message
                                                         preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:nil];
    [alert addAction:okAction];
    [self presentViewController:alert animated:YES completion:nil];
}

@end
