QT       += core gui network widgets

CONFIG += c++17

TARGET = ServerApp
TEMPLATE = app

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h

RESOURCES += \
    server_resources.qrc \
    server_resources.qrc
