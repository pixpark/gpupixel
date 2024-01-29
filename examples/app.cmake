# Detect platform
# ------
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
ELSE()
    MESSAGE(FATAL_ERROR "NOT SUPPORT THIS SYSTEM")
ENDIF()

# Config build output path
# ------
SET(OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
SET(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_PATH}/include")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_PATH}/app/${CURRENT_OS}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_PATH}/library/${CURRENT_OS}")
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)

IF(BUILD_DEBUG)
	SET(CMAKE_BUILD_TYPE Debug)
	LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/debug)
ELSE()
	SET(CMAKE_BUILD_TYPE Release)
	LINK_DIRECTORIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/release)
ENDIF(BUILD_DEBUG)

 
# Config source and header file
# -------
# header include path
INCLUDE_DIRECTORIES(
	${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
	${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/include
	${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/stb
	${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glad/include
)
 
# Add common source file
FILE(GLOB SOURCE_FILES 
	"${CMAKE_CURRENT_SOURCE_DIR}/desktop/*" 
	)
 
# build type: executable
# ------
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
 
# link libs
# -------
IF(${CURRENT_OS} STREQUAL "linux")
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
						gpupixel
						GL
						glfw)
ELSEIF(${CURRENT_OS} STREQUAL "windows")
	# link libs find path
	LINK_DIRECTORIES( 
		${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/lib-mingw-w64)
		
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
						gpupixel
						opengl32
						glfw3)
ENDIF()

