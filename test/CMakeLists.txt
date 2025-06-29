find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/test)

add_subdirectory(numeric)
add_subdirectory(solvercore)

add_library(GTest_core INTERFACE)
target_link_libraries(GTest_core INTERFACE
  GTest::gtest
  GTest::gmock
  GTest::gtest_main
  pthread
)