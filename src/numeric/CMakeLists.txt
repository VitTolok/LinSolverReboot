add_library(rational SHARED rational.cpp)

target_include_directories(rational PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)