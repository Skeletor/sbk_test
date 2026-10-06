#include "app/config/modules/ui/ui_config.h"

#include <QSettings>

namespace {

constexpr auto JournalMaximumEntriesKey = "Ui/JournalMaximumEntries";
constexpr auto DefaultJournalMaximumEntries = 1000;

}  // namespace

namespace Config::Ui {

UiConfig::UiConfig(QSettings& settings)
{
    if (!settings.contains(JournalMaximumEntriesKey)) {
        settings.setValue(JournalMaximumEntriesKey, DefaultJournalMaximumEntries);
    }

    m_journalMaximumEntries = settings.value(JournalMaximumEntriesKey).toInt();
}

int UiConfig::journalMaximumEntries() const
{
    return m_journalMaximumEntries;
}

}  // namespace Config::Ui
