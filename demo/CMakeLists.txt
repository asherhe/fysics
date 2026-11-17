add_executable(demo main.cpp scene.cpp)

target_link_libraries(demo PRIVATE
  Fysics::core
  SDL3::SDL3
  imgui
)

get_target_property(sdl_library_type SDL3::SDL3 TYPE)
if(WIN32 AND sdl_library_type STREQUAL "SHARED_LIBRARY")
  add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:demo>
    VERBATIM
  )
endif()
