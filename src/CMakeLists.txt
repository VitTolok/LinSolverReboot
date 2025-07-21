add_subdirectory(numeric)
add_subdirectory(GUI)

set (CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(LinSolver
    main.cpp
)
target_include_directories(LinSolver PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/GUI
  ${CMAKE_SOURCE_DIR}/include/solvercore)
target_link_libraries(LinSolver PRIVATE
    MainWindow
)
