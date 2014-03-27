#include "testblackcoremain.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackCoreTest;

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
    CBlackCoreTestMain::unitMain(argc, argv);

    // bye
    return 0;
}
