//-------------------------------------------------------------------------------------------------------
// Copyright (c) 2021 Guangzhou Joyy Information Technology Co., Ltd. All rights reserved.
// Licensed under the MIT license. See license.txt file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
#ifndef VNN_FACE_H
#define VNN_FACE_H

#include "vnn_common.h"

#ifdef __cplusplus
extern "C" {
#endif

VNN_API VNN_Result VNN_Create_Face( VNNHandle * handle, const int argc, const void * argv[] );

VNN_API VNN_Result VNN_Destroy_Face( VNNHandle* handle );

VNN_API VNN_Result VNN_Apply_Face_CPU( VNNHandle handle, const void * input, void * output );

VNN_API VNN_Result VNN_Apply_Face_GPU( VNNHandle handle, const void * input, void * output );

VNN_API VNN_Result VNN_Set_Face_Attr( VNNHandle handle, const char * name, const void * value );

VNN_API VNN_Result VNN_Get_Face_Attr( VNNHandle handle, const char * name, void * value );


#ifdef __cplusplus
}
#endif

#endif /* VNN_FACE_H */

