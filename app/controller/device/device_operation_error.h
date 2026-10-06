#pragma once

#include <QMetaType>
#include <QString>

namespace Controller::Device {

enum class DeviceOperation {
    LoadTopology,
    PollEvents,
};

enum class DeviceOperationErrorCode {
    Communication,
    Timeout,
    InvalidResponse,
    Cancelled,
};

struct DeviceOperationError {
    DeviceOperation operation = DeviceOperation::LoadTopology;
    DeviceOperationErrorCode code = DeviceOperationErrorCode::Communication;
    QString message;
};

}  // namespace Controller::Device

Q_DECLARE_METATYPE(Controller::Device::DeviceOperation)
Q_DECLARE_METATYPE(Controller::Device::DeviceOperationErrorCode)
Q_DECLARE_METATYPE(Controller::Device::DeviceOperationError)
