#include <QCoreApplication>
#include <QDebug>
#include "blackmisc/pqconstants.h"
#include "blackmisc/debug.h"

using namespace BlackMisc;

/*!
 * Sample tests
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // cases which must not work
    // CMeasurementUnit mu; //must not work
    // CLengthUnit du1(CAngleUnit::rad());
    CLengthUnit du2(CLengthUnit::cm());
    CLengthUnit du3(CLengthUnit::ft());
    const CLength d1(5.0, CLengthUnit::ft()); // 5 ft
    CLength d2(1, CLengthUnit::NM()); // 1NM
    CLength d3(1, CLengthUnit::km());
    CLength d4(d3);

    qDebug() << CLengthUnit::ft();
    qDebug() << d1 << d2 << d3 << d4;
    qDebug() << d1.valueRoundedWithUnit(CLengthUnit::ft(),5)
             << d2.valueRoundedWithUnit(CLengthUnit::km());
    qDebug() << d3.getUnit();


    d2.switchUnit(CLengthUnit::ft()); // now in ft
    d3 += d3; // 2km now
    d3 *= 1.5;// 3km now
    qDebug() << d2 << d3;

    CFrequency f1(1E6, CFrequencyUnit::Hz()); // 1MHz
    qDebug() << f1 << f1.valueRoundedWithUnit(CFrequencyUnit::MHz()) << f1.valueRoundedWithUnit(CFrequencyUnit::GHz(), 3);

    CSpeed s1 = CSpeed(100, CSpeedUnit::km_h());
    CSpeed s2 = CSpeed(1000,CSpeedUnit::ft_min());
    CSpeed s3 = CSpeed(s2);
    s3.switchUnit(CSpeedUnit::m_s());
    qDebug() << s1 << s1.convertedSiValueRoundedWithUnit() << s1.valueRoundedWithUnit(CSpeedUnit::NM_h());
    qDebug() << s2 << s3;

    CAngle a1(180, CAngleUnit::deg());
    CAngle a2(1.5 * CAngle::pi(), CAngleUnit::rad());
    a1 += a2;
    // a1 = d2; // must not work
    qDebug() << a1;
    a1.switchUnit(CAngleUnit::deg());
    // a2 += d1; // must not work
    a2 = a1 + a1;

    a2.switchUnit(CAngleUnit::deg());
    qDebug() << a1.unitValueRoundedWithUnit() << a1.piFactor();
    qDebug() << a2;

    CMass w1(1,CMassUnit::t());
    CMass w2(w1);
    w2.switchUnit(CMassUnit::lb());
    qDebug() << w1 << w1.valueRoundedWithUnit(CMassUnit::kg()) << w2;

    CPressure p1(1013.25, CPressureUnit::hPa());
    qDebug() << p1 << p1.valueRoundedWithUnit(CPressureUnit::psi()) << p1.valueRoundedWithUnit(CPressureUnit::inHg());

    CTemperature t1;
    CTemperature t2(20, CTemperatureUnit::C());
    CTemperature t3(1, CTemperatureUnit::F());
    qDebug() << t1 << t2 << t2.convertedSiValueRoundedWithUnit();
    qDebug() << t3 << t3.valueRoundedWithUnit(CTemperatureUnit::C());

    // some logging with CLogMessage
    bDebug << p1;
    bDebug << p1.getUnit() << p1.getUnit().getMultiplier();

    // some of the faults Mathew has pointed out,not longer possible
    // CAngleUnit::rad() = CAngleUnit::deg();
    // qDebug() << CAngleUnit::rad(); // wrong

    (t1 - t2).switchUnit(CTemperatureUnit::F()); // was not working since wrong return type const
    // CLengthUnit duA(CSpeedUnit::ft_min()); // no longer possible
    CLengthUnit duB(CLengthUnit::cm());
    qDebug() << duB;

    // bye
    return a.exec();
}
