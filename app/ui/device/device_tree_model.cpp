#include "app/ui/device/device_tree_model.h"

#include <QHash>

#include <memory>
#include <vector>

namespace {

const auto UnknownDeviceName = QStringLiteral("<UNKNOWN>");

QString getStateText(Domain::DeviceState state)
{
    switch (state) {
    case Domain::DeviceState::Unknown:
        return Ui::Device::DeviceTreeModel::tr("Unknown");

    case Domain::DeviceState::Online:
        return Ui::Device::DeviceTreeModel::tr("Online");

    case Domain::DeviceState::Offline:
        return Ui::Device::DeviceTreeModel::tr("Offline");

    case Domain::DeviceState::Unreliable:
        return Ui::Device::DeviceTreeModel::tr("Unreliable");
    }

    return {};
}

}  // namespace

namespace Ui::Device {

class DeviceTreeModelPrivate {
public:
    enum class NodeKind {
        Device,
        Metric,
    };

    struct Node {
        explicit Node(NodeKind kind)
            : kind(kind)
        {
        }

        NodeKind kind;
        int row = 0;
    };

    struct DeviceNode;

    struct MetricNode : Node {
        MetricNode(const QString& name, double value, DeviceNode* device)
            : Node(NodeKind::Metric)
            , name(name)
            , value(value)
            , device(device)
        {
        }

        QString name;
        double value = 0.0;
        DeviceNode* device = nullptr;
    };

    struct DeviceNode : Node {
        explicit DeviceNode(const Domain::DeviceInfo& info)
            : Node(NodeKind::Device)
        {
            snapshot.info = info;
        }

        Domain::DeviceSnapshot snapshot;
        std::vector<std::unique_ptr<MetricNode>> metrics;
        QHash<QString, MetricNode*> metricsByName;
    };

    DeviceNode* getDevice(const QString& deviceId) const
    {
        return devicesById.value(deviceId, nullptr);
    }

    DeviceNode* appendDevice(const Domain::DeviceInfo& info)
    {
        auto device = std::make_unique<DeviceNode>(info);
        device->row = static_cast<int>(devices.size());
        DeviceNode* devicePointer = device.get();
        devices.push_back(std::move(device));
        devicesById.insert(info.id, devicePointer);
        return devicePointer;
    }

    std::vector<std::unique_ptr<DeviceNode>> devices;
    QHash<QString, DeviceNode*> devicesById;
};

DeviceTreeModel::DeviceTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_private(std::make_unique<DeviceTreeModelPrivate>())
{
}

DeviceTreeModel::~DeviceTreeModel() = default;

QModelIndex DeviceTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column, m_private->devices[static_cast<std::size_t>(row)].get());
    }

    if (parent.column() != 0) {
        return {};
    }

    auto* parentNode = static_cast<DeviceTreeModelPrivate::Node*>(parent.internalPointer());
    if (!parentNode || parentNode->kind != DeviceTreeModelPrivate::NodeKind::Device) {
        return {};
    }

    auto* device = static_cast<DeviceTreeModelPrivate::DeviceNode*>(parentNode);
    return createIndex(row, column, device->metrics[static_cast<std::size_t>(row)].get());
}

QModelIndex DeviceTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return {};
    }

    auto* node = static_cast<DeviceTreeModelPrivate::Node*>(index.internalPointer());
    if (!node || node->kind == DeviceTreeModelPrivate::NodeKind::Device) {
        return {};
    }

    auto* metric = static_cast<DeviceTreeModelPrivate::MetricNode*>(node);
    return createIndex(metric->device->row, 0, metric->device);
}

int DeviceTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(m_private->devices.size());
    }

    if (parent.column() != 0) {
        return 0;
    }

    auto* node = static_cast<DeviceTreeModelPrivate::Node*>(parent.internalPointer());
    if (!node || node->kind != DeviceTreeModelPrivate::NodeKind::Device) {
        return 0;
    }

    auto* device = static_cast<DeviceTreeModelPrivate::DeviceNode*>(node);
    return static_cast<int>(device->metrics.size());
}

int DeviceTreeModel::columnCount(const QModelIndex&) const
{
    return static_cast<int>(Column::Count);
}

QVariant DeviceTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto* node = static_cast<DeviceTreeModelPrivate::Node*>(index.internalPointer());
    if (!node) {
        return {};
    }

    const Column column = static_cast<Column>(index.column());
    if (role == Qt::TextAlignmentRole && column == Column::Value) {
        return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }

    if (node->kind == DeviceTreeModelPrivate::NodeKind::Device) {
        auto* device = static_cast<DeviceTreeModelPrivate::DeviceNode*>(node);
        const Domain::DeviceSnapshot& snapshot = device->snapshot;

        if (role != Qt::DisplayRole) {
            return {};
        }

        switch (column) {
        case Column::DeviceOrMetric:
            return snapshot.info.name.isEmpty() ? UnknownDeviceName : snapshot.info.name;

        case Column::Id:
            return snapshot.info.id;

        case Column::Value:
            return {};

        case Column::State:
            return getStateText(snapshot.state);

        case Column::Count:
            return {};
        }
    }

    auto* metric = static_cast<DeviceTreeModelPrivate::MetricNode*>(node);
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (column) {
    case Column::DeviceOrMetric:
        return metric->name;

    case Column::Value:
        return metric->value;

    case Column::Id:
    case Column::State:
    case Column::Count:
        return {};
    }

    return {};
}

QVariant DeviceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (static_cast<Column>(section)) {
    case Column::DeviceOrMetric:
        return tr("Device / metric");

    case Column::Id:
        return tr("ID");

    case Column::Value:
        return tr("Value");

    case Column::State:
        return tr("State");

    case Column::Count:
        return {};
    }

    return {};
}

void DeviceTreeModel::setDevices(const Domain::DeviceList& devices)
{
    for (const Domain::DeviceInfo& info : devices) {
        DeviceTreeModelPrivate::DeviceNode* device = m_private->getDevice(info.id);
        if (device) {
            if (device->snapshot.info.name != info.name) {
                device->snapshot.info.name = info.name;
                emit dataChanged(
                    index(device->row, static_cast<int>(Column::DeviceOrMetric)),
                    index(device->row, static_cast<int>(Column::Id)));
            }

            continue;
        }

        const int row = static_cast<int>(m_private->devices.size());
        beginInsertRows({}, row, row);
        m_private->appendDevice(info);
        endInsertRows();
    }
}

void DeviceTreeModel::applyEvents(const Domain::DeviceEventList& events)
{
    for (const Domain::DeviceEvent& event : events) {
        DeviceTreeModelPrivate::DeviceNode* device = m_private->getDevice(event.deviceId);
        if (!device) {
            const int deviceRow = static_cast<int>(m_private->devices.size());
            beginInsertRows({}, deviceRow, deviceRow);
            device = m_private->appendDevice({event.deviceId, {}});
            endInsertRows();
        }

        Domain::DeviceState state = Domain::DeviceState::Online;
        if (event.type == Domain::EventType::Offline) {
            state = Domain::DeviceState::Offline;
        }

        if (device->snapshot.state != state) {
            device->snapshot.state = state;
            emit dataChanged(
                index(device->row, static_cast<int>(Column::State)),
                index(device->row, static_cast<int>(Column::State)));
        }

        device->snapshot.lastMessage = event.message;

        if (event.type != Domain::EventType::Value || !event.metric || !event.value) {
            continue;
        }

        DeviceTreeModelPrivate::MetricNode* metric = device->metricsByName.value(*event.metric, nullptr);
        if (!metric) {
            const int metricRow = static_cast<int>(device->metrics.size());
            const QModelIndex deviceIndex = index(device->row, 0);
            beginInsertRows(deviceIndex, metricRow, metricRow);
            auto newMetric = std::make_unique<DeviceTreeModelPrivate::MetricNode>(*event.metric, *event.value, device);
            newMetric->row = metricRow;
            metric = newMetric.get();
            device->metrics.push_back(std::move(newMetric));
            device->metricsByName.insert(metric->name, metric);
            device->snapshot.metrics.insert(metric->name, metric->value);
            endInsertRows();
            continue;
        }

        if (metric->value != *event.value) {
            metric->value = *event.value;
            device->snapshot.metrics.insert(metric->name, metric->value);
            const QModelIndex deviceIndex = index(device->row, 0);
            emit dataChanged(
                index(metric->row, static_cast<int>(Column::Value), deviceIndex),
                index(metric->row, static_cast<int>(Column::Value), deviceIndex));
        }
    }
}

void DeviceTreeModel::setDevicesUnreliable(const QStringList& deviceIds)
{
    for (const QString& deviceId : deviceIds) {
        DeviceTreeModelPrivate::DeviceNode* device = m_private->getDevice(deviceId);
        if (!device || device->snapshot.state == Domain::DeviceState::Unreliable) {
            continue;
        }

        device->snapshot.state = Domain::DeviceState::Unreliable;
        emit dataChanged(
            index(device->row, static_cast<int>(Column::State)),
            index(device->row, static_cast<int>(Column::State)));
    }
}

}  // namespace Ui::Device
