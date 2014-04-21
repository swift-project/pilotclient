#include "testblacksimmain.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackSimTest;

/*!
 * Starter for test cases
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CBlackSimTestMain::unitMain(argc, argv);
    Q_UNUSED(a);

    // bye
    return 0;
}
