#include "samplesvectormatrix.h"

using namespace BlackMisc::Math;

/*
 * Run the samples
 */
int BlackMiscTest::CSamplesVectorMatrix::samples()
{
    CVector3D v1;
    CVector3D v2(1, 2, 3);
    qDebug() << v1 << "value:" << v2[2] << v2.length();
    v2 *= v2; // v2 * v2
    qDebug() << v2 << 2 *  v1 << v1 *v1;

    CMatrix3x3 m;
    CMatrix3x3 mr = m;
    qDebug() << m << mr;
    mr.setRandom();
    bool im;
    CMatrix3x3 mi = mr.inverse(im);
    CMatrix3x3 mid = mr * mi;
    qDebug() << mr << mi << mid << im;

    // bye
    qDebug() << "-----------------------------------------------";
    return 0;

}
