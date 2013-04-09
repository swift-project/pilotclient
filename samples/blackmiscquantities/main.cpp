#include <QCoreApplication>
#include "samplesphysicalquantities.h"
#include "samplesaviation.h"

using namespace BlackMisc;
using namespace BlackMiscTest;

/*!
 * Sample tests
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CSamplesPhysicalQuantities::samples();
    CSamplesAviation::samples();

    return a.exec();
}
