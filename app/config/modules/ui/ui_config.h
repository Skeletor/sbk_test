#pragma once

class QSettings;

namespace Config::Ui {

class UiConfig {
public:
    explicit UiConfig(QSettings& settings);

    int journalMaximumEntries() const;

private:
    int m_journalMaximumEntries = 0;
};

}  // namespace Config::Ui
