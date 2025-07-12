add_subdirectory(numeric)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set (CMAKE_EXPORT_COMPILE_COMMANDS true)

add_executable(my_qt_app
    main.cpp
)

target_link_libraries(my_qt_app
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
