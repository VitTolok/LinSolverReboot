add_executable(CircleArr_test)
target_sources(CircleArr_test PRIVATE CircleArr_test.cpp)
target_link_libraries(CircleArr_test
  PRIVATE
    CircleArr
    GTest_core
)
