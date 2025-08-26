cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()


add_compile_options(-fsanitize=address,undefined,leak)
add_link_options(-fsanitize=address,undefined,leak)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

project(LinSolverReboot)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)