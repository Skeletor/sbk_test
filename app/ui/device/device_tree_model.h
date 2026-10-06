#pragma once

#include "app/domain/device/device_types.h"

#include <QAbstractItemModel>
#include <QStringList>

#include <memory>

namespace Ui::Device {

class DeviceTreeModelPrivate;

class DeviceTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum class Column {
        DeviceOrMetric,
        Id,
        Value,
        State,
        Count,
    };

    explicit DeviceTreeModel(QObject* parent = nullptr);
    ~DeviceTreeModel() override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setDevices(const Domain::DeviceList& devices);
    void applyEvents(const Domain::DeviceEventList& events);
    void setDevicesUnreliable(const QStringList& deviceIds);

private:
    std::unique_ptr<DeviceTreeModelPrivate> m_private;
};

}  // namespace Ui::Device
