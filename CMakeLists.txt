cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

project(LinSolverReboot)

add_subdirectory(src)
add_subdirectory(include)
add_subdirectory(test)
