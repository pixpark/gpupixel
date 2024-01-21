# 侦测操作系统
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# 以操作系统名称作为编译输出、安装目录
    SET(CURRENT_OS "linux")
	
	# 在应用程序中可以使用这个宏定义
    ADD_DEFINITIONS(-DMY_LINUX)
	
	# 设置编译参数
	SET(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS} -O0 -g -fPIC -Wall")
	SET(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS} -O3 -fPIC -DNDEBUG -Wall")
	SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g -fPIC -Wall")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fPIC -DNDEBUG -Wall")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# 以操作系统名称作为编译输出、安装目录
    SET(CURRENT_OS "windows")
	
	# 在应用程序中可以使用这个宏定义
    ADD_DEFINITIONS(-DMY_WINDOWS)
	
	# 设置编译参数
	ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

	SET(CMAKE_C_FLAGS_DEBUG     "/O0")
	SET(CMAKE_C_FLAGS_RELEASE   "/O3")
	SET(CMAKE_CXX_FLAGS_DEBUG   "/O0")
	SET(CMAKE_CXX_FLAGS_RELEASE "/O3")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(CURRENT_OS "macos")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
	SET(CURRENT_OS "ios")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	SET(CURRENT_OS "android")
ELSE()
    MESSAGE(FATAL_ERROR "NOT SUPPORT THIS SYSTEM")
ENDIF()

# 设置编译结果的输出、安装路径
SET(OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
SET(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_PATH}/include")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_PATH}/library/${CURRENT_OS}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_PATH}/library/${CURRENT_OS}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_PATH}/bin/${CURRENT_OS}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/release)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

# 引入头文件的路径
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/core
	${CMAKE_CURRENT_SOURCE_DIR}/filter
	${CMAKE_CURRENT_SOURCE_DIR}/source
	${CMAKE_CURRENT_SOURCE_DIR}/target
	${CMAKE_CURRENT_SOURCE_DIR}/utils
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/include
	${CMAKE_CURRENT_SOURCE_DIR}/android
	${CMAKE_CURRENT_SOURCE_DIR}/target/objc
)
 
IF(MY_DEBUG)
	# 编译 Debug 版本
	SET(CMAKE_BUILD_TYPE Debug)
	
	# 引入动态库的路径，因为此模块有可能依赖第三方库
	LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)
ELSE()
	# 编译 Release 版本
	SET(CMAKE_BUILD_TYPE Release)
	
	# 引入动态库的路径，因为此模块有可能依赖第三方库
	LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
ENDIF(MY_DEBUG)

# 参与编译的源文件
FILE(GLOB SOURCE_FILES     
	"${CMAKE_CURRENT_SOURCE_DIR}/core/*"         
	"${CMAKE_CURRENT_SOURCE_DIR}/filter/*"         
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*"       
	"${CMAKE_CURRENT_SOURCE_DIR}/target/*"                               
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/*"                 
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/source/*"
)
# message(STATUS "Variable: ${SOURCE_FILES}")
FILE(GLOB EXPORT_HEADER 
	"${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"         
	"${CMAKE_CURRENT_SOURCE_DIR}/filter/*.h"         
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"       
	"${CMAKE_CURRENT_SOURCE_DIR}/target/*.h"                      
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h"                 
)

FILE(GLOB RESOURCE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/resources/*"               
)
 
IF(${CURRENT_OS} STREQUAL "linux")

ELSEIF(${CURRENT_OS} STREQUAL "windows")

ELSEIF(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")
	list(APPEND EXPORT_HEADER 	"${CMAKE_CURRENT_SOURCE_DIR}/target/objc/GPUPixelTarget.h"
	 							"${CMAKE_CURRENT_SOURCE_DIR}/target/objc/GPUPixelView.h"
	 							"${CMAKE_CURRENT_SOURCE_DIR}/target/objc/ObjcTarget.h")

	list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/target/objc/GPUPixelTarget.mm" 
	"${CMAKE_CURRENT_SOURCE_DIR}/target/objc/GPUPixelView.mm"
	"${CMAKE_CURRENT_SOURCE_DIR}/target/objc/ObjcTarget.cc")
		
ELSEIF(${CURRENT_OS} STREQUAL "android")

ENDIF()
# message(STATUS "Variable: ${EXPORT_HEADER}")

# 编译出动态链接库
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${RESOURCE_FILES})
 
 
IF(${CURRENT_OS} STREQUAL "linux")

ELSEIF(${CURRENT_OS} STREQUAL "windows")

ELSEIF(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")

	set_target_properties(${PROJECT_NAME} PROPERTIES
		COMPILE_FLAGS "-x objective-c++"
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER net.pixpark.${PROJECT_NAME}
		# MACOSX_FRAMEWORK_INFO_PLIST Info.plist
		# "current version" in semantic format in Mach-O binary file
		VERSION 1.0.1
		# "compatibility version" in semantic format in Mach-O binary file
		SOVERSION 1.0.1
		PUBLIC_HEADER "${EXPORT_HEADER}"
		RESOURCE "${RESOURCE_FILES}"
		#XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
	)
ELSEIF(${CURRENT_OS} STREQUAL "android")

ENDIF()


# 用来把头文件复制到安装路径中
MACRO(EXPORT_INCLUDE)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD 
				COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
				COMMAND ${CMAKE_COMMAND} -E copy 
				${EXPORT_HEADER} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
				COMMENT "Copying export headers to output/include directory.")
ENDMACRO()
EXPORT_INCLUDE()

# 链接三方库
IF(${CURRENT_OS} STREQUAL "linux")

ELSEIF(${CURRENT_OS} STREQUAL "windows")
ELSEIF(${CURRENT_OS} STREQUAL "macos")
	TARGET_LINK_LIBRARIES(
		${PROJECT_NAME} "-framework OpenGL 		\
						-framework AppKit 		\
						-framework QuartzCore  	\
						-framework CoreVideo  	\
						-framework CoreGraphics \
						-framework AVFoundation \
						-framework CoreMedia"
	)
ELSEIF(${CURRENT_OS} STREQUAL "ios")
TARGET_LINK_LIBRARIES(
	${PROJECT_NAME} "-framework OpenGLES 		\
					-framework UIKit 		\
					-framework QuartzCore  	\
					-framework CoreVideo  	\
					-framework CoreGraphics \
					-framework AVFoundation \
					-framework CoreMedia"
	)
ELSEIF(${CURRENT_OS} STREQUAL "android")

ENDIF()