QT += core gui network widgets

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = sbk_test

ProjectRoot = $$clean_path($$PWD/..)
INCLUDEPATH += $$ProjectRoot

SOURCES += \
    $$ProjectRoot/main.cpp \
    $$PWD/application.cpp \
    $$PWD/config/modules/app_config.cpp \
    $$PWD/config/modules/controller/controller_config.cpp \
    $$PWD/config/modules/device/device_freshness_config.cpp \
    $$PWD/config/modules/device/device_poller_config.cpp \
    $$PWD/config/modules/device/device_transport_config.cpp \
    $$PWD/config/modules/device/http_device_transport_config.cpp \
    $$PWD/controller/controller.cpp \
    $$PWD/controller/device/device_data_parser.cpp \
    $$PWD/controller/device/device_freshness_sentinel.cpp \
    $$PWD/controller/device/device_manager.cpp \
    $$PWD/controller/device/device_poller.cpp \
    $$PWD/controller/device/http_device_transport.cpp \
    $$PWD/controller/device/idevice_transport.cpp \
    $$PWD/controller/device/mock_device_transport.cpp \
    $$PWD/ui/main_window.cpp \

HEADERS += \
    $$PWD/application.h \
    $$PWD/config/modules/app_config.h \
    $$PWD/config/modules/controller/controller_config.h \
    $$PWD/config/modules/device/device_freshness_config.h \
    $$PWD/config/modules/device/device_poller_config.h \
    $$PWD/config/modules/device/device_transport_config.h \
    $$PWD/config/modules/device/http_device_transport_config.h \
    $$PWD/controller/controller.h \
    $$PWD/controller/device/device_data_parser.h \
    $$PWD/controller/device/device_freshness_sentinel.h \
    $$PWD/controller/device/device_manager.h \
    $$PWD/controller/device/device_poller.h \
    $$PWD/controller/device/http_device_transport.h \
    $$PWD/controller/device/idevice_transport.h \
    $$PWD/controller/device/mock_device_transport.h \
    $$PWD/domain/device/device_types.h \
    $$PWD/ui/main_window.h \
