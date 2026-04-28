QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    changeusername.cpp \
    chat.cpp \
    chatlogic.cpp \
    main.cpp \
    loginwindow.cpp \
    networkclient.cpp \
    server.cpp \
    settings.cpp \
    test_logic.cpp

HEADERS += \
    MessageProcessor.hpp \
    changeusername.h \
    chat.h \
    chatlogic.h \
    loginwindow.h \
    networkclient.h \
    settings.h

FORMS += \
    changeusername.ui \
    chat.ui \
    loginwindow.ui \
    settings.ui

TRANSLATIONS += \
    GUIforchat_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
