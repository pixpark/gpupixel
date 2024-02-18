# Detect platform
# --------
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(CURRENT_OS "linux")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(CURRENT_OS "windows")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(CURRENT_OS "macos")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
	SET(CURRENT_OS "ios")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	SET(CURRENT_OS "android")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
	SET(CURRENT_OS "wasm")
	add_definitions(-D__emscripten__)
ELSE()
    MESSAGE(FATAL_ERROR "NOT SUPPORT THIS SYSTEM")
ENDIF()

# Config build output path
# --------
SET(OUTPUT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
SET(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/include")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/release)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)


IF(BUILD_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
ENDIF(BUILD_DEBUG)

 
# Config source and header file
# ---------
# header include path
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/core
	${CMAKE_CURRENT_SOURCE_DIR}/filter
	${CMAKE_CURRENT_SOURCE_DIR}/source
	${CMAKE_CURRENT_SOURCE_DIR}/target
	${CMAKE_CURRENT_SOURCE_DIR}/utils
	${CMAKE_CURRENT_SOURCE_DIR}/face_detect
	${CMAKE_CURRENT_SOURCE_DIR}/android/jni
	${CMAKE_CURRENT_SOURCE_DIR}/target/objc
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/include
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/include
)
 
# Add common source file
FILE(GLOB SOURCE_FILES     
	"${CMAKE_CURRENT_SOURCE_DIR}/core/*"        
	"${CMAKE_CURRENT_SOURCE_DIR}/filter/*"         
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*"       
	"${CMAKE_CURRENT_SOURCE_DIR}/target/*"                               
	"${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*"                 
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/*"                 
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/source/*"
)

# Add export header file
FILE(GLOB EXPORT_HEADER 
	"${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"         
	"${CMAKE_CURRENT_SOURCE_DIR}/filter/*.h"         
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"       
	"${CMAKE_CURRENT_SOURCE_DIR}/target/*.h"                      
	"${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h"                 
	"${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*.h"                 
)

# Add resource file
FILE(GLOB RESOURCE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/resources/*"               
	"${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/models/vnn_face278_data/*"               
)

# Add platform source and header and lib link search path
IF(${CURRENT_OS} STREQUAL "windows") 														# windows
	# Source 
	FILE(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
	list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})

	# link libs find path
	LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/lib-mingw-w64)
	FILE(GLOB VNN_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/x64/*)
ELSEIF(${CURRENT_OS} STREQUAL "linux" OR ${CURRENT_OS} STREQUAL "wasm")	
	# Source 
	FILE(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
	list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
	FILE(GLOB VNN_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/*)
ELSEIF(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")						# ios and mac
	# Header
	FILE(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/target/objc/*.h")
	list(APPEND EXPORT_HEADER 	${OBJC_HEADER_FILE})
	
	# Source 
	FILE(GLOB OBJC_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/target/objc/*")
	list(APPEND SOURCE_FILES ${OBJC_SOURCE_FILE})
ELSEIF(${CURRENT_OS} STREQUAL "android")													# android 
	# Header
	FILE(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*.h")
	list(APPEND EXPORT_HEADER 	${OBJC_HEADER_FILE})
	
	# Source 
	FILE(GLOB JNI_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*")
	list(APPEND SOURCE_FILES ${JNI_SOURCE_FILE})

	LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/${ANDROID_ABI})
ENDIF()

# Config project 
# ----------
# build shared or static lib
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${RESOURCE_FILES})
# ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES} ${RESOURCE_FILES})
 
# set platform project 
IF(${CURRENT_OS} STREQUAL "linux")
	# 设置要构建的目标库的名称和类型
	add_library(vnn_kit SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_kit PROPERTIES IMPORTED_LOCATION
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/libvnn_kit.so)

	add_library(vnn_face SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_face PROPERTIES IMPORTED_LOCATION
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/libvnn_face.so)

	# add_library(vnn_core SHARED IMPORTED)
	# # 设置目标库的实际路径
	# set_target_properties(vnn_core PROPERTIES IMPORTED_LOCATION
	# ${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/libvnn_core.so)


	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
ELSEIF(${CURRENT_OS} STREQUAL "windows")
	# 设置要构建的目标库的名称和类型
	add_library(vnn_kit SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_kit PROPERTIES IMPORTED_IMPLIB
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/x64/vnn_kit.dll)

	add_library(vnn_core SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_core PROPERTIES IMPORTED_IMPLIB
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/x64/vnn_core.dll)

	add_library(vnn_face SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_face PROPERTIES IMPORTED_IMPLIB
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/x64/vnn_face.dll)
ELSEIF(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		XCODE_ATTRIBUTE_PRODUCT_NAME ${PROJECT_NAME}
		COMPILE_FLAGS "-x objective-c++"
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER net.pixpark.${PROJECT_NAME}
		PRODUCT_BUNDLE_IDENTIFIER net.pixpark.${PROJECT_NAME}
		CMAKE_XCODE_ATTRIBUTE_BUILT_PRODUCTS_DIR ${PROJECT_NAME}
		MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
		FRAMEWORK_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gezhaoyou
		# "current version" in semantic format in Mach-O binary file
		VERSION 1.0.1
		# "compatibility version" in semantic format in Mach-O binary file
		SOVERSION 1.0.1
		PUBLIC_HEADER "${EXPORT_HEADER}"
		RESOURCE "${RESOURCE_FILES}"
		LINK_FLAGS "-Wl,-F${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}"
		#XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
	)
ELSEIF(${CURRENT_OS} STREQUAL "android")
	# 设置要构建的目标库的名称和类型
	add_library(vnn_kit SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_kit PROPERTIES IMPORTED_LOCATION
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/${ANDROID_ABI}/libvnn_kit.so)

	add_library(vnn_core SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_core PROPERTIES IMPORTED_LOCATION
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/${ANDROID_ABI}/libvnn_core.so)

	add_library(vnn_face SHARED IMPORTED)
	# 设置目标库的实际路径
	set_target_properties(vnn_face PROPERTIES IMPORTED_LOCATION
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/vnn/libs/${CURRENT_OS}/${ANDROID_ABI}/libvnn_face.so)
ELSEIF(${CURRENT_OS} STREQUAL "wasm")
	set_target_properties(${PROJECT_NAME} PROPERTIES 
						SUFFIX ".wasm"
    					LINK_FLAGS "-Os -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3  -s WASM=1")
ENDIF()


# link libs
# -------
IF(${CURRENT_OS} STREQUAL "linux" OR ${CURRENT_OS} STREQUAL "wasm")
	TARGET_LINK_LIBRARIES(
						${PROJECT_NAME}  
						GL
						glfw
						vnn_core
						vnn_kit
						vnn_face)
ELSEIF(${CURRENT_OS} STREQUAL "windows")
	TARGET_LINK_LIBRARIES(
						${PROJECT_NAME} 
						opengl32
						glfw3
						vnn_core
						vnn_kit
						vnn_face)
ELSEIF(${CURRENT_OS} STREQUAL "macos")
	TARGET_LINK_LIBRARIES(
		${PROJECT_NAME} "-framework OpenGL 		\
						-framework AppKit 		\
						-framework QuartzCore  	\
						-framework CoreVideo  	\
						-framework CoreGraphics \
						-framework AVFoundation \
						-framework CoreMedia
						-framework vnn_kit_osx \
						-framework vnn_core_osx \
						-framework vnn_face_osx"
	)
ELSEIF(${CURRENT_OS} STREQUAL "ios")
	TARGET_LINK_LIBRARIES(
	${PROJECT_NAME} "-framework OpenGLES 		\
					-framework UIKit 		\
					-framework QuartzCore  	\
					-framework CoreVideo  	\
					-framework CoreGraphics \
					-framework AVFoundation \
					-framework CoreMedia \
					-framework vnn_kit_ios \
					-framework vnn_core_ios \
					-framework vnn_face_ios"
	)
ELSEIF(${CURRENT_OS} STREQUAL "android")
	TARGET_LINK_LIBRARIES(
					${PROJECT_NAME}  
					log
					android
					GLESv3
					EGL
					jnigraphics
					vnn_core
					vnn_kit
					vnn_face)
ENDIF()

# copy header to install dir
# --------
MACRO(EXPORT_INCLUDE)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} PRE_BUILD 
				COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
				COMMAND ${CMAKE_COMMAND} -E copy 
				${EXPORT_HEADER} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
				COMMENT "Copying export headers to output/include directory.")
ENDMACRO()
EXPORT_INCLUDE()