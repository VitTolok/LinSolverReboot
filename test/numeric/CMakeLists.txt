set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/test/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/numeric)

add_executable(rational_test ${CMAKE_SOURCE_DIR}/test/numeric/rational_test.cpp)
target_include_directories(rational_test PRIVATE ${CMAKE_SOURCE_DIR}/include/numeric)
target_link_libraries(rational_test PRIVATE
  rational
  GTest_core
)
add_executable(UBigInt_test ${CMAKE_SOURCE_DIR}/test/numeric/UBigInt_test.cpp)
target_include_directories(UBigInt_test PRIVATE ${CMAKE_SOURCE_DIR}/include/numeric)
target_link_libraries(UBigInt_test PRIVATE
  UBigInt
  GTest_core
)
add_executable(BigInt_test ${CMAKE_SOURCE_DIR}/test/numeric/BigInt_test.cpp)
target_include_directories(BigInt_test PRIVATE ${CMAKE_SOURCE_DIR}/include/numeric)
target_link_libraries(BigInt_test PRIVATE
  BigInt
  GTest_core
)

gtest_discover_tests(rational_test)
gtest_discover_tests(UBigInt_test)