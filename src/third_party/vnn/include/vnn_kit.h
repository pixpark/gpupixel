//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2021 Guangzhou Joyy Information Technology Co., Ltd. All rights reserved.
// Licensed under the MIT license. See license.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
#ifndef VNN_KIT_H
#define VNN_KIT_H

#include "vnn_common.h"

#ifdef __cplusplus
extern "C" {
#endif

VNN_API VNN_Result VNN_GetKitVersion(VNNUInt32 &major_ver, VNNUInt32 &minor_ver);

VNN_API VNN_Result VNN_SetLogTag(const char *i_tag);

typedef int (*VNNLOGCALLBACK) (const char* i_log);
VNN_API VNN_Result VNN_SetLogCallback(VNNLOGCALLBACK i_callback);

VNN_API VNN_Result VNN_SetLogLevel(VNNUInt32 i_level);

VNN_API VNN_Result VNN_GetLogLevel(VNNUInt32* o_level);

VNN_API VNN_Result VNN_Init(VNNHandle* i_contextID);

VNN_API VNN_Result VNN_Uninit(VNNHandle i_contextID);

VNN_API VNN_Result VNN_CheckGLVersion();

VNN_API VNN_Result VNN_Get_VNNImage_Bytes(const void * i_img, unsigned int * o_bytes);

VNN_API VNN_Result VNN_Create_VNNImage_From_PixelBuffer(const void * i_cvpixelbuffer,  void * o_vnimage, const bool i_gpu_only);

VNN_API VNN_Result VNN_Free_VNNImage(const void * i_cvpixelbuffer, void * io_vnimage, const bool i_gpu_only);

VNN_API VNN_Result VNN_FaceFrameDataArr_Result_Rotate(VNN_FaceFrameDataArr* data, int rotate);

VNN_API VNN_Result VNN_FaceFrameDataArr_Result_Mirror(VNN_FaceFrameDataArr* data);

VNN_API VNN_Result VNN_FaceFrameDataArr_Result_FlipV(VNN_FaceFrameDataArr* data);

VNN_API VNN_Result VNN_GestureFrameDataArr_Result_Rotate(VNN_GestureFrameDataArr* data, int rotate);

VNN_API VNN_Result VNN_GestureFrameDataArr_Result_Mirror(VNN_GestureFrameDataArr* data);

VNN_API VNN_Result VNN_GestureFrameDataArr_Result_FlipV(VNN_GestureFrameDataArr* data);

VNN_API VNN_Result VNN_ObjCountDataArr_Result_Rotate(VNN_ObjCountDataArr* data, int rotate);

VNN_API VNN_Result VNN_ObjCountDataArr_Result_Mirror(VNN_ObjCountDataArr* data);

VNN_API VNN_Result VNN_ObjCountDataArr_Result_FlipV(VNN_ObjCountDataArr* data);

VNN_API VNN_Result VNN_BodyFrameDataArr_Result_Rotate(VNN_BodyFrameDataArr* data, int rotate);

VNN_API VNN_Result VNN_BodyFrameDataArr_Result_Mirror(VNN_BodyFrameDataArr* data);

VNN_API VNN_Result VNN_BodyFrameDataArr_Result_FlipV(VNN_BodyFrameDataArr* data);

VNN_API VNN_Result VNN_Rect2D_Result_Rotate(VNN_Rect2D* data, int rotate);

VNN_API VNN_Result VNN_Rect2D_Result_Mirror(VNN_Rect2D* data);

VNN_API VNN_Result VNN_Rect2D_Result_FlipV(VNN_Rect2D* data);

#ifdef _OPENMP
VNN_API VNN_Result VNN_Get_Num_Procs(int* num_procs);
VNN_API VNN_Result VNN_Set_NUM_Threads(int num_threads);
#endif


#ifdef __cplusplus
}
#endif

#endif // VNN_KIT_H

