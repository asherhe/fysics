add_executable(fysics_tests
  math_test.cpp
  pool_test.cpp
)

target_link_libraries(fysics_tests PRIVATE Fysics::core Catch2::Catch2WithMain)

# internal headers (e.g. pool.hpp) live in src/
target_include_directories(fysics_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

# register each TEST_CASE with CTest
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(fysics_tests)
