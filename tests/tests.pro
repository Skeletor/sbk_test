QT += core testlib

CONFIG += c++17 console testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = sbk_tests

ProjectRoot = $$clean_path($$PWD/..)
INCLUDEPATH += $$ProjectRoot

SOURCES += \
    $$PWD/test_main.cpp \
    $$PWD/device_data_parser_test.cpp \
    $$ProjectRoot/app/controller/device/device_data_parser.cpp

HEADERS += \
    $$PWD/device_data_parser_test.h \
    $$ProjectRoot/app/controller/device/device_data_parser.h \
    $$ProjectRoot/app/controller/device/device_transport_types.h \
    $$ProjectRoot/app/domain/device/device_types.h
