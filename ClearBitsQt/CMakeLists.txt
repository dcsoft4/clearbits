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
    ../SAMPBUF.CPP
    ../MP3/MCWaveReader.cpp
    ../MP3/id3_int28.cpp
)

qt_add_qml_module(ClearBitsQt
    URI ClearBitsQt
    QML_FILES
        Main.qml
        Screen01.qml
        Screen01Form.ui.qml
)

qt_add_resources(ClearBitsQt "conf"
    PREFIX "/"
    FILES qtquickcontrols2.conf
)

qt_add_resources(ClearBitsQt "data"
    PREFIX "/"
    FILES
        ../res/random.bin
)

set_target_properties(ClearBitsQt PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_include_directories(ClearBitsQt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../MP3
)

target_compile_definitions(ClearBitsQt PRIVATE
    CLEARBITS_QT
)

if (MSVC)
    # Legacy shared sources contain non-ASCII comments that trigger C4828 under /utf-8.
    target_compile_options(ClearBitsQt PRIVATE /wd4828)
endif()

target_link_libraries(ClearBitsQt
    PRIVATE Qt6::Quick Qt6::QuickControls2
    winmm Msacm32 vfw32 advapi32
)
