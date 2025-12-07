set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/structural)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/structural)

add_library(CircleArr)
target_sources(CircleArr
PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
  FILES
    CircleArr.cppm
)
target_include_directories(CircleArr PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(CircleArr PROPERTIES
    CXX_MODULES_DIRECTORY ${CMAKE_BINARY_DIR}/modules/structural
    CXX_SCAN_FOR_MODULES ON
)