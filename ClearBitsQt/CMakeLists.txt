cmake_minimum_required(VERSION 3.16)

project(ClearBitsQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(ClearBitsQt
    main.cpp
)

qt_add_qml_module(ClearBitsQt
    URI ClearBitsQt
    QML_FILES
        Main.qml
)

set_target_properties(ClearBitsQt PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ClearBitsQt
    PRIVATE Qt6::Quick
)
