# ---- Application project definition ----
# Main desktop demo application for GPUPixel library
set(GPL_APP_NAME "app")

# ---- Source files configuration ----
# add desktop application source files
set(APP_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/desktop/app.cc")

# add ImGui source files ImGui provides the GUI components for the desktop demo
file(GLOB IMGUI_SOURCE_FILES "${PROJECT_SOURCE_DIR}/third_party/imgui/*.cpp"
     "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp"
     "${PROJECT_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_opengl3.cpp")
list(APPEND APP_SOURCE_FILES ${IMGUI_SOURCE_FILES})

# ---- Build configuration ----
# create executable from source files
add_executable(${GPL_APP_NAME} ${APP_SOURCE_FILES})

# set common include directories
target_include_directories(${GPL_APP_NAME}
                           PRIVATE ${PROJECT_SOURCE_DIR}/third_party/imgui)

# ---- Platform-specific configuration ----
# Each platform requires different compiler flags and linked libraries
if(APPLE)
  # macOS specific configuration Objective-C++ compilation and proper runtime
  # library paths
  set_target_properties(
    ${GPL_APP_NAME}
    PROPERTIES COMPILE_FLAGS "-x objective-c++"
               MACOSX_BUNDLE FALSE
               INSTALL_RPATH "@executable_path/../lib"
               BUILD_WITH_INSTALL_RPATH TRUE)

  target_link_libraries(${GPL_APP_NAME} PRIVATE "-framework OpenGL" glad glfw
                                                dl marsface gpupixel)
elseif(WIN32)
  # Windows specific configuration Support delayed loading of DLLs for better
  # startup performance
  target_compile_definitions(${GPL_APP_NAME} PRIVATE WINDOWS_DLL_DELAY_LOAD)

  target_link_libraries(${GPL_APP_NAME} PRIVATE glad glfw opengl32 marsface
                                                gpupixel delayimp)

  # set delayed DLL loading
  if(MSVC)
    set_target_properties(
      ${GPL_APP_NAME}
      PROPERTIES LINK_FLAGS
                 "/DELAYLOAD:gpupixel.dll /DELAYLOAD:mars-face-kit.dll")
  endif()

else()
  # Linux specific configuration Set runtime path to find shared libraries in
  # the lib subdirectory
  set_target_properties(
    ${GPL_APP_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../lib"
                               BUILD_WITH_INSTALL_RPATH TRUE)

  target_link_libraries(${GPL_APP_NAME} PRIVATE glad glfw GL dl marsface
                                                gpupixel)
endif()

# ---- Installation configuration ----
# Define where and how files should be installed
install(
  TARGETS ${GPL_APP_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Install resource files
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/desktop/demo.png
  DESTINATION bin
  COMPONENT resource)
