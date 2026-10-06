#include "app/controller/device/http_device_transport.h"

#include "app/config/modules/device/http_device_transport_config.h"
#include "app/controller/device/device_data_parser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrlQuery>

namespace {

const auto DevicesEndpoint = QStringLiteral("/devices");
const auto PollEndpoint = QStringLiteral("/poll");
const auto SinceQueryKey = QStringLiteral("since");
constexpr auto HttpOkStatus = 200;

}  // namespace

namespace Controller::Device {

HttpDeviceTransport::HttpDeviceTransport(
    const Config::Device::HttpDeviceTransportConfig& config,
    QObject* parent)
    : IDeviceTransport(parent)
    , m_config(config)
    , m_network(new QNetworkAccessManager(this))
{
}

void HttpDeviceTransport::requestDevices(RequestId requestId)
{
    startRequest(
        RequestKind::Devices,
        requestId,
        m_config.baseUrl().resolved(QUrl(DevicesEndpoint)),
        m_config.devicesRequestTimeoutMs());
}

void HttpDeviceTransport::requestPoll(RequestId requestId, qint64 since)
{
    QUrl url = m_config.baseUrl().resolved(QUrl(PollEndpoint));
    QUrlQuery query;
    query.addQueryItem(SinceQueryKey, QString::number(since));
    url.setQuery(query);

    startRequest(RequestKind::Poll, requestId, url, m_config.pollRequestTimeoutMs());
}

void HttpDeviceTransport::abortRequest(RequestKind requestKind)
{
    PendingRequest& request = getPendingRequest(requestKind);
    if (!request.reply) {
        return;
    }

    QNetworkReply* reply = request.reply;
    const RequestId requestId = request.requestId;
    request.reply.clear();
    request.requestId = 0;
    request.timedOut = false;

    disconnect(reply, nullptr, this, nullptr);
    reply->abort();
    reply->deleteLater();

    emit requestFailed({
        requestId,
        requestKind,
        TransportErrorCode::Aborted,
        0,
        QStringLiteral("Request was aborted"),
    });
}

void HttpDeviceTransport::startRequest(RequestKind requestKind, RequestId requestId, const QUrl& url, int timeoutMs)
{
    PendingRequest& request = getPendingRequest(requestKind);
    if (request.reply) {
        emit requestFailed({
            requestId,
            requestKind,
            TransportErrorCode::Aborted,
            0,
            QStringLiteral("Another request of this kind is already active"),
        });
        return;
    }

    request.requestId = requestId;
    request.timedOut = false;
    request.reply = m_network->get(QNetworkRequest(url));

    QNetworkReply* reply = request.reply;
    connect(reply, &QNetworkReply::finished, this, [this, requestKind]() {
        handleRequestFinished(requestKind);
    });
    QTimer::singleShot(timeoutMs, reply, [this, reply, requestKind, requestId]() {
        PendingRequest& activeRequest = getPendingRequest(requestKind);
        if (activeRequest.reply == reply && activeRequest.requestId == requestId) {
            activeRequest.timedOut = true;
            reply->abort();
        }
    });
}

void HttpDeviceTransport::handleRequestFinished(RequestKind requestKind)
{
    PendingRequest& request = getPendingRequest(requestKind);
    if (!request.reply) {
        return;
    }

    QNetworkReply* reply = request.reply;
    const RequestId requestId = request.requestId;
    const bool timedOut = request.timedOut;
    request.reply.clear();
    request.requestId = 0;
    request.timedOut = false;

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QNetworkReply::NetworkError networkError = reply->error();
    const QString errorMessage = reply->errorString();
    const QByteArray body = reply->readAll();
    reply->deleteLater();

    if (timedOut) {
        const QString message = requestKind == RequestKind::Devices
            ? QStringLiteral("Devices request timed out")
            : QStringLiteral("Poll request timed out");

        emit requestFailed({requestId, requestKind, TransportErrorCode::Timeout, status, message});
    } else if (status != 0 && status != HttpOkStatus) {
        emit requestFailed({
            requestId,
            requestKind,
            TransportErrorCode::HttpStatus,
            status,
            QStringLiteral("Unexpected HTTP status: %1").arg(status),
        });
    } else if (networkError != QNetworkReply::NoError) {
        emit requestFailed({
            requestId,
            requestKind,
            TransportErrorCode::Network,
            status,
            errorMessage,
        });
    } else {
        emitParsedResponse(requestKind, requestId, body);
    }
}

void HttpDeviceTransport::emitParsedResponse(RequestKind requestKind, RequestId requestId, const QByteArray& body)
{
    if (requestKind == RequestKind::Devices) {
        const std::optional<DeviceTopology> topology = DeviceDataParser::parseDevices(body);
        if (topology) {
            emit topologyReceived(requestId, *topology);
            return;
        }
    } else {
        const std::optional<DeviceEventBatch> batch = DeviceDataParser::parsePoll(body);
        if (batch) {
            emit eventBatchReceived(requestId, *batch);
            return;
        }
    }

    emit requestFailed({
        requestId,
        requestKind,
        TransportErrorCode::InvalidPayload,
        HttpOkStatus,
        QStringLiteral("Invalid device API response"),
    });
}

HttpDeviceTransport::PendingRequest& HttpDeviceTransport::getPendingRequest(RequestKind requestKind)
{
    return requestKind == RequestKind::Devices
        ? m_devicesRequest
        : m_pollRequest;
}

}  // namespace Controller::Device
