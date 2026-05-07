QT += core

CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app

TARGET = gtest_logic

INCLUDEPATH += C:/vcpkg/installed/x64-mingw-dynamic/include

LIBS += -LC:/vcpkg/installed/x64-mingw-dynamic/lib
LIBS += -lgtest
LIBS += -lgmock

SOURCES += \
    gtest_logic.cpp \
    chatlogic.cpp

HEADERS += \
    chatlogic.h \
    MessageProcessor.hpp