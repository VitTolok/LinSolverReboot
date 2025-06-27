find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

add_subdirectory(numeric)

add_library(GTest_core INTERFACE)
target_link_libraries(GTest_core INTERFACE
  GTest::gtest
  GTest::gmock
  GTest::gtest_main
  pthread
)