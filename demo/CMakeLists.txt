add_executable(demo main.cpp)

target_link_libraries(demo PRIVATE
  Fysics::core
  SDL3::SDL3
  imgui
)
