#include "app/application.h"
#include "app/config/modules/app_config.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication qtApplication(argc, argv);

    Config::AppConfig appConfig;

    Application application(appConfig);
    application.start();

    return qtApplication.exec();
}
