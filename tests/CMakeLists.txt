add_executable(fysics_tests
  math_test.cpp
  pool_test.cpp
  shape_test.cpp
  body_test.cpp
)

target_link_libraries(fysics_tests PRIVATE Fysics::core Catch2::Catch2WithMain)

# internal headers (e.g. pool.hpp) live in src/
target_include_directories(fysics_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

# register each TEST_CASE with CTest
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(fysics_tests)

# test coverage display
find_program(GCOVR_PATH gcovr)
if(GCOVR_PATH)
    add_custom_target(fysics_coverage
        COMMENT "Running tests and generating code coverage report..."
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND ${GCOVR_PATH} 
            --root ${CMAKE_SOURCE_DIR} 
            --filter ${CMAKE_SOURCE_DIR}/src/ 
            --filter ${CMAKE_SOURCE_DIR}/include/
            --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    add_dependencies(fysics_coverage fysics)
    add_dependencies(fysics_coverage fysics_tests)
    message(STATUS "Added 'fysics_coverage' target to tests directory")
endif()

