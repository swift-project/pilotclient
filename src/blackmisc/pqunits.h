/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQUNITS_H
#define BLACKMISC_PQUNITS_H
#include "blackmisc/pqbase.h"
#include <math.h>

//
// Used with the template for quantities. This is the reason for
// having all units in one file, since template requires concrete instantiations
//
namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Specialized class for distance units (meter, foot, nautical miles).
 */
class CLengthUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor Distance unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CLengthUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "distance", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon)
    {
        // void
    }
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CLengthUnit(const CLengthUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Meter m
     * \return
     */
    static const CLengthUnit &m()
    {
        static CLengthUnit m("meter", "m", true, true);
        return m;
    }
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static const CLengthUnit &NM()
    {
        static CLengthUnit NM("nautical miles", "NM", false, false, 1000.0 * 1.85200, CMeasurementPrefix::One(), 3);
        return NM;
    }
    /*!
     * \brief Foot ft
     * \return
     */
    static const CLengthUnit &ft()
    {
        static CLengthUnit ft("foot", "ft", false, false, 0.3048, CMeasurementPrefix::One(), 0);
        return ft;
    }
    /*!
     * \brief Kilometer km
     * \return
     */
    static const CLengthUnit &km()
    {
        static CLengthUnit km("kilometer", "km", true, false, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 3);
        return km;
    }
    /*!
     * \brief Centimeter cm
     * \return
     */
    static const CLengthUnit &cm()
    {
        static CLengthUnit cm("centimeter", "cm", true, false, CMeasurementPrefix::c().getFactor(), CMeasurementPrefix::c(), 1);
        return cm;
    }
};

/*!
 * \brief Specialized class for angles (degrees, radian).
 * \author KWB, MS
 */
class CAngleUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor angle units: Radian, degree
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CAngleUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0,
               const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2,
               double epsilon = 1E-9, UnitConverter converterToSi = nullptr,  UnitConverter converterFromSi = nullptr) :
        CMeasurementUnit(name, unitName, "angle", isSIUnit, false, conversionFactorToSI,
                         mulitplier, displayDigits, epsilon, converterToSi, converterFromSi)
    {
        // void
    }
    /*!
     * \brief Special conversion for sexagesimal degrees
     * \param value
     * \return
     */
    static double conversionSexagesimalToSi(const CMeasurementUnit &angleUnit, double value);
    /*!
     * \brief Special conversion for sexagesimal degrees
     * \param value
     * \return
     */
    static double conversionSexagesimalFromSi(const CMeasurementUnit &angleUnit, double value);

public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CAngleUnit(const CAngleUnit &otherUnit) : CMeasurementUnit(otherUnit) { }
    /*!
     * \brief Special conversion to QString for sexagesimal degrees.
     * \param value
     * \param digits
     * \return
     */
    virtual QString toQStringRounded(double value, int digits) const;
    /*!
     * \brief Radians
     * \return
     */
    static const CAngleUnit &rad()
    {
        static CAngleUnit rad("radian", "rad", true);
        return rad;
    }
    /*!
     * \brief Degrees
     * \return
     */
    static const CAngleUnit &deg()
    {
        static CAngleUnit deg("degree", "°", false, M_PI / 180);
        return deg;
    }
    /*!
     * \brief Sexagesimal degree (degree, minute, seconds)
     * \return
     */
    static const CAngleUnit &sexagesimalDeg()
    {
        static CAngleUnit deg("segadecimal degree", "°", false, M_PI / 180,
                              CMeasurementPrefix::One(), 0, 1E-9, CAngleUnit::conversionSexagesimalToSi, CAngleUnit::conversionSexagesimalFromSi); return deg;
    }
};

/*!
 * \brief Specialized class for frequency (hertz, mega hertz, kilo hertz).
 * \author KWB, MS
 */
class CFrequencyUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief CFrequencyUnit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CFrequencyUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "frequency", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CFrequencyUnit(const CFrequencyUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Hertz
     * \return
     */
    static const CFrequencyUnit &Hz()
    {
        static CFrequencyUnit Hz("hertz", "Hz", true);
        return Hz;
    }
    /*!
     * \brief Kilohertz
     * \return
     */
    static const CFrequencyUnit &kHz()
    {
        static CFrequencyUnit kHz("kilohertz", "kHz", true, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 1);
        return kHz;
    }
    /*!
     * \brief Megahertz
     * \return
     */
    static const CFrequencyUnit &MHz()
    {
        static CFrequencyUnit MHz("megahertz", "MHz", false, CMeasurementPrefix::M().getFactor(), CMeasurementPrefix::M(), 2);
        return MHz;
    }
    /*!
     * \brief Gigahertz
     * \return
     */
    static const CFrequencyUnit &GHz()
    {
        static CFrequencyUnit GHz("gigahertz", "GHz", true, CMeasurementPrefix::G().getFactor(), CMeasurementPrefix::G(), 2);
        return GHz;
    }
};

/*!
 * \brief Specialized class for mass units (kg, lbs).
 * \author KWB, MS
 */
class CMassUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor mass units
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CMassUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "mass", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CMassUnit(const CMassUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Kilogram, SI base unit
     * \return
     */
    static const CMassUnit &kg()
    {
        static CMassUnit kg("kilogram", "kg", true, true, 1.0, CMeasurementPrefix::k(), 1);
        return kg;
    }
    /*!
     * \brief Gram, SI unit
     * \return
     */
    static const CMassUnit &g()
    {
        static CMassUnit g("gram", "g", true, false, 1.0 / 1000.0, CMeasurementPrefix::One(), 0);
        return g;
    }
    /*!
     * \brief Tonne, aka metric tonne (1000kg)
     * \return
     */
    static const CMassUnit &t()
    {
        static CMassUnit t("tonne", "t", false, false, 1000.0, CMeasurementPrefix::One(), 3);
        return t;
    }
    /*!
     * \brief Pound, aka mass pound
     * \return
     */
    static const CMassUnit &lb()
    {
        static CMassUnit lbs("pound", "lb", false, false, 0.45359237, CMeasurementPrefix::One(), 1);
        return lbs;
    }
};

/*!
 * \brief Specialized class for pressure (psi, hPa, bar).
 * \author KWB, MS
 */
class CPressureUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Pressure unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CPressureUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "frequency", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CPressureUnit(const CPressureUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Pascal
     * \return
     */
    static const CPressureUnit &Pa()
    {
        static CPressureUnit Pa("pascal", "Pa", true);
        return Pa;
    }
    /*!
     * \brief Hectopascal
     * \return
     */
    static const CPressureUnit &hPa()
    {
        static CPressureUnit hPa("hectopascal", "hPa", true, CMeasurementPrefix::h().getFactor(), CMeasurementPrefix::h());
        return hPa;
    }
    /*!
     * \brief Pounds per square inch
     * \return
     */
    static const CPressureUnit &psi()
    {
        static CPressureUnit psi("pounds per square inch", "psi", false, 6894.8, CMeasurementPrefix::One(), 2);
        return psi;
    }
    /*!
     * \brief Bar
     * \return
     */
    static const CPressureUnit &bar()
    {
        static CPressureUnit bar("bar", "bar", false, 1E5);
        return bar;
    }
    /*!
     * \brief Millibar, actually the same as hPa
     * \return
     */
    static const CPressureUnit &mbar()
    {
        static CPressureUnit bar("bar", "bar", false, 1E2);
        return bar;
    }
    /*!
     * \brief Inch of mercury at 0°C
     * \return
     */
    static const CPressureUnit &inHg()
    {
        static CPressureUnit inhg("Inch of mercury 0°C", "inHg", false, 3386.389);
        return inhg;
    }
    /*!
     * \brief Inch of mercury for flight level 29,92inHg = 1013,25mbar = 1013,25hPa
     * \return
     */
    static const CPressureUnit &inHgFL()
    {
        static CPressureUnit inhg("Inch of mercury ", "inHg", false, 3386.5307486631);
        return inhg;
    }
};

/*!
 * \brief Specialized class for temperatur units (kelvin, centidegree).
 * \author KWB
 */
class CTemperatureUnit : public CMeasurementUnit
{
private:
    double m_conversionOffsetToSi;
private:
    /*!
     * Constructor temperature unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param temperatureOffsetToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CTemperatureUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, double temperatureOffsetToSI = 0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "temperature", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon), m_conversionOffsetToSi(temperatureOffsetToSI) {}
protected:
    /*!
     * \brief Convert to SI conversion unit, specific for temperature
     * \param value
     * \return
     */
    virtual double CTemperatureUnit::conversionToSiConversionUnit(double value) const;
    /*!
     * \brief Convert from SI conversion unit, specific for temperature
     * \param value
     * \return
     */
    virtual double CTemperatureUnit::conversionFromSiConversionUnit(double value) const;
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CTemperatureUnit(const CTemperatureUnit &otherUnit) : CMeasurementUnit(otherUnit), m_conversionOffsetToSi(otherUnit.m_conversionOffsetToSi) {}
    /*!
     * Assigment operator
     */
    CTemperatureUnit &CTemperatureUnit::operator =(const CTemperatureUnit &otherUnit)
    {
        if (this == &otherUnit) return *this; // Same object? Yes, so skip assignment, and just return *this
        CMeasurementUnit::operator = (otherUnit);
        this->m_conversionOffsetToSi = otherUnit.m_conversionOffsetToSi;
        return (*this);
    }
    /*!
     * \brief Kelvin
     * \return
     */
    static const CTemperatureUnit &K()
    {
        static CTemperatureUnit K("Kelvin", "K", true, true);
        return K;
    }
    /*!
     * \brief Centigrade C
     * \return
     */
    static const CTemperatureUnit &C()
    {
        static CTemperatureUnit C("centigrade", "°C", false, false, 1.0, 273.15);
        return C;
    }
    /*!
     * \brief Fahrenheit F
     * \return
     */
    static const CTemperatureUnit &F()
    {
        static CTemperatureUnit F("Fahrenheit", "°F", false, false, 5.0 / 9.0, 459.67);
        return F;
    }
};

/*!
 * \brief Specialized class for speed units (m/s, ft/s, NM/h).
 * \author KWB
 */
class CSpeedUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Speed unit constructor
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CSpeedUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "speed", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CSpeedUnit(const CSpeedUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter/second m/s
     * \return
     */
    static const CSpeedUnit &m_s()
    {
        static CSpeedUnit ms("meters/second", "m/s", true, false);
        return ms;
    }
    /*!
     * \brief Knots
     * \return
     */
    static const CSpeedUnit &kts()
    {
        static CSpeedUnit kts("knot", "kts", false, false, 1852.0 / 3600.0, CMeasurementPrefix::One(), 1);
        return kts;
    }
    /*!
     * \brief Nautical miles per hour NM/h (same as kts)
     * \return
     */
    static const CSpeedUnit &NM_h()
    {
        static CSpeedUnit NMh("nautical miles/hour", "NM/h", false, false, 1852.0 / 3600.0, CMeasurementPrefix::One(), 1);
        return NMh;
    }
    /*!
     * \brief Feet/second ft/s
     * \return
     */
    static const CSpeedUnit &ft_s()
    {
        static CSpeedUnit fts("feet/seconds", "ft/s", false, false, 0.3048, CMeasurementPrefix::One(), 0);
        return fts;
    }
    /*!
     * \brief Feet/min ft/min
     * \return
     */
    static const CSpeedUnit &ft_min()
    {
        static CSpeedUnit ftmin("feet/minute", "ft/min", false, false, 0.3048 / 60.0, CMeasurementPrefix::One(), 0);
        return ftmin;
    }
    /*!
     * \brief Kilometer/hour km/h
     * \return
     */
    static const CSpeedUnit &km_h()
    {
        static CSpeedUnit kmh("kilometers/hour", "km/h", false, false, 1.0 / 3.6, CMeasurementPrefix::One(), 1);
        return kmh;
    }
};

/*!
 * \brief Specialized class for time units (ms, hour, min).
 * \author KWB
 */
class CTimeUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Time unit constructor
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CTimeUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "time", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CTimeUnit(const CTimeUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Second s
     * \return
     */
    static const CTimeUnit &s()
    {
        static CTimeUnit s("second", "s", true, true, 1, CMeasurementPrefix::None(), 1);
        return s;
    }
    /*!
     * \brief Millisecond ms
     * \return
     */
    static const CTimeUnit &ms()
    {
        static CTimeUnit ms("millisecond", "ms", true, false, 1E-03, CMeasurementPrefix::m(), 0);
        return ms;
    }
    /*!
     * \brief Hour
     * \return
     */
    static const CTimeUnit &h()
    {
        static CTimeUnit h("hour", "h", false, false, 3600, CMeasurementPrefix::None(), 1);
        return h;
    }
    /*!
     * \brief Minute
     * \return
     */
    static const CTimeUnit &min()
    {
        static CTimeUnit min("minute", "min", false, false, 60, CMeasurementPrefix::None(), 2);
        return min;
    }
    /*!
     * \brief Day
     * \return
     */
    static const CTimeUnit &d()
    {
        static CTimeUnit day("day", "d", false, false, 3600 * 24, CMeasurementPrefix::None(), 1);
        return day;
    }

};

} // namespace
} // namespace
#endif // BLACKMISC_PQUNITS_H
