set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/GUI/interfaces)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/GUI/interfaces)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS 
  ${CMAKE_SOURCE_DIR}/include/GUI/ui
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/interfaces
  ${CMAKE_CURRENT_BINARY_DIR}
)

qt6_wrap_ui(DecompInterface_UI_FILES 
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/interfaces/DecompInterface.ui
)

add_library(DecompInterface STATIC
  ${CMAKE_SOURCE_DIR}/src/GUI/interfaces/DecompInterface.cpp
  ${CMAKE_SOURCE_DIR}/include/GUI/interfaces/DecompInterface.hpp
  ${DecompInterface_UI_FILES}
)

target_include_directories(DecompInterface PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/numeric
  ${CMAKE_SOURCE_DIR}/include/GUI
  ${CMAKE_SOURCE_DIR}/include/GUI/interfaces
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include/solvercore/solvers
  ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_libraries(DecompInterface PUBLIC
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::UiTools
  rational
)
