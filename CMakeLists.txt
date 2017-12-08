# QT settings
# see http://www.cmake.org/cmake/help/cmake-2-8-docs.html#module:FindQt4
cmake_minimum_required (VERSION 2.6)

project(untitled1)

set(CMAKE_AUTOMOC ON)

find_package(Qt4 REQUIRED
    COMPONENTS
    QtCore 
    QtGui 
    QtOpenGL
)

set(QT_USE_QTOPENGL TRUE)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIIONS})

find_package(OpenGL REQUIRED)

# List of all the C++ source code files
SET(HEADERS
#    arthurstyle.h
    controlpoints.h
    quadcontrolpolygon.h
	mywidget.h
    myglwidget.h
	mainwindow.h
)

SET(SRCS
	main.cpp
#    arthurstyle.cpp
    controlpoints.cpp
    quadcontrolpolygon.cpp
	mywidget.cpp
    myglwidget.cpp
	mainwindow.cpp
)

SET(RESOURCE
    untitled1.qrc
)

link_directories(
	C:/Qt/4.8.7/msvc2015
)

# List of all Qt header files
SET(MOC_HDRS
)

add_executable(untitled1
    ${SRCS}
	${HEADERS}
    ${RESOURCE}
)

TARGET_LINK_LIBRARIES(untitled1
    ${OPENGL_LIBRARIES}
    ${QT_LIBRARIES}
    ${QT_QTMAIN_LIBRARY}
)
