#pragma once

#include "app/controller/device/device_transport_types.h"

#include <QMetaType>
#include <QObject>
#include <QString>

namespace Controller::Device {

using RequestId = quint64;

enum class RequestKind {
    Devices,
    Poll,
};

enum class TransportErrorCode {
    Network,
    Timeout,
    HttpStatus,
    InvalidPayload,
    Aborted,
};

struct TransportError {
    RequestId requestId = 0;
    RequestKind requestKind = RequestKind::Devices;
    TransportErrorCode code = TransportErrorCode::Network;
    int httpStatus = 0;
    QString message;
};

class IDeviceTransport : public QObject {
    Q_OBJECT

public:
    explicit IDeviceTransport(QObject* parent = nullptr);
    ~IDeviceTransport() override;

    virtual void requestDevices(RequestId requestId) = 0;
    virtual void requestPoll(RequestId requestId, qint64 since) = 0;
    virtual void abortRequest(RequestKind requestKind) = 0;

signals:
    void topologyReceived(RequestId requestId, const DeviceTopology& topology);
    void eventBatchReceived(RequestId requestId, const DeviceEventBatch& batch);
    void requestFailed(const TransportError& error);
};

}  // namespace Controller::Device

Q_DECLARE_METATYPE(Controller::Device::RequestKind)
Q_DECLARE_METATYPE(Controller::Device::TransportErrorCode)
Q_DECLARE_METATYPE(Controller::Device::TransportError)
