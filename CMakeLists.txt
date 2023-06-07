cmake_minimum_required(VERSION 3.20)
project(CX_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/Qt6/6.4.3/mingw_64")


find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        )

qt_add_resources(QRC_FILES res.qrc)
add_subdirectory(compiler)
include_directories(compiler)
add_subdirectory(interpreter)
include_directories(interpreter)
aux_source_directory(compiler COMPILER_SRC)
aux_source_directory(interpreter INTERPRETER_SRC)
add_executable(qt_gui WIN32
        ${QRC_FILES}  main.cpp MainWindow.cpp ${COMPILER_SRC} ${INTERPRETER_SRC} MainWindow.h MyTextEdit.cpp MyTextEdit.h TextItem.h StackDialog.cpp StackDialog.h)


target_link_libraries(qt_gui
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        )
