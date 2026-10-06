#include "app/ui/device/journal_model.h"
#include "app/config/modules/ui/ui_config.h"

namespace {

const auto TimestampFormat = QStringLiteral("HH:mm:ss");

QString getEventTypeText(Domain::EventType type)
{
    switch (type) {
    case Domain::EventType::Value:
        return Ui::Device::JournalModel::tr("Value");

    case Domain::EventType::Online:
        return Ui::Device::JournalModel::tr("Online");

    case Domain::EventType::Offline:
        return Ui::Device::JournalModel::tr("Offline");
    }

    return {};
}

}  // namespace

namespace Ui::Device {

JournalModel::JournalModel(
    const Config::Ui::UiConfig& config,
    QObject* parent)
    : QAbstractTableModel(parent)
    , m_maximumEntries(config.journalMaximumEntries())
{
}

int JournalModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_events.size();
}

int JournalModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(Column::Count);
}

QVariant JournalModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_events.size()) {
        return {};
    }

    const Domain::DeviceEvent& event = m_events.at(index.row());
    const Column column = static_cast<Column>(index.column());

    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (column) {
    case Column::Timestamp:
        return event.timestamp.toString(TimestampFormat);

    case Column::Device:
        return event.deviceId;

    case Column::Type:
        return getEventTypeText(event.type);

    case Column::Message:
        return event.message;

    case Column::Count:
        return {};
    }

    return {};
}

QVariant JournalModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (static_cast<Column>(section)) {
    case Column::Timestamp:
        return tr("Timestamp");

    case Column::Device:
        return tr("Device");

    case Column::Type:
        return tr("Type");

    case Column::Message:
        return tr("Message");

    case Column::Count:
        return {};
    }

    return {};
}

void JournalModel::appendEvents(const Domain::DeviceEventList& events)
{
    if (events.isEmpty()) {
        return;
    }

    const int firstRow = m_events.size();
    const int lastRow = firstRow + events.size() - 1;
    beginInsertRows({}, firstRow, lastRow);
    m_events += events;
    endInsertRows();

    if (m_maximumEntries <= 0 || m_events.size() <= m_maximumEntries) {
        return;
    }

    const int eventsToRemove = m_events.size() - m_maximumEntries;
    beginRemoveRows({}, 0, eventsToRemove - 1);
    m_events.remove(0, eventsToRemove);
    endRemoveRows();
}

void JournalModel::clear()
{
    if (m_events.isEmpty()) {
        return;
    }

    beginRemoveRows({}, 0, m_events.size() - 1);
    m_events.clear();
    endRemoveRows();
}

}  // namespace Ui::Device
