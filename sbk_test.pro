QT += core gui network widgets

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = sbk_test

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    app/ui/main_window.cpp \

HEADERS += \
    app/ui/main_window.h \
