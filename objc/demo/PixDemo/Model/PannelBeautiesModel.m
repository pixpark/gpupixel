/*
 * PixDemo
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */


#import "PannelBeautiesModel.h"

@implementation PannelBeautiesModel
- (void)encodeWithCoder:(NSCoder *)aCoder {
  [aCoder encodeObject:self.imgName forKey:@"beautyImage"];
  [aCoder encodeObject:self.beautyTitle forKey:@"beautyTitle"];
  [aCoder encodeObject:self.originalValue forKey:@"value"];
  [aCoder encodeObject:@(self.isSelected) forKey:@"isSelected"];
  [aCoder encodeObject:@(self.type) forKey:@"beautyType"];
  [aCoder encodeObject:@(self.aliment) forKey:@"watermarkAliment"];
  
  [aCoder encodeObject:self.nose_defaultValue forKey:@"nose_defaultValue"];
  [aCoder encodeObject:self.mouth_maxValue forKey:@"mouth_maxValue"];
  [aCoder encodeObject:self.mouth_minValue forKey:@"mouth_minValue"];
  [aCoder encodeObject:self.mouth_defaultValue forKey:@"mouth_defaultValue"];
  [aCoder encodeObject:self.face_maxValue forKey:@"face_maxValue"];
  [aCoder encodeObject:self.face_minValue forKey:@"face_minValue"];
  [aCoder encodeObject:self.face_defaultValue forKey:@"face_defaultValue"];
  [aCoder encodeObject:self.eyeAngle_maxValue forKey:@"eyeAngle_maxValue"];
  [aCoder encodeObject:self.eyeAngle_minValue forKey:@"eyeAngle_minValue"];
  [aCoder encodeObject:self.eyeAngle_defaultValue forKey:@"eyeAngle_defaultValue"];
  [aCoder encodeObject:self.eyeDistance_maxValue forKey:@"eyeDistance_maxValue"];
  [aCoder encodeObject:self.eyeDistance_minValue forKey:@"eyeDistance_minValue"];
  [aCoder encodeObject:self.eyeDistance_defaultValue forKey:@"eyeDistance_defaultValue"];
  [aCoder encodeObject:self.eyeBrown_maxValue forKey:@"eyeBrown_maxValue"];
  [aCoder encodeObject:self.eyeBrown_minValue forKey:@"eyeBrown_minValue"];
  [aCoder encodeObject:self.eyeBrown_defaultValue forKey:@"eyeBrown_defaultValue"];
  [aCoder encodeObject:self.bigEye_maxValue forKey:@"bigEye_maxValue"];
  [aCoder encodeObject:self.bigEye_minValue forKey:@"bigEye_minValue"];
  [aCoder encodeObject:self.bigEye_defaultValue forKey:@"bigEye_defaultValue"];
  [aCoder encodeObject:self.ruddinessValue forKey:@"ruddinessValue"];
  [aCoder encodeObject:self.buffingValue forKey:@"buffingValue"];
  [aCoder encodeObject:self.nose_minValue forKey:@"nose_minValue"];
  [aCoder encodeObject:self.nose_maxValue forKey:@"nose_maxValue"];
  [aCoder encodeObject:self.chin_defaultValue forKey:@"chin_defaultValue"];
  [aCoder encodeObject:self.chin_minValue forKey:@"chin_minValue"];
  [aCoder encodeObject:self.chin_maxValue forKey:@"chin_maxValue"];
  [aCoder encodeObject:self.eyeAlae_maxValue forKey:@"eyeAlae_maxValue"];
  [aCoder encodeObject:self.eyeAlae_minValue forKey:@"eyeAlae_minValue"];
  [aCoder encodeObject:self.eyeAlae_defaultValue forKey:@"eyeAlae_defaultValue"];
  [aCoder encodeObject:self.shaveFace_maxValue forKey:@"shaveFace_maxValue"];
  [aCoder encodeObject:self.shaveFace_minValue forKey:@"shaveFace_minValue"];
  [aCoder encodeObject:self.shaveFace_defaultValue forKey:@"shaveFace_defaultValue"];
  [aCoder encodeObject:self.longnose_maxValue forKey:@"longnose_maxValue"];
  [aCoder encodeObject:self.longnose_minValue forKey:@"longnose_minValue"];
  [aCoder encodeObject:self.longnose_defaultValue forKey:@"longnose_defaultValue"];
  [aCoder encodeObject:self.forehead_maxValue forKey:@"forehead_maxValue"];
  [aCoder encodeObject:self.forehead_minValue forKey:@"forehead_minValue"];
  [aCoder encodeObject:self.forehead_defaultValue forKey:@"forehead_defaultValue"];
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder {
  if (self = [super init]) {
    self.imgName = [aDecoder decodeObjectForKey:@"beautyImage"];
    self.beautyTitle = [aDecoder decodeObjectForKey:@"beautyTitle"];
    self.originalValue = [aDecoder decodeObjectForKey:@"value"];
    self.type = (NSInteger)[aDecoder decodeObjectForKey:@"beautyType"];
    self.aliment = (NSInteger)[aDecoder decodeObjectForKey:@"watermarkAliment"];
    self.isSelected = [aDecoder decodeObjectForKey:@"isSelected"];
    self.whiteValue = [aDecoder decodeObjectForKey:@"whiteValue"];
    self.buffingValue = [aDecoder decodeObjectForKey:@"buffingValue"];
    self.ruddinessValue = [aDecoder decodeObjectForKey:@"ruddinessValue"];
    self.bigEye_defaultValue = [aDecoder decodeObjectForKey:@"bigEye_defaultValue"];
    self.bigEye_minValue = [aDecoder decodeObjectForKey:@"bigEye_minValue"];
    self.bigEye_maxValue = [aDecoder decodeObjectForKey:@"bigEye_maxValue"];
    self.eyeBrown_defaultValue = [aDecoder decodeObjectForKey:@"eyeBrown_defaultValue"];
    self.eyeBrown_minValue = [aDecoder decodeObjectForKey:@"eyeBrown_minValue"];
    self.eyeBrown_maxValue = [aDecoder decodeObjectForKey:@"eyeBrown_maxValue"];
    self.eyeDistance_defaultValue = [aDecoder decodeObjectForKey:@"eyeDistance_defaultValue"];
    self.eyeDistance_minValue = [aDecoder decodeObjectForKey:@"eyeDistance_minValue"];
    self.eyeDistance_maxValue = [aDecoder decodeObjectForKey:@"eyeDistance_maxValue"];
    self.eyeAngle_defaultValue = [aDecoder decodeObjectForKey:@"eyeAngle_defaultValue"];
    self.eyeAngle_minValue = [aDecoder decodeObjectForKey:@"eyeAngle_minValue"];
    self.eyeAngle_maxValue = [aDecoder decodeObjectForKey:@"eyeAngle_maxValue"];
    self.face_defaultValue = [aDecoder decodeObjectForKey:@"face_defaultValue"];
    self.face_minValue = [aDecoder decodeObjectForKey:@"face_minValue"];
    self.face_maxValue = [aDecoder decodeObjectForKey:@"face_maxValue"];
    self.mouth_defaultValue = [aDecoder decodeObjectForKey:@"mouth_defaultValue"];
    self.mouth_minValue = [aDecoder decodeObjectForKey:@"mouth_minValue"];
    self.mouth_maxValue = [aDecoder decodeObjectForKey:@"mouth_maxValue"];
    self.nose_defaultValue = [aDecoder decodeObjectForKey:@"nose_defaultValue"];
    self.nose_minValue = [aDecoder decodeObjectForKey:@"nose_minValue"];
    self.nose_maxValue = [aDecoder decodeObjectForKey:@"nose_maxValue"];
    self.chin_defaultValue = [aDecoder decodeObjectForKey:@"chin_defaultValue"];
    self.chin_minValue = [aDecoder decodeObjectForKey:@"chin_minValue"];
    self.chin_maxValue = [aDecoder decodeObjectForKey:@"chin_maxValue"];
    self.forehead_defaultValue = [aDecoder decodeObjectForKey:@"forehead_defaultValue"];
    self.forehead_minValue = [aDecoder decodeObjectForKey:@"forehead_minValue"];
    self.forehead_maxValue = [aDecoder decodeObjectForKey:@"forehead_maxValue"];
    self.longnose_defaultValue = [aDecoder decodeObjectForKey:@"longnose_defaultValue"];
    self.longnose_minValue = [aDecoder decodeObjectForKey:@"longnose_minValue"];
    self.longnose_maxValue = [aDecoder decodeObjectForKey:@"longnose_maxValue"];
    self.shaveFace_defaultValue = [aDecoder decodeObjectForKey:@"shaveFace_defaultValue"];
    self.shaveFace_minValue = [aDecoder decodeObjectForKey:@"shaveFace_minValue"];
    self.shaveFace_maxValue = [aDecoder decodeObjectForKey:@"shaveFace_maxValue"];
    self.eyeAlae_defaultValue = [aDecoder decodeObjectForKey:@"eyeAlae_defaultValue"];
    self.eyeAlae_minValue = [aDecoder decodeObjectForKey:@"eyeAlae_minValue"];
    self.eyeAlae_maxValue = [aDecoder decodeObjectForKey:@"eyeAlae_maxValue"];
    
    
  }
  return self;
}

+ (instancetype)panel_quickBeautyModelWithDictionary:(NSDictionary *)dic {
  PannelBeautiesModel *model = [PannelBeautiesModel new];
  model.beautyTitle = [dic objectForKey:@"title"];
  model.whiteValue = [dic objectForKey:@"whiteValue"];
  model.buffingValue = [dic objectForKey:@"buffingValue"];
  model.ruddinessValue = [dic objectForKey:@"ruddinessValue"];
  model.bigEye_defaultValue = [dic objectForKey:@"bigEye_defaultValue"];
  model.bigEye_minValue = [dic objectForKey:@"bigEye_minValue"];
  model.bigEye_maxValue = [dic objectForKey:@"bigEye_maxValue"];
  model.eyeBrown_defaultValue = [dic objectForKey:@"eyeBrown_defaultValue"];
  model.eyeBrown_minValue = [dic objectForKey:@"eyeBrown_minValue"];
  model.eyeBrown_maxValue = [dic objectForKey:@"eyeBrown_maxValue"];
  model.eyeDistance_defaultValue = [dic objectForKey:@"eyeDistance_defaultValue"];
  model.eyeDistance_minValue = [dic objectForKey:@"eyeDistance_minValue"];
  model.eyeDistance_maxValue = [dic objectForKey:@"eyeDistance_maxValue"];
  model.eyeAngle_defaultValue = [dic objectForKey:@"eyeAngle_defaultValue"];
  model.eyeAngle_minValue = [dic objectForKey:@"eyeAngle_minValue"];
  model.eyeAngle_maxValue = [dic objectForKey:@"eyeAngle_maxValue"];
  model.face_defaultValue = [dic objectForKey:@"face_defaultValue"];
  model.face_minValue = [dic objectForKey:@"face_minValue"];
  model.face_maxValue = [dic objectForKey:@"face_maxValue"];
  model.mouth_defaultValue = [dic objectForKey:@"mouth_defaultValue"];
  model.mouth_minValue = [dic objectForKey:@"mouth_minValue"];
  model.mouth_maxValue = [dic objectForKey:@"mouth_maxValue"];
  model.nose_defaultValue = [dic objectForKey:@"nose_defaultValue"];
  model.nose_minValue = [dic objectForKey:@"nose_minValue"];
  model.nose_maxValue = [dic objectForKey:@"nose_maxValue"];
  model.chin_defaultValue = [dic objectForKey:@"chin_defaultValue"];
  model.chin_minValue = [dic objectForKey:@"chin_minValue"];
  model.chin_maxValue = [dic objectForKey:@"chin_maxValue"];
  model.forehead_defaultValue = [dic objectForKey:@"forehead_defaultValue"];
  model.forehead_minValue = [dic objectForKey:@"forehead_minValue"];
  model.forehead_maxValue = [dic objectForKey:@"forehead_maxValue"];
  model.longnose_defaultValue = [dic objectForKey:@"longnose_defaultValue"];
  model.longnose_minValue = [dic objectForKey:@"longnose_minValue"];
  model.longnose_maxValue = [dic objectForKey:@"longnose_maxValue"];
  model.shaveFace_defaultValue = [dic objectForKey:@"shaveFace_defaultValue"];
  model.shaveFace_minValue = [dic objectForKey:@"shaveFace_minValue"];
  model.shaveFace_maxValue = [dic objectForKey:@"shaveFace_maxValue"];
  model.eyeAlae_defaultValue = [dic objectForKey:@"eyeAlae_defaultValue"];
  model.eyeAlae_minValue = [dic objectForKey:@"eyeAlae_minValue"];
  model.eyeAlae_maxValue = [dic objectForKey:@"eyeAlae_maxValue"];
  
  return model;
}
@end


