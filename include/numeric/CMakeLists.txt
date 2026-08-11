set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/numeric)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/numeric)

add_library(UBigInt INTERFACE)
target_link_libraries(UBigInt INTERFACE CircleArr)
target_include_directories(UBigInt INTERFACE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/numeric
    ${CMAKE_SOURCE_DIR}/include/structural
)

add_library(BigInt INTERFACE)
target_link_libraries(BigInt INTERFACE UBigInt)

add_library(Rational INTERFACE)
target_link_libraries(Rational INTERFACE BigInt)

add_library(numeric INTERFACE)
target_link_libraries(numeric INTERFACE Rational)