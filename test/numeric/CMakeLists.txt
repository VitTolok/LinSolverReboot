set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/test/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/numeric)

add_executable(rational_test ${CMAKE_SOURCE_DIR}/test/numeric/rational_test.cpp)
target_include_directories(rational_test PRIVATE ${CMAKE_SOURCE_DIR}/include/numeric)
target_link_libraries(rational_test PRIVATE
  rational
  GTest_core
)

gtest_discover_tests(rational_test)