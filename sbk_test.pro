QT += core gui network widgets

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = sbk_test

INCLUDEPATH += $$PWD

SOURCES += \
    app/controller/device/idevice_transport.cpp \
    main.cpp \
    app/application.cpp \
    app/config/modules/app_config.cpp \
    app/config/modules/controller/controller_config.cpp \
    app/config/modules/device/device_freshness_config.cpp \
    app/config/modules/device/device_poller_config.cpp \
    app/config/modules/device/http_device_transport_config.cpp \
    app/ui/main_window.cpp \

HEADERS += \
    app/application.h \
    app/config/modules/app_config.h \
    app/config/modules/controller/controller_config.h \
    app/config/modules/device/device_freshness_config.h \
    app/config/modules/device/device_poller_config.h \
    app/config/modules/device/http_device_transport_config.h \
    app/controller/device/idevice_transport.h \
    app/domain/device/device_types.h \
    app/ui/main_window.h \
