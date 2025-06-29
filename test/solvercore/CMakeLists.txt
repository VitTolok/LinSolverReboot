add_executable(matrix_test ${CMAKE_SOURCE_DIR}/test/solvercore/matrix_test.cpp)
target_include_directories(matrix_test PRIVATE
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(matrix_test PRIVATE GTest_core)
gtest_discover_tests(matrix_test)
