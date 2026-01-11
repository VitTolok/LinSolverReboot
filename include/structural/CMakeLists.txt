set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/structural)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/structural)

add_library(CircleArr INTERFACE)
target_include_directories(CircleArr INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)