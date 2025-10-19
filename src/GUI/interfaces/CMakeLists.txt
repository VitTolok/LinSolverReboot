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

add_library(interface_core INTERFACE)
target_include_directories(interface_core INTERFACE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/numeric
  ${CMAKE_SOURCE_DIR}/include/GUI
  ${CMAKE_SOURCE_DIR}/include/GUI/interfaces
  ${CMAKE_SOURCE_DIR}/include/solvercore
  ${CMAKE_SOURCE_DIR}/include/solvercore/solvers
  ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(interface_core INTERFACE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::UiTools
  numeric
)

add_library(interfaces INTERFACE)
target_link_libraries(interfaces INTERFACE
  DecompInterface
  MatAriInterface
)

qt6_wrap_ui(DecompInterface_UI_FILES 
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/interfaces/DecompInterface.ui
)
qt6_wrap_ui(MatAriInterface_UI_FILES 
  ${CMAKE_SOURCE_DIR}/include/GUI/ui/interfaces/MatAriInterface.ui
)

add_library(DecompInterface STATIC
  ${CMAKE_SOURCE_DIR}/src/GUI/interfaces/DecompInterface.cpp
  ${CMAKE_SOURCE_DIR}/include/GUI/interfaces/DecompInterface.hpp
  ${DecompInterface_UI_FILES}
)
add_library(MatAriInterface STATIC
  ${CMAKE_SOURCE_DIR}/src/GUI/interfaces/MatAriInterface.cpp
  ${CMAKE_SOURCE_DIR}/include/GUI/interfaces/MatAriInterface.hpp
  ${MatAriInterface_UI_FILES}
)

target_link_libraries(DecompInterface PUBLIC
  interface_core
)
target_link_libraries(MatAriInterface PUBLIC
  interface_core
)
