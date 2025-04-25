# ---- Application project definition ----
# Main desktop demo application for GPUPixel library
set(GPL_APP_NAME "app")

# ---- Source files configuration ----
# add desktop application source files
if(NOT ${CURRENT_PLAT} STREQUAL ${PLAT_WASM})
  set(APP_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/desktop/app.cc")

  # add ImGui source files ImGui provides the GUI components for the desktop
  # demo
  file(
    GLOB IMGUI_SOURCE_FILES "${PROJECT_SOURCE_DIR}/third_party/imgui/*.cpp"
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

    target_compile_options(${GPL_APP_NAME} PRIVATE "-fno-objc-arc")
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

  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/desktop/demo.png
       DESTINATION ${BUILD_OUTPUT_DIR}/bin)
else()

  set(APP_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/wasm/wasm_app.cc")
  add_executable(${GPL_APP_NAME} ${APP_SOURCE_FILES})

  set_target_properties(${GPL_APP_NAME} PROPERTIES SUFFIX ".js")

  target_link_options(
    ${GPL_APP_NAME}
    PRIVATE
    "SHELL:-s EXPORTED_FUNCTIONS=['_malloc','_free']"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
    "SHELL:-s USE_WEBGL2=1"
    "SHELL:-s FULL_ES3=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:--preload-file ${PROJECT_SOURCE_DIR}/src/res/@/gpupixel/res")

  target_link_libraries(${GPL_APP_NAME} gpupixel yuv)

  install(
    TARGETS ${GPL_APP_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

  # WebAssembly installation configuration
  install(TARGETS ${GPL_APP_NAME} RUNTIME DESTINATION bin)

  # Install all Emscripten generated files
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${GPL_APP_NAME}.wasm"
          "${CMAKE_CURRENT_BINARY_DIR}/${GPL_APP_NAME}.data"
          "${CMAKE_CURRENT_BINARY_DIR}/${GPL_APP_NAME}.worker.js"
    DESTINATION bin
    OPTIONAL)

  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/wasm/index.html" DESTINATION bin)
endif()
