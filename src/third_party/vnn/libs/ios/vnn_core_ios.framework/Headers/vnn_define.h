#ifndef VNN_DEFINE_H
#define VNN_DEFINE_H
#define VNN_NULL                             0
#define VNN_TRUE                             1
#define VNN_FALSE                            0


/* ------------------------------------------- */
/* Base type definition                		   */

#if defined(VNN_WIN32) || defined(_WIN32) || defined(WIN32)
typedef __int8                          VNNInt8;
typedef __int16                         VNNInt16;
typedef __int32                         VNNInt32;
typedef __int64                         VNNInt64;
typedef unsigned __int8                 VNNUInt8;
typedef unsigned __int16                VNNUInt16;
typedef unsigned __int32                VNNUInt32;
typedef unsigned __int64                VNNUInt64;
#ifdef _WIN64
typedef unsigned __int64                VNNUIntPtr;
typedef __int64                         VNNIntPtr;
#else  /* _WIN64 */
typedef __w64 unsigned int              VNNUIntPtr;
typedef __w64 int                       VNNIntPtr;
#endif  /* _WIN64 */
#else
#include <stdint.h>
typedef int8_t                          VNNInt8;
typedef int16_t                         VNNInt16;
typedef int32_t                         VNNInt32;
typedef int64_t                         VNNInt64;
typedef uint8_t                         VNNUInt8;
typedef uint16_t                        VNNUInt16;
typedef uint32_t                        VNNUInt32;
typedef uint64_t                        VNNUInt64;
typedef uintptr_t                       VNNUIntPtr;
typedef intptr_t                        VNNIntPtr;
typedef void                            VNNVoid;
#endif
typedef void* VNNVoidPtr;
typedef int                             VNNBool;
typedef char                            VNNPath[1024];
typedef char                            VNNText[1024];
typedef char                            VNNName[256];
typedef char                            VNNUUID[64];
typedef VNNUInt32                       VNNHandle;
typedef float                           VNNFloat32;
typedef double                          VNNFloat64;

/* ------------------------------------------- */

/* ------------------------------------------- */
/* Global API result definition                */

typedef enum _VNN_Result {
    VNN_Result_Success = 0,
    VNN_Result_Failed = -1,
    VNN_Result_InvalidModel = -2,
    VNN_Result_InvalidFrameHandler = -3,
    VNN_Result_InvalidInput = -4,
    VNN_Result_InvalidParamName = -5,
    VNN_Result_InvalidParamValue = -6,
    VNN_Result_BadMalloc = -7,
    VNN_Result_NotImplemented = -8,
    VNN_Result_FileOpenFailed = -9,
    VNN_Result_UnsurportedFormat = -10,
    VNN_Result_Failed_GLVersion = -11,
    VNN_Result_Failed_GLEnvironment = -12,
} VNN_Result;

/* ------------------------------------------- */
/* Global Log level definition                 */
typedef enum _VNN_LogLevel {
    VNN_LOG_LEVEL_VERBOSE = 0x00000001, // 0b 0...00000001
    VNN_LOG_LEVEL_DEBUG = 0x00000002, // 0b 0...00000010
    VNN_LOG_LEVEL_INFO = 0x00000004, // 0b 0...00000100
    VNN_LOG_LEVEL_WARN = 0x00000008, // 0b 0...00001000
    VNN_LOG_LEVEL_ERROR = 0x00000010, // 0b 0...00010000
    VNN_LOG_LEVEL_ALL = 0x000000ff, // 0b 0...11111111 (Easy setting to enable all log-level informations)
} VNN_LogLevel;

// New macro definition
#if defined(_WIN32) || defined(WIN32)
#   ifdef VNN_LIB
#       define VNN_API __declspec(dllexport)
#   else
#       define VNN_API
#   endif
#elif defined(ANDROID) || defined(__APPLE__)
#   define VNN_API __attribute__ ((visibility("default")))
#else
#   define VNN_API
#endif
VNN_API VNN_Result VNN_Get_Core_Version(VNNUInt32 &major_ver, VNNUInt32 &minor_ver);
#endif //VNN_DEFINE_H
