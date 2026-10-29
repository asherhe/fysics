cmake_minimum_required(VERSION 3.22)
project(Fysics VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(FYSICS_BUILD_DEMO "Build the fysics demo executable" ON)

add_subdirectory(src)

# optionally build demo program
if(FYSICS_BUILD_DEMO)
  
  include(FetchContent)

  # fetch SDL3
  FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG        release-3.4.18
  )
  set(SDL_TESTS OFF CACHE BOOL "" FORCE) # skip SDL tests
  FetchContent_MakeAvailable(SDL3)

  # fetch imgui
  FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.93.0
  )
  FetchContent_MakeAvailable(imgui)

  # imgui ships as source cpp so we need to add it as a target
  add_library(imgui STATIC
      ${imgui_SOURCE_DIR}/imgui.cpp
      ${imgui_SOURCE_DIR}/imgui_demo.cpp
      ${imgui_SOURCE_DIR}/imgui_draw.cpp
      ${imgui_SOURCE_DIR}/imgui_tables.cpp
      ${imgui_SOURCE_DIR}/imgui_widgets.cpp
      ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
      ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
  )
  target_include_directories(imgui PUBLIC
      ${imgui_SOURCE_DIR}
      ${imgui_SOURCE_DIR}/backends
  )
  target_link_libraries(imgui PUBLIC SDL3::SDL3)

  # fysics demo build
  add_subdirectory(demo)

endif()
