add_library(MatAriSolver STATIC MatAriSolver.cpp)
target_link_libraries(MatAriSolver PUBLIC Rational)
target_include_directories(MatAriSolver PUBLIC
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/numeric
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include/solvercore/solvers
)