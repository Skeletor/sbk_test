#pragma once

#include "app/domain/device/device_types.h"

#include <QMetaType>

namespace Controller::Device {

struct DeviceTopology {
    Domain::DeviceList devices;
};

struct DeviceEventBatch {
    qint64 lastSequence = 0;
    Domain::DeviceEventList events;
};

}  // namespace Controller::Device

Q_DECLARE_METATYPE(Controller::Device::DeviceTopology)
Q_DECLARE_METATYPE(Controller::Device::DeviceEventBatch)
