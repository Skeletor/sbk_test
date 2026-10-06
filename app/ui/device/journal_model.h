#pragma once

#include "app/domain/device/device_types.h"

#include <QAbstractTableModel>

namespace Config::Ui {
class UiConfig;
}

namespace Ui::Device {

class JournalModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum class Column {
        Timestamp,
        Device,
        Type,
        Message,
        Count,
    };

    explicit JournalModel(const Config::Ui::UiConfig& config, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void appendEvents(const Domain::DeviceEventList& events);
    void clear();

private:
    Domain::DeviceEventList m_events;
    int m_maximumEntries = 0;
};

}  // namespace Ui::Device
