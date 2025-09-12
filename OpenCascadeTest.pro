QT += core widgets 3dcore 3drender 3dextras

CONFIG += c++17

SOURCES += main.cpp \
    cylinderobject.cpp \
    geo3dobject.cpp \
    geo3dobjectset.cpp \
    qt3dviewer.cpp

TARGET = qt3d_cylinder_viewer

HEADERS += \
    cylinderobject.h \
    geo3dobject.h \
    geo3dobjectset.h \
    qt3dviewer.h
