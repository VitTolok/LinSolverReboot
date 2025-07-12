set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/numeric)

add_library(rational SHARED rational.cpp)
add_library(UBigInt SHARED UBigInt.cpp)
add_library(BigInt SHARED BigInt.cpp)

target_include_directories(rational PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)
target_include_directories(UBigInt PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)
target_include_directories(BigInt PUBLIC ${CMAKE_SOURCE_DIR}/include/numeric ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(BigInt PUBLIC UBigInt)
target_link_libraries(rational PUBLIC BigInt)
