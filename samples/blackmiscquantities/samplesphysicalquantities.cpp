/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesphysicalquantities.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /**
     * Running the quantities
     */
    int CSamplesPhysicalQuantities::samples()
    {
        QVariant parsedPq = CPqString::parseToVariant("100 km/h");
        parsedPq = CPqString::parseToVariant("-33ft");
        parsedPq = CPqString::parseToVariant("666");
        CSpeed speedParsed = CPqString::parse<CSpeed>("111.33ft/s");
        CFrequency frequencyParsed = CPqString::parse<CFrequency>("122.8MHz");
        qDebug() << "parsed" << speedParsed << speedParsed.valueRoundedWithUnit(2, true) << frequencyParsed << frequencyParsed.valueRoundedWithUnit(2, true);

        // cases which must not work
        // CLengthUnit du1(CAngleUnit::rad());

        CLengthUnit lu1(CLengthUnit::cm());
        CLengthUnit lu2(CLengthUnit::ft());
        QString lu1s = lu1.toQString(true);
        QString lu2s = lu2.toQString(true);
        qDebug() << lu1 << lu2 << lu1s << lu2s << lu1.getName(true) << lu2.getName(true);
        const CLength l1(5.0, CLengthUnit::ft()); // 5 ft
        CLength l2(1, CLengthUnit::NM()); // 1NM
        CLength l3(1, CLengthUnit::km());
        CLength l4(l3);

        qDebug() << CLengthUnit::ft();
        qDebug() << l1 << l2 << l3 << l4;
        qDebug() << l1.valueRoundedWithUnit(CLengthUnit::ft(), 5)
                 << l2.valueRoundedWithUnit(CLengthUnit::km());
        qDebug() << l3.getUnit();

        l2.switchUnit(CLengthUnit::ft()); // now in ft
        l3 += l3; // 2km now
        l3 *= 1.5;// 3km now
        qDebug() << l2 << l3;

        l3 = l3 * 2;
        qDebug() << "doubled l3:" << l3;

        // more tests
        CFrequency f1(1E6, CFrequencyUnit::Hz()); // 1MHz
        qDebug() << f1 << f1.valueRoundedWithUnit(CFrequencyUnit::MHz()) << f1.valueRoundedWithUnit(CFrequencyUnit::GHz(), 3);

        CSpeed s1 = CSpeed(100, CSpeedUnit::km_h());
        CSpeed s2 = CSpeed(1000, CSpeedUnit::ft_min());
        CSpeed s3 = CSpeed(s2);
        s3.switchUnit(CSpeedUnit::m_s());
        qDebug() << s1 << s1.valueRoundedWithUnit(CSpeedUnit::defaultUnit()) << s1.valueRoundedWithUnit(CSpeedUnit::NM_h());
        qDebug() << s2 << s3;

        CAngle a1(180, CAngleUnit::deg());
        CAngle a2(1.5 * CAngle::PI(), CAngleUnit::rad());
        CAngle a3(180.5, CAngleUnit::deg());
        CAngle a4(35.4336, CAngleUnit::sexagesimalDeg()); // 35.72666
        a1 += a2;
        // a1 = d2; // must not work
        qDebug() << a1;
        a1.switchUnit(CAngleUnit::deg());
        // a2 += d1; // must not work
        a2 = a1 + a1;

        a2.switchUnit(CAngleUnit::deg());
        qDebug() << a1.valueRoundedWithUnit() << a1.piFactor();
        qDebug() << a2;
        a3.switchUnit(CAngleUnit::sexagesimalDeg());
        a4.switchUnit(CAngleUnit::deg());
        qDebug() << a3 << a4;

        CMass w1(1, CMassUnit::tonne());
        CMass w2(w1);
        w2.switchUnit(CMassUnit::lb());
        qDebug() << w1 << w1.valueRoundedWithUnit(CMassUnit::kg()) << w2;

        CPressure p1(1013.25, CPressureUnit::hPa());
        qDebug() << p1 << p1.valueRoundedWithUnit(CPressureUnit::psi()) << p1.valueRoundedWithUnit(CPressureUnit::inHg());

        CTemperature t1;
        CTemperature t2(20, CTemperatureUnit::C());
        CTemperature t3(1, CTemperatureUnit::F());
        qDebug() << t1 << t2 << t2.valueRoundedWithUnit(CTemperatureUnit::defaultUnit(), -1, true);
        qDebug() << t3.valueRoundedWithUnit(CTemperatureUnit::F(), -1, true)  << t3.valueRoundedWithUnit(CTemperatureUnit::C(), -1, true) << "I18N/UTF";

        (t1 - t2).switchUnit(CTemperatureUnit::F()); // was not working since wrong return type const
        // CLengthUnit duA(CSpeedUnit::ft_min()); // no longer possible
        CLengthUnit duB(CLengthUnit::cm());
        qDebug() << duB;

        CTime ti1(1, CTimeUnit::h());
        CTime ti2(ti1);
        ti2.switchUnit(CTimeUnit::ms());
        CTime ti3(1.0101, CTimeUnit::hms());
        CTime ti4(1.15, CTimeUnit::hrmin());
        CTime ti5(1.30, CTimeUnit::minsec());
        CTime ti6("12:30");
        CTime ti7("20s");
        CTime ti8("12:30:40");

        qDebug() << ti1 << ti2 << ti3 << ti4 << ti5;
        qDebug() << ti6 << ti7 << ti8;

        CAcceleration ac1(10, CAccelerationUnit::m_s2());
        qDebug() << ac1 << ac1.toQString(true) << ac1.valueRoundedWithUnit(-1, true) << "I18N/UTF";

        // bye
        qDebug() << "-----------------------------------------------";
        return 0;
    }

} // namespace
