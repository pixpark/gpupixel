#ifndef _VNN_COMMON_H_
#define _VNN_COMMON_H_
#include "vnn_define.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------- */
/* Some base interface definition.             */

typedef enum _VNN_PIX_FMT {
    VNN_PIX_FMT_UNKNOW,      /*  Unknow pixel format, as a cube */
    VNN_PIX_FMT_YUVI420,     /*  YUV  4:2:0   12bpp ( 3 planes, the first is Y, the second is U, the third is V */
    VNN_PIX_FMT_YUV420F,     /*  YUV  4:2:0   12bpp ( 2 planes, ios FullRange, the first is Y - luminance channel, the other channel was UV alternate permutation.) */
    VNN_PIX_FMT_YUV420V,     /*  YUV  4:2:0   12bpp ( 2 planes, ios VideoRange, the first is Y - luminance channel, the other channel was UV alternate permutation.) */
    VNN_PIX_FMT_YUV420P_888_SKIP1, /*  YUV  4:2:0   12bpp ( 3 planes, android Camera2, the first is Y - luminance channel, the second is U channel with skip 1, the third is V channel with skip 1.) */
    VNN_PIX_FMT_BGRA8888,    /*  BGRA 8:8:8:8 32bpp ( 4 channel, 8x4=32bit BGRA pixel ) */
    VNN_PIX_FMT_RGBA8888,    /*  RGBA 8:8:8:8 32bpp ( 4 channel, 8x4=32bit RGBA pixel ) */
    VNN_PIX_FMT_GRAY8,       /*  Y    1        8bpp ( 1 channel, 8bit luminance pixel ) */
    VNN_PIX_FMT_NV12,        /*  YUV  4:2:0   12bpp ( 2 planes, the first is Y - luminance channel, the other channel was UV alternate permutation ) */
    VNN_PIX_FMT_NV21,        /*  YUV  4:2:0   12bpp ( 2 planes, andoird default, the first is Y - luminance channel, the other channel was VU alternate permutation ) */
    VNN_PIX_FMT_BGR888,      /*  BGR  8:8:8   24bpp ( 3 channel, 8x3=24bit BGR pixel ) */
    VNN_PIX_FMT_RGB888,      /*  RGB  8:8:8   24bpp ( 3 channel, 8x3=24bit RGB pixel ) */
    VNN_PIX_FMT_GRAY32,      /*  Y    1        8bpp ( 1 channel, 32bit float luminance pixel ) */
    VNN_PIX_FMT_CHW_U8,      /*  As a cube , data layerout was chw, data type was unsigned char */
    VNN_PIX_FMT_CHW_F32,     /*  As a cube , data layerout was chw, data type was float 32 */
    VNN_PIX_FMT_ERROR        /*  Error pixel format */
} VNN_PIX_FMT;

typedef enum _VNN_ORIENT_FMT {
    /*
     Like value as VNN_ORIENT_FMT_ROTATE_90L|VNN_ORIENT_FMT_FLIP_V, means fristly anticlockwise rotate 90 degree, and then flip vertically;
     So, the priority rotation is higher then flip.
     */
    VNN_ORIENT_FMT_DEFAULT =     0x00000000,	/*  Unknow orientated format, as a default option, no rotate and no flip */
    VNN_ORIENT_FMT_ROTATE_90L =  0x00000001, /*  anticlockwise rotate 90 degree (clockwise rotate 270 degree)  */
    VNN_ORIENT_FMT_ROTATE_90R =  0x00000002, /*  clockwise rotate 90 degree  */
    VNN_ORIENT_FMT_ROTATE_180 =  0x00000004, /*  rotate 180 degree  */
    VNN_ORIENT_FMT_FLIP_V =      0x00000008, /*  flip vertically */
    VNN_ORIENT_FMT_FLIP_H =      0x00000010, /*  flip horizontally */
    VNN_ORIENT_FMT_ROTATE_360 =  0x00000020, /*  android case: post carma orientation = 270 degree */
    VNN_ORIENT_FMT_ROTATE_180L = 0x00000040, /*  android case: post carma orientation = 270 degree */
    VNN_ORIENT_FMT_ERROR =       0xFFFFFFFF, /*  ERROR */
} VNN_ORIENT_FMT;

typedef enum _VNN_MODE_FMT {
    VNN_MODE_FMT_VIDEO = 0x00000000,
    VNN_MODE_FMT_PICTURE = 0x00000001,
    VNN_MODE_FMT_DEFAULT = 0x00000000,
} VNN_MODE_FMT;

typedef struct _VNN_Texture {
    VNNUInt32 	textureID;	/* OpenGL texture id.*/
    VNNUInt32 	target;    	/* OpenGL texture target, e.g. GL_TEXTURE_2D.*/
    VNNUInt32 	format;    	/* OpenGL texture format, e.g. GL_RGBA.*/
    VNNInt32 	width;		/* OpenGL texture width.*/
    VNNInt32 	height;		/* OpenGL texture height.*/
} VNN_Texture;

typedef struct _VNN_Rect2D {
    VNNFloat32 x0;	/* left 	*/
    VNNFloat32 y0; 	/* top 		*/
    VNNFloat32 x1; 	/* right 	*/
    VNNFloat32 y1; 	/* bottom	*/
} VNN_Rect2D;

typedef struct _VNN_Point2D {
    VNNFloat32 x;
    VNNFloat32 y;
} VNN_Point2D;

typedef struct _VNN_Image {
    VNN_ORIENT_FMT   ori_fmt;	 /* orientation format enum of img*/
    VNN_PIX_FMT      pix_fmt;    /* pixel format enum of img */
    VNN_MODE_FMT	 mode_fmt;	 /* mode format enum of detection, can use video/picture/default */
    VNNInt32         width;      /* width of img */
    VNNInt32         height;     /* height of img */
    VNNInt32         channels;   /* channels of img */
    VNN_Rect2D       rect;       /* rect record the place in target image where this image should be put into, when used as output this member may help.*/
    VNNVoidPtr       data;       /* if is cpu-backend, data means the raw data of img, default type is "unsigned char". But data is NULL in gpu-backend. */
    VNNVoidPtr       texture;    /* if is cpu-backend, texture is NULL. But if is gpu-backend, texture may be a VNN_Texture(android|ios) or a CVPixelbuffer(ios). */
} VNN_Image;



/* ------------------------------------------- */

/* ------------------------------------------- */
/* Some definition of Object Count.            */
typedef struct _VNN_ObjectCountDataArr {
    VNNInt32     count;
    VNN_Rect2D * objRectArr;
} VNN_ObjCountDataArr;
VNN_API void VNN_ObjCountDataArr_Free(VNN_ObjCountDataArr *);
/* ------------------------------------------- */



/* ------------------------------------------- */
/* Some definition of Frame mask		       */
//typedef struct _VNN_FrameInstanceMaskData {
//    VNNInt32	width;
//    VNNInt32    height;
//    VNN_Rect2D  maskRect;
//    VNNVoidPtr  data;
//}VNN_FrameInstanceMaskData;


/* ------------------------------------------- */
/* Some Face definition.             		   */

#define VNN_FRAMEDATA_MAX_FACE_LANDMARKS_NUM 278
#define VNN_FRAMEDATAARR_MAX_FACES_NUM       5

typedef struct _VNN_FaceFrameData {
    VNNFloat32 	    faceScore;
    VNN_Rect2D 	    faceRect;
    VNNUInt32       inputWidth;
    VNNUInt32       inputHeight;
    VNNUInt32 	    faceLandmarksNum;
    VNN_Point2D 	faceLandmarks[VNN_FRAMEDATA_MAX_FACE_LANDMARKS_NUM];
    VNNFloat32 	    faceLandmarkScores[VNN_FRAMEDATA_MAX_FACE_LANDMARKS_NUM];
    /* Face Attributes */
    VNNBool         ifCloseLeftEye;
    VNNBool         ifCloseRightEye;
    VNNBool 		ifBlinkLeftEye;
    VNNBool 		ifBlinkRightEye;
    VNNBool 		ifOpenMouth;
    VNNBool 		ifShakeHead;
    VNNBool 		ifNodHead;
    VNNBool         ifOpenCloseMouth;
    VNNFloat32      smileScore;
    VNNFloat32      faceYaw;
} VNN_FaceFrameData;

typedef struct _VNN_FaceFrameDataArr {
    VNNUInt32 			facesNum;
    VNN_FaceFrameData 	facesArr[VNN_FRAMEDATAARR_MAX_FACES_NUM];
} VNN_FaceFrameDataArr;

//typedef struct _VNN_FaceMaskDataArr {
//    VNNUInt32 			        facesNum;
//    VNN_FrameInstanceMaskData 	faceMaskArr[VNN_FRAMEDATAARR_MAX_FACES_NUM];
//} VNN_FaceMaskDataArr;

#define VNN_FACE_APPLY_VIDEO_MODE 	0
#define VNN_FACE_APPLY_PICTURE_MODE 	1

/* ------------------------------------------- */


/* ------------------------------------------- */
/* Some Gesture definition.             	   */

#define VNN_FRAMEDATAARR_MAX_GESTURE_NUM  15

typedef enum _VNN_GestureType {
    VNN_GestureType_Unknow       = 0x00,
    VNN_GestureType_V            = 0x01, // Pinyin: jian dao shou
    VNN_GestureType_ThumbUp      = 0x02, // Pinyin: dian zan
    VNN_GestureType_OneHandHeart = 0x03, // Pinyin: dan shou bi xin
    VNN_GestureType_SpiderMan    = 0x04, // Pinyin: zhi zhu xia
    VNN_GestureType_Lift         = 0x05, // Pinyin: tuo ju
    VNN_GestureType_666          = 0x06, // 6 6 6
    VNN_GestureType_TwoHandHeart = 0x07, // Pinyin: shuang shou bi xin
    VNN_GestureType_PalmTogether = 0x08, // Pinyin: bao quan
    VNN_GestureType_PalmOpen     = 0x09, // Pinyin: zhang kai shou zhang
    VNN_GestureType_ZuoYi        = 0x0a, // Pinyin: zuo yi | bao quan
    VNN_GestureType_OK           = 0x0b, // OK
    VNN_GestureType_ERROR        = 0xff,
} VNN_GestureType;

typedef struct _VNN_GestureFrameData {
    VNN_GestureType 	type;                   /* Gesture type */
    VNN_Rect2D 		rect;					/* Gesture rect, left-top-right-bottom */
    VNNFloat32		score;					/* Gesture confidence socre */
} VNN_GestureFrameData;

typedef struct _VNN_GestureFrameDataArr {
    VNNUInt32 			gestureNum;
    VNN_GestureFrameData gestureArr[VNN_FRAMEDATAARR_MAX_GESTURE_NUM];
} VNN_GestureFrameDataArr;

/* ------------------------------------------- */


/* ------------------------------------------- */
/* Some struct definitions for pose sdk.       */

#define VNN_FRAMEDATA_MAX_BODYLANDMARKS_NUM  22
#define VNN_FRAMEDATAARR_MAX_BODYS_NUM       5
typedef enum _VNN_BodyResultDesc {
    VNN_BodyResultDesc_Normal = 0,
    VNN_BodyResultDesc_NoPerson = 1,						// "没有人"
    VNN_BodyResultDesc_MorethanOnePerson = 2,			// "注意：仅支持1人"
    VNN_BodyResultDesc_NoKneeSeen = 3,					// "看不到膝盖，请退后"
    VNN_BodyResultDesc_NoFootSeen = 4,					// "看不到脚部，请退后"
    VNN_BodyResultDesc_NoHipSeen = 5                     // "看不到腰部，请退后"
}VNN_BodyResultDesc;
typedef struct _VNN_BodyFrameData {
    VNNFloat32   bodyScore;
    VNN_Rect2D   bodyRect;
    VNNUInt32    bodyLandmarksNum;
    VNN_Point2D  bodyLandmarks[VNN_FRAMEDATA_MAX_BODYLANDMARKS_NUM];
    VNNFloat32   bodyLandmarkScores[VNN_FRAMEDATA_MAX_BODYLANDMARKS_NUM];
    VNNUInt32	 isWriggleWaist;
    VNNUInt32	 isCrouch;
    VNNUInt32	 isRun;
    VNN_BodyResultDesc bodyResultDesc;
} VNN_BodyFrameData;

typedef struct _VNN_BodyFrameDataArr {
    VNNUInt32         bodiesNum;
    VNN_BodyFrameData bodiesArr[VNN_FRAMEDATAARR_MAX_BODYS_NUM];
} VNN_BodyFrameDataArr;

/* ------------------------------------------- */

/* ---------------------------------------------------------- */
/* Some struct definitions for hand reconstruction sdk.       */

#define VNN_HANDRECONSTRUCTION_SHAPE_COEF_DIMS 10
#define VNN_HANDRECONSTRUCTION_POSE_COEF_DIMS  33
#define VNN_HANDLECONSTRUCTION_CAM_COEF_DIMS   3


typedef struct _VNN_HandReconstructionFrameData {
    VNNFloat32 rightHandShapeCoefs[VNN_HANDRECONSTRUCTION_SHAPE_COEF_DIMS];
    VNNFloat32 rightHandPoseCoefs[VNN_HANDRECONSTRUCTION_POSE_COEF_DIMS];
    VNNFloat32 rightHandCamCoefs[VNN_HANDLECONSTRUCTION_CAM_COEF_DIMS];
    VNNFloat32 leftHandShapeCoefs[VNN_HANDRECONSTRUCTION_SHAPE_COEF_DIMS];
    VNNFloat32 leftHandPoseCoefs[VNN_HANDRECONSTRUCTION_POSE_COEF_DIMS];
    VNNFloat32 leftHandCamCoefs[VNN_HANDLECONSTRUCTION_CAM_COEF_DIMS];
} VNN_HandReconstructionFrameData;

/* ---------------------------------------------------------- */


/* ---------------------------------------------------------- */
/* Some struct definitions for body reconstruction sdk.       */

#define VNN_BODYRECONSTRUCTION_POSE_COEF_DIMS 207
#define VNN_BODYRECONSTRUCTION_SHAPE_COEF_DIMS 10
#define VNN_BODYRECONSTRUCTION_GLOBAL_ROTATE_COEF_DIMS  9
#define VNN_BODYLECONSTRUCTION_CAM_OFFSET_COEF_DIMS 3  


typedef struct _VNN_BodyReconstructionFrameData {
    VNNFloat32 bodyPoseCoefs[VNN_BODYRECONSTRUCTION_POSE_COEF_DIMS];
    VNNFloat32 bodyShapeCoefs[VNN_BODYRECONSTRUCTION_SHAPE_COEF_DIMS];
    VNNFloat32 bodyGlobalRotateCoefs[VNN_BODYRECONSTRUCTION_GLOBAL_ROTATE_COEF_DIMS];
    VNNFloat32 bodyCamOffsetCoefs[VNN_BODYLECONSTRUCTION_CAM_OFFSET_COEF_DIMS];
} VNN_BodyReconstructionFrameData;

/* ---------------------------------------------------------- */

/* ------------------------------------------- */
/* Some definition of Classification.          */
#define VNN_CLASSIFICATION_ACC_TOP_N 5
#define VNN_MAX_MULTI_CLASSFICATION_NUM 10
#define VNN_MAX_LABEL_LENGTH 100
typedef struct _VNN_Classification_Accuracy_Top_N {
    VNNUInt32  categories[VNN_CLASSIFICATION_ACC_TOP_N];
    char   labels[VNN_CLASSIFICATION_ACC_TOP_N][VNN_MAX_LABEL_LENGTH];
    VNNFloat32 probabilities[VNN_CLASSIFICATION_ACC_TOP_N];
    VNNInt32 usedTopN;
} VNN_ClsTopNAcc;

typedef struct _VNN_ClassificationTopNDataArr {
    VNNUInt32 			numCls;
    VNN_ClsTopNAcc      clsArr[VNN_MAX_MULTI_CLASSFICATION_NUM];

} VNN_ClsTopNAccArr;

typedef struct _VNN_MultiClassificationTopNDataArr {
    VNNUInt32 			numOut;
    VNN_ClsTopNAccArr      multiClsArr[VNN_FRAMEDATAARR_MAX_FACES_NUM]; //used for multi face

} VNN_MultiClsTopNAccArr;
/* ------------------------------------------- */

typedef struct _VNN_ImageArr {
    VNNInt32     imgsNum;
    VNN_Image    imgsArr[VNN_FRAMEDATAARR_MAX_FACES_NUM];
}VNN_ImageArr;

#ifdef __cplusplus
}
#endif

#endif // _VNN_COMMON_H_
