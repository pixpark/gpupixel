//
//  ViewController.m
//  GPUPixelDemo
//
//  Created by gezhaoyou on 2021/3/31.
//
//

#import "ViewController.h"
#import "VideoFilterController.h"
#import "ImageFilterController.h"

@interface ViewController ()
@property (weak, nonatomic) IBOutlet UIButton *videoFilterTestBtn;
@property (weak, nonatomic) IBOutlet UIButton *imageFilterTestBtn;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.navigationController.navigationBarHidden = false;
    NSInteger btnWidth = 300;
    NSInteger btnheight = 150;
    self.videoFilterTestBtn.frame = CGRectMake((self.view.frame.size.width - btnWidth)/2,
                                               100, 
                                               btnWidth,
                                               btnheight);
 
  self.videoFilterTestBtn.layer.cornerRadius = 10;
  [self.videoFilterTestBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
  [self.videoFilterTestBtn setBackgroundColor:[[UIColor alloc] initWithRed:62/255.0 green:173/255.0 blue:247/255.0 alpha:1]];
  
 
  self.imageFilterTestBtn.frame = CGRectMake((self.view.frame.size.width - btnWidth)/2,
                                             120 + btnheight,
                                             btnWidth,
                                             btnheight);
  [self.imageFilterTestBtn setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
  [self.imageFilterTestBtn setBackgroundColor:[[UIColor alloc] initWithRed:214/255.0 green:82/255.0 blue:64/255.0 alpha:0.9]];
  
  self.imageFilterTestBtn.layer.cornerRadius = 10;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
@end
