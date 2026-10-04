#include "app/application.h"

#include "app/config/modules/app_config.h"

Application::Application(const Config::AppConfig& appConfig, QObject* parent)
    : QObject(parent)
    , m_appConfig(appConfig)
{
}

Application::~Application() = default;

void Application::start()
{
}
