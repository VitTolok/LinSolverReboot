set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/test/solvercore)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/solvercore)

add_executable(matrix_test ${CMAKE_SOURCE_DIR}/test/solvercore/matrix_test.cpp)
target_include_directories(matrix_test PRIVATE
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(matrix_test PRIVATE GTest_core)
gtest_discover_tests(matrix_test)

add_executable(vector_test ${CMAKE_SOURCE_DIR}/test/solvercore/vector_test.cpp)
target_include_directories(vector_test PRIVATE
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(vector_test PRIVATE GTest_core)
gtest_discover_tests(vector_test)

add_executable(decomp_test ${CMAKE_SOURCE_DIR}/test/solvercore/decomp_test.cpp)
target_include_directories(decomp_test PRIVATE
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(decomp_test PRIVATE GTest_core)
gtest_discover_tests(decomp_test)
