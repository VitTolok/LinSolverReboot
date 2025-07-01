set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/numeric)

add_library(rational SHARED rational.cpp)

target_include_directories(rational PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)
