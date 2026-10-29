add_library(fysics STATIC
  core.cpp
)

add_library(Fysics::core ALIAS fysics)

target_include_directories(fysics PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:include>
)
