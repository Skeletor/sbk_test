#include "tests/device_data_parser_test.h"

#include <QTest>

int main(int argc, char* argv[])
{
    int exitCode = 0;

    DeviceDataParserTest deviceDataParserTest;
    exitCode |= QTest::qExec(&deviceDataParserTest, argc, argv);

    return exitCode;
}
