//
//  ViewController.m
//  BeautyFaceDemo
//
//  Created by gezhaoyou on 2021/3/31.
//
//

#import "ViewController.h"
#import "VideoFilterController.h"
#import "ImageFilterController.h"

@interface ViewController ()
@property(strong, nonatomic) UITableView* tableView;
@property(strong, nonatomic) NSArray* tableTitleArr;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    self.navigationController.navigationBarHidden = true;
    [self.view addSubview:self.tableView];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}

-(UITableView*)tableView {
    if(_tableView == nil) {
        self.tableTitleArr = [[NSArray alloc] initWithObjects:@"🧚‍♀️视频滤镜",
                                                              @"🧘‍♀️图片滤镜",
                                                              nil];
        
        _tableView = [[UITableView alloc] initWithFrame:CGRectMake(0,
                                                                   0,
                                                                   self.view.frame.size.width,
                                                                   self.view.frame.size.height)
                                                  style:UITableViewStyleGrouped];
        
        _tableView.delegate = self;
        _tableView.dataSource = self;
        _tableView.rowHeight = 80;
    }
    return _tableView;
}

-(nullable NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
    return @"Function List";
}

#pragma mark
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    UIViewController* vc = nil;
    switch ((int)indexPath.row) {
        case 0:
            vc = [[VideoFilterController alloc] init];
            break;
        case 1:
            vc = [[ImageFilterController alloc] init];
            break;
        case 2:
            
            break;
        case 3:
            
            break;
        default:
            break;
    }
    if(vc) {
        [self.navigationController pushViewController:vc  animated:YES];
    } else {
        NSLog(@"Warning! vc is nil, noting to do.");
    }
}

#pragma mark
-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

#pragma mark
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.tableTitleArr.count;
}

#pragma mark
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    NSString *cellIdentity=@"cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentity];
    if(cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentity];
    }
    cell.textLabel.text = self.tableTitleArr[indexPath.row];
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
    return cell;
}

@end
