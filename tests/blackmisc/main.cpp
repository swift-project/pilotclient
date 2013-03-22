#include <QCoreApplication>
#include <QDebug>
#include "testmain.h"

using namespace BlackMiscTest;

/*!
 * Starter for testcases
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestMain::unitMain(argc, argv);

    // bye
    return a.exec();
}
