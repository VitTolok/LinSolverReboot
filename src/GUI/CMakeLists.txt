set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/GUI)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/GUI)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
  ${CMAKE_SOURCE_DIR}/include/GUI/ui
  ${CMAKE_CURRENT_BINARY_DIR}
)

qt6_wrap_ui(MainWindow_UI_FILES
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/MainWindow.ui
)
qt6_wrap_ui(Process_UI_FILES
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/Process.ui
)

add_library(MainWindow STATIC
  MainWindow.cpp
  ${CMAKE_SOURCE_DIR}/include/GUI/MainWindow.hpp
  ${MainWindow_UI_FILES}
)
add_library(Process STATIC
  Process.cpp
  ${CMAKE_SOURCE_DIR}/include/GUI/Process.hpp
  ${Process_UI_FILES}
)

target_include_directories(MainWindow PUBLIC
  ${CMAKE_SOURCE_DIR}/include/GUI
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_CURRENT_BINARY_DIR} 
)
target_include_directories(Process PUBLIC
  ${CMAKE_SOURCE_DIR}/include/GUI
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include/solvercore/solvers
  ${CMAKE_CURRENT_BINARY_DIR} 
)

target_link_libraries(Process PUBLIC
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::UiTools
)

target_link_libraries(MainWindow PUBLIC
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::UiTools
  Process
)
