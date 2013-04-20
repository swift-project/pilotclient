#include "samplesvectormatrix.h"
#include "samplesgeo.h"
#include "samplesgeodetictoecef.h"
#include <QCoreApplication>

/*!
 * \brief Main entry
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMiscTest::CSamplesVectorMatrix::samples();
    BlackMiscTest::CSamplesGeo::samples();
    BlackMiscTest::CSamplesGeodeticToEcef::samples();
    return a.exec();
}
