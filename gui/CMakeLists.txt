cmake_minimum_required(VERSION 3.16)

project(LegionAuraGUI)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(LegionAuraGUI
    main.cpp
    MainWindow.cpp
    MainWindow.ui
)

target_link_libraries(LegionAuraGUI
    Qt6::Widgets
    legionaura_lib
)
