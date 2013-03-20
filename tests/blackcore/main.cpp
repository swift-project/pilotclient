#include <QCoreApplication>
#include <QDebug>
#include "testmain.h"

using namespace BlackCoreTest;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestMain::unitMain(argc, argv);

    // bye
    return a.exec();
}
