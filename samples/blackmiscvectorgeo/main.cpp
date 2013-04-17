#include "samplesvectormatrix.h"
#include "samplesgeo.h"
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
    return a.exec();
}
