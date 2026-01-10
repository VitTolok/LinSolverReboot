set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/test/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test/numeric)

add_executable(Rational_test ${CMAKE_SOURCE_DIR}/test/numeric/Rational_test.cpp)
target_link_libraries(Rational_test PRIVATE
  Rational
  GTest_core
)
add_executable(UBigInt_test ${CMAKE_SOURCE_DIR}/test/numeric/UBigInt_test.cpp)
target_link_libraries(UBigInt_test PRIVATE
  UBigInt
  GTest_core
)
add_executable(BigInt_test ${CMAKE_SOURCE_DIR}/test/numeric/BigInt_test.cpp)
target_link_libraries(BigInt_test PRIVATE
  BigInt
  GTest_core
)

gtest_discover_tests(Rational_test)
gtest_discover_tests(UBigInt_test)