/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "samplesphysicalquantities.h"
#include "blackmisc/variant.h"

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMiscTest
{

    /**
     * Running the quantities
     */
    int CSamplesPhysicalQuantities::samples(QTextStream &out)
    {
        // parsing
        CSpeed parsedPq1 = CPqString::parseToVariant("100.123 km/h").value<CSpeed>();
        CLength parsedPq2 = CPqString::parseToVariant("-33.123ft").value<CLength>();
        CVariant parsedPq3 = CPqString::parseToVariant("666");
        out << "parsed" << " " << parsedPq1 << " " << parsedPq2 << " " << parsedPq3 << endl;

        CSpeed speedParsed = CPqString::parse<CSpeed>("111.33ft/s");
        CFrequency frequencyParsed = CPqString::parse<CFrequency>("122.8MHz");
        out << "parsed" << " " << speedParsed << " " << speedParsed.valueRoundedWithUnit(2, true) << frequencyParsed << " " << frequencyParsed.valueRoundedWithUnit(2, true) << endl;

        // the time clasas
        CTime time1;
        time1.parseFromString("11:30"); // hhmm
        out << time1 << " " << time1.toQTime().toString() << " " << time1.formattedHrsMin() << endl;

        CTime time2;
        time2.parseFromString("-11:30"); // hhmm
        out << time2 << " " << time2.toQTime().toString() << " " << time2.formattedHrsMin() << endl;

        time1 += time2;
        out << "0 time: " << time1 << " " << time1.toQTime().toString() << " " << time1.formattedHrsMin() << endl;

        // standard tests
        CLengthUnit lu1(CLengthUnit::cm());
        CLengthUnit lu2(CLengthUnit::ft());
        QString lu1s = lu1.toQString(true);
        QString lu2s = lu2.toQString(true);
        out << "units: " << lu1 << " " << lu2 << " " << lu1s << " " << lu2s << " " << lu1.getName(true) << " " << lu2.getName(true) << endl;
        const CLength l1(5.0, CLengthUnit::ft()); // 5 ft
        CLength l2(1, CLengthUnit::NM()); // 1NM
        CLength l3(1, CLengthUnit::km());
        CLength l4(l3);

        out << CLengthUnit::ft() << endl;
        out << l1 << " " << l2 << " " << l3 << " " << l4 << endl;
        out << l1.valueRoundedWithUnit(CLengthUnit::ft(), 5)
            << " " << l2.valueRoundedWithUnit(CLengthUnit::km()) << endl;
        out << l3.getUnit() << endl;

        l2.switchUnit(CLengthUnit::ft()); // now in ft
        l3 += l3; // 2km now
        l3 *= 1.5;// 3km now
        out << l2 << " " << l3 << endl;

        l3 = l3 * 2;
        out << "doubled l3: " << l3 << endl;

        // more tests
        CFrequency f1(1E6, CFrequencyUnit::Hz()); // 1MHz
        out << f1 << " " << f1.valueRoundedWithUnit(CFrequencyUnit::MHz()) << " " << f1.valueRoundedWithUnit(CFrequencyUnit::GHz(), 3);

        CSpeed s1 = CSpeed(100, CSpeedUnit::km_h());
        CSpeed s2 = CSpeed(1000, CSpeedUnit::ft_min());
        CSpeed s3 = CSpeed(s2);
        s3.switchUnit(CSpeedUnit::m_s());
        out << s1 << " " << s1.valueRoundedWithUnit(CSpeedUnit::defaultUnit()) << " " << s1.valueRoundedWithUnit(CSpeedUnit::NM_h());
        out << s2 << " " << s3 << endl;

        CAngle a1(180, CAngleUnit::deg());
        CAngle a2(1.5 * CAngle::PI(), CAngleUnit::rad());
        CAngle a3(180.5, CAngleUnit::deg());
        CAngle a4(35.4336, CAngleUnit::sexagesimalDeg()); // 35.72666
        a1 += a2;
        // a1 = d2; // must not work
        out << a1;
        a1.switchUnit(CAngleUnit::deg());
        // a2 += d1; // must not work
        a2 = a1 + a1;

        a2.switchUnit(CAngleUnit::deg());
        out << a1.valueRoundedWithUnit() << " " << a1.piFactor() << endl;
        out << a2 << endl;
        a3.switchUnit(CAngleUnit::sexagesimalDeg());
        a4.switchUnit(CAngleUnit::deg());
        out << a3 << " " << a4 << endl;

        CMass w1(1, CMassUnit::tonne());
        CMass w2(w1);
        w2.switchUnit(CMassUnit::lb());
        out << w1 << " " << w1.valueRoundedWithUnit(CMassUnit::kg()) << " " << w2 << endl;

        CPressure p1(1013.25, CPressureUnit::hPa());
        out << p1 << " " << p1.valueRoundedWithUnit(CPressureUnit::psi()) << " " << p1.valueRoundedWithUnit(CPressureUnit::inHg()) << endl;

        CTemperature t1;
        CTemperature t2(20, CTemperatureUnit::C());
        CTemperature t3(1, CTemperatureUnit::F());
        out << t1 << " " << t2 << " " << t2.valueRoundedWithUnit(CTemperatureUnit::defaultUnit(), -1, true);
        out << t3.valueRoundedWithUnit(CTemperatureUnit::F(), -1, true)  << " " << t3.valueRoundedWithUnit(CTemperatureUnit::C(), -1, true) << " " << "I18N/UTF" << endl;

        (t1 - t2).switchUnit(CTemperatureUnit::F()); // was not working since wrong return type const
        // CLengthUnit duA(CSpeedUnit::ft_min()); // no longer possible
        CLengthUnit duB(CLengthUnit::cm());
        out << duB << endl;

        CTime ti1(1, CTimeUnit::h());
        CTime ti2(ti1);
        ti2.switchUnit(CTimeUnit::ms());
        CTime ti3(1.0101, CTimeUnit::hms());
        CTime ti4(1.15, CTimeUnit::hrmin());
        CTime ti5(1.30, CTimeUnit::minsec());
        CTime ti6("12:30");
        CTime ti7("20s");
        CTime ti8("12:30:40");

        out << ti1 << " " << ti2 << " " << ti3 << " " << ti4 << " " << ti5 << endl;
        out << ti6 << " " << ti7 << " " << ti8 << endl;

        CAcceleration ac1(10, CAccelerationUnit::m_s2());
        out << ac1 << " " << ac1.toQString(true) << " " << ac1.valueRoundedWithUnit(-1, true) << " " << "I18N/UTF" << endl;

        // bye
        out << "-----------------------------------------------" << endl;
        return 0;
    }

} // namespace
