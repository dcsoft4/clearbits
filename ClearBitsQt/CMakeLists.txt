cmake_minimum_required(VERSION 3.16)

project(ClearBitsQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(ClearBitsQt
    AppState.cpp
    AppState.h
    main.cpp
)

qt_add_qml_module(ClearBitsQt
    URI ClearBitsQt
    QML_FILES
        Main.qml
        Screen01.qml
)

qt_add_resources(ClearBitsQt "conf"
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

# ClearBitsDesign support module (Constants singleton etc.)
qt_add_qml_module(ClearBitsDesign
    URI ClearBitsDesign
    VERSION 1.0
    QML_FILES
        ClearBitsDesign/Constants.qml
        ClearBitsDesign/EventListModel.qml
        ClearBitsDesign/EventListSimulator.qml
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ClearBitsDesign"
)

set_target_properties(ClearBitsQt PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ClearBitsQt
    PRIVATE Qt6::Quick Qt6::QuickControls2
    PRIVATE ClearBitsDesign
)
