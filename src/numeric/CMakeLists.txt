set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/numeric)

add_library(rational SHARED rational.cpp)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(UBigInt SHARED UBigInt.cpp)
add_library(BigInt INTERFACE)

target_include_directories(rational PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)
target_include_directories(UBigInt PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)
target_include_directories(BigInt INTERFACE ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(BigInt INTERFACE UBigInt)
target_link_libraries(rational PUBLIC BigInt)

add_library(numeric INTERFACE)
target_link_libraries(numeric INTERFACE
  rational
)
