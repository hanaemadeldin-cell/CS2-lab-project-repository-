QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# ── Boost (header-only on this machine, no -lboost_system needed) ─────────────
INCLUDEPATH += /usr/local/opt/boost/include
LIBS        += -pthread

# ── Qt GUI sources ────────────────────────────────────────────────────────────
SOURCES += \
    changeusername.cpp \
    chat.cpp \
    chatlogic.cpp \
    database.cpp \
    groupswindow.cpp \
    main.cpp \
    loginwindow.cpp \
    networkclient.cpp \
    privatechatwindow.cpp \
    settings.cpp \
    userswindow.cpp

HEADERS += \
    MessageProcessor.hpp \
    changeusername.h \
    chat.h \
    chatlogic.h \
    database.h \
    groupswindow.h \
    loginwindow.h \
    networkclient.h \
    privatechatwindow.h \
    settings.h \
    userswindow.h

FORMS += \
    changeusername.ui \
    chat.ui \
    loginwindow.ui \
    settings.ui

TRANSLATIONS += \
    GUIforchat_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
