add_library(fysics STATIC
  detail/body.cpp
  detail/body.hpp
  detail/contact.hpp
  detail/pool.hpp
  detail/world_impl.hpp
  body.cpp
  world.cpp
)

add_library(Fysics::core ALIAS fysics)

target_include_directories(fysics
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
