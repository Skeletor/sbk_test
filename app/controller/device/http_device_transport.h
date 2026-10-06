#pragma once

#include "app/controller/device/idevice_transport.h"

#include <QByteArray>
#include <QPointer>

namespace Config::Device {
class HttpDeviceTransportConfig;
}

class QNetworkAccessManager;
class QNetworkReply;
class QUrl;

namespace Controller::Device {

class HttpDeviceTransport : public IDeviceTransport {
    Q_OBJECT

public:
    explicit HttpDeviceTransport(const Config::Device::HttpDeviceTransportConfig& config, QObject* parent = nullptr);

    void requestDevices(RequestId requestId) override;
    void requestPoll(RequestId requestId, qint64 since) override;
    void abortRequest(RequestKind requestKind) override;

private:
    struct PendingRequest {
        QPointer<QNetworkReply> reply;
        RequestId requestId = 0;
        bool timedOut = false;
    };

    void startRequest(RequestKind requestKind, RequestId requestId, const QUrl& url, int timeoutMs);
    void handleRequestFinished(RequestKind requestKind);
    void emitParsedResponse(RequestKind requestKind, RequestId requestId, const QByteArray& body);
    PendingRequest& getPendingRequest(RequestKind requestKind);

private:
    const Config::Device::HttpDeviceTransportConfig& m_config;
    QNetworkAccessManager* m_network = nullptr;
    PendingRequest m_devicesRequest;
    PendingRequest m_pollRequest;
};

}  // namespace Controller::Device
