/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQUNITS_H
#define BLACKMISC_PQUNITS_H

#include "blackmisc/pqbase.h"
#include <QDBusArgument>
#include <QList>
#include <QtCore/qmath.h>
#include <QTranslator>


//
// Used with the template for quantities. This is the reason for
// having all units in one file, since template requires concrete instantiations
//
// I18N: http://qt-project.org/doc/qt-4.8/linguist-programmers.html#translating-text-that-is-outside-of-a-qobject-subclass
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
     * \brief Constructor length unit
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CLengthUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * \brief Constructor length unit
     * \param prefix
     * \param base
     * \param displayDigits
     * \param epsilon
     */
    CLengthUnit(const CMeasurementPrefix &prefix, const CLengthUnit &base, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(base, prefix, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CLengthUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CLengthUnit(const CLengthUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CLengthUnit &defaultUnit() { return m(); }

    /*!
     * \brief Meter m
     * \return
     */
    static const CLengthUnit &m()
    {
        static CLengthUnit m(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter"), "m", 1);
        return m;
    }

    /*!
     * \brief Nautical miles NM
     * \return
     */
    static const CLengthUnit &NM()
    {
        static CLengthUnit NM(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile"), "NM", 1000.0 * 1.85200, 3);
        return NM;
    }

    /*!
     * \brief Foot ft
     * \return
     */
    static const CLengthUnit &ft()
    {
        static CLengthUnit ft(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot"), "ft", 0.3048, 0);
        return ft;
    }

    /*!
     * \brief Kilometer km
     * \return
     */
    static const CLengthUnit &km()
    {
        static CLengthUnit km(CMeasurementPrefix::k(), m(), 3);
        return km;
    }

    /*!
     * \brief Centimeter cm
     * \return
     */
    static const CLengthUnit &cm()
    {
        static CLengthUnit cm(CMeasurementPrefix::c(), m(), 1);
        return cm;
    }

    /*!
     * \brief International mile
     * \return
     */
    static const CLengthUnit &mi()
    {
        static CLengthUnit mi(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile"), "mi", 1609.344, 3);
        return mi;
    }

    /*!
     * \brief Statute mile
     * \return
     */
    static const CLengthUnit &SM()
    {
        static CLengthUnit sm(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute miles"), "SM", 1609.3472, 3);
        return sm;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CLengthUnit> &allUnits()
    {
        static QList<CLengthUnit> u;
        if (u.isEmpty())
        {
            u.append(CLengthUnit::cm());
            u.append(CLengthUnit::ft());
            u.append(CLengthUnit::km());
            u.append(CLengthUnit::m());
            u.append(CLengthUnit::mi());
            u.append(CLengthUnit::SM());
            u.append(CLengthUnit::NM());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CLengthUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for angles (degrees, radian).
 */
class CAngleUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor angle units: Radian, degree
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CAngleUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * \brief Constructor angle units: Sexagesimal
     * \param name
     * \param symbol
     * \param converter
     * \param displayDigits
     * \param epsilon
     */
    CAngleUnit(const QString &name, const QString &symbol, Converter *converter, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, converter, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CAngleUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CAngleUnit(const CAngleUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CAngleUnit &defaultUnit() { return deg(); }

    /*!
     * \brief Override for sexagesimal degrees.
     * \param value
     * \param digits
     * \param i18n
     * \return
     */
    virtual QString makeRoundedQString(double value, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Radians
     * \return
     */
    static const CAngleUnit &rad()
    {
        static CAngleUnit rad(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian"), "rad", 180.0 / M_PI);
        return rad;
    }

    /*!
     * \brief Degrees
     * \return
     */
    static const CAngleUnit &deg()
    {
        static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree"), QT_TRANSLATE_NOOP("CMeasurementUnit", "deg"), 1);
        return deg;
    }

    /*!
     * \brief Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
     * \return
     */
    static const CAngleUnit &sexagesimalDeg()
    {
        static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degrees minutes seconds"), "DMS", new SubdivisionConverter2<60, 100, 60, 100>, 0);
        return deg;
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2'%L3\"");
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1 %L2'%L3\"");
    }

    /*!
     * \brief Sexagesimal degree (degrees, minutes, decimal minutes)
     * \return
     */
    static const CAngleUnit &sexagesimalDegMin()
    {
        static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degrees minutes"), "MinDec", new SubdivisionConverter<60, 100>, 2);
        return deg;
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2'");
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2'");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CAngleUnit> &allUnits()
    {
        static QList<CAngleUnit> u;
        if (u.isEmpty())
        {
            u.append(CAngleUnit::deg());
            u.append(CAngleUnit::rad());
            u.append(CAngleUnit::sexagesimalDeg());
            u.append(CAngleUnit::sexagesimalDegMin());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CAngleUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for frequency (hertz, mega hertz, kilo hertz).
 */
class CFrequencyUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor frequency unit
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CFrequencyUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * Constructor frequency unit
     * \param prefix
     * \param base
     * \param displayDigits
     * \param epsilon
     */
    CFrequencyUnit(const CMeasurementPrefix &prefix, const CFrequencyUnit &base, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(base, prefix, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CFrequencyUnit(const CFrequencyUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CFrequencyUnit &defaultUnit() { return Hz(); }

    /*!
     * \brief Hertz
     * \return
     */
    static const CFrequencyUnit &Hz()
    {
        static CFrequencyUnit Hz(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz"), "Hz", 1);
        return Hz;
    }

    /*!
     * \brief Kilohertz
     * \return
     */
    static const CFrequencyUnit &kHz()
    {
        static CFrequencyUnit kHz(CMeasurementPrefix::k(), Hz(), 1);
        return kHz;
    }

    /*!
     * \brief Megahertz
     * \return
     */
    static const CFrequencyUnit &MHz()
    {
        static CFrequencyUnit MHz(CMeasurementPrefix::M(), Hz(), 2);
        return MHz;
    }

    /*!
     * \brief Gigahertz
     * \return
     */
    static const CFrequencyUnit &GHz()
    {
        static CFrequencyUnit GHz(CMeasurementPrefix::G(), Hz(), 2);
        return GHz;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CFrequencyUnit> &allUnits()
    {
        static QList<CFrequencyUnit> u;
        if (u.isEmpty())
        {
            u.append(CFrequencyUnit::GHz());
            u.append(CFrequencyUnit::Hz());
            u.append(CFrequencyUnit::kHz());
            u.append(CFrequencyUnit::MHz());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for mass units (kg, lbs).
 */
class CMassUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor mass units
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CMassUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * \brief Constructor mass units
     * \param prefix
     * \param base
     * \param displayDigits
     * \param epsilon
     */
    CMassUnit(const CMeasurementPrefix &prefix, const CMassUnit &base, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(base, prefix, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CMassUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMassUnit(const CMassUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CMassUnit &defaultUnit() { return kg(); }

    /*!
     * \brief Kilogram, SI base unit
     * \return
     */
    static const CMassUnit &kg()
    {
        static CMassUnit kg(CMeasurementPrefix::k(), g(), 1);
        return kg;
    }

    /*!
     * \brief Gram, SI unit
     * \return
     */
    static const CMassUnit &g()
    {
        static CMassUnit g(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram"), "g", 0.001, 0);
        return g;
    }

    /*!
     * \brief Tonne, aka metric tonne (1000kg)
     * \return
     */
    static const CMassUnit &t()
    {
        static CMassUnit t(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne"), "t", 1000.0, 3);
        return t;
    }

    /*!
     * \brief Pound, aka mass pound
     * \return
     */
    static const CMassUnit &lb()
    {
        static CMassUnit lbs(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound"), "lb", 0.45359237, 1);
        return lbs;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CMassUnit> &allUnits()
    {
        static QList<CMassUnit> u;
        if (u.isEmpty())
        {
            u.append(CMassUnit::g());
            u.append(CMassUnit::kg());
            u.append(CMassUnit::lb());
            u.append(CMassUnit::t());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CMassUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for pressure (psi, hPa, bar).
 */
class CPressureUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Pressure unit constructor
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CPressureUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * \brief Pressure unit constructor
     * \param prefix
     * \param base
     * \param displayDigits
     * \param epsilon
     */
    CPressureUnit(const CMeasurementPrefix &prefix, const CPressureUnit &base, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(base, prefix, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CPressureUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CPressureUnit(const CPressureUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CPressureUnit &defaultUnit() { return hPa(); }

    /*!
     * \brief Pascal
     * \return
     */
    static const CPressureUnit &Pa()
    {
        static CPressureUnit Pa(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal"), "Pa", 0.01);
        return Pa;
    }

    /*!
     * \brief Hectopascal
     * \return
     */
    static const CPressureUnit &hPa()
    {
        static CPressureUnit hPa(CMeasurementPrefix::h(), Pa());
        return hPa;
    }

    /*!
     * \brief Pounds per square inch
     * \return
     */
    static const CPressureUnit &psi()
    {
        static CPressureUnit psi(QT_TRANSLATE_NOOP("CMeasurementUnit", "pounds per square inch"), "psi", 68.948, 2);
        return psi;
    }

    /*!
     * \brief Bar
     * \return
     */
    static const CPressureUnit &bar()
    {
        static CPressureUnit bar(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar"), "bar", 1000, 1);
        return bar;
    }

    /*!
     * \brief Millibar, actually the same as hPa
     * \return
     */
    static const CPressureUnit &mbar()
    {
        static CPressureUnit bar(CMeasurementPrefix::m(), bar(), 1);
        return bar;
    }

    /*!
     * \brief Inch of mercury at 0°C
     * \return
     */
    static const CPressureUnit &inHg()
    {
        static CPressureUnit inhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury"), "inHg", 33.86389);
        return inhg;
    }

    /*!
     * \brief Millimeter of mercury
     * \return
     */
    static const CPressureUnit &mmHg()
    {
        static CPressureUnit mmhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury"), "mmHg", 860.142806);
        return mmhg;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CPressureUnit> &allUnits()
    {
        static QList<CPressureUnit> u;
        if (u.isEmpty())
        {
            u.append(CPressureUnit::bar());
            u.append(CPressureUnit::hPa());
            u.append(CPressureUnit::inHg());
            u.append(CPressureUnit::mmHg());
            u.append(CPressureUnit::mbar());
            u.append(CPressureUnit::psi());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CPressureUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for temperatur units (kelvin, centidegree).
 */
class CTemperatureUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor temperature unit
     * \param name
     * \param symbol
     * \param factor
     * \param offset
     * \param displayDigits
     * \param epsilon
     */
    CTemperatureUnit(const QString &name, const QString &symbol, double factor, double offset, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, offset, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CTemperatureUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * \brief Copy constructor
     * \param other
     */
    CTemperatureUnit(const CTemperatureUnit &other) : CMeasurementUnit(other) {}

    /*!
     * \brief Default unit
     * \return
     */
    static const CTemperatureUnit &defaultUnit() { return C(); }

    /*!
     * \brief Kelvin
     * \return
     */
    static const CTemperatureUnit &K()
    {
        static CTemperatureUnit K(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin"), "K", 1, 273.15);
        return K;
    }

    /*!
     * \brief Centigrade C
     * \return
     */
    static const CTemperatureUnit &C()
    {
        static CTemperatureUnit C(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade"), QT_TRANSLATE_NOOP("CMeasurementUnit", "C"), 1, 0);
        return C;
    }

    /*!
     * \brief Fahrenheit F
     * \return
     */
    static const CTemperatureUnit &F()
    {
        static CTemperatureUnit F(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit"), QT_TRANSLATE_NOOP("CMeasurementUnit", "F"), 5.0 / 9.0, 32);
        return F;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CTemperatureUnit> &allUnits()
    {
        static QList<CTemperatureUnit> u;
        if (u.isEmpty())
        {
            u.append(CTemperatureUnit::C());
            u.append(CTemperatureUnit::F());
            u.append(CTemperatureUnit::K());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for speed units (m/s, ft/s, NM/h).
 */
class CSpeedUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Speed unit constructor
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CSpeedUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CSpeedUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * Constructor, allows to implement methods in base class
     * \param other
     */
    CSpeedUnit(const CSpeedUnit &other) : CMeasurementUnit(other) {}

    /*!
     * Default unit
     * \return
     */
    static const CSpeedUnit &defaultUnit() { return m_s(); }

    /*!
     * \brief Meter/second m/s
     * \return
     */
    static const CSpeedUnit &m_s()
    {
        static CSpeedUnit ms(QT_TRANSLATE_NOOP("CMeasurementUnit", "meters/second"), "m/s", 1);
        return ms;
    }

    /*!
     * \brief Knots
     * \return
     */
    static const CSpeedUnit &kts()
    {
        static CSpeedUnit kts(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot"), "kts", 1852.0 / 3600.0, 1);
        return kts;
    }

    /*!
     * \brief Nautical miles per hour NM/h (same as kts)
     * \return
     */
    static const CSpeedUnit &NM_h()
    {
        static CSpeedUnit NMh(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical miles/hour"), "NM/h", 1852.0 / 3600.0, 1);
        return NMh;
    }

    /*!
     * \brief Feet/second ft/s
     * \return
     */
    static const CSpeedUnit &ft_s()
    {
        static CSpeedUnit fts(QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/second"), "ft/s", 0.3048, 0);
        return fts;
    }

    /*!
     * \brief Feet/min ft/min
     * \return
     */
    static const CSpeedUnit &ft_min()
    {
        static CSpeedUnit ftmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/minute"), "ft/min", 0.3048 / 60.0, 0);
        return ftmin;
    }

    /*!
     * \brief Kilometer/hour km/h
     * \return
     */
    static const CSpeedUnit &km_h()
    {
        static CSpeedUnit kmh(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometers/hour"), "km/h", 1.0 / 3.6, 1);
        return kmh;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CSpeedUnit> &allUnits()
    {
        static QList<CSpeedUnit> u;
        if (u.isEmpty())
        {
            u.append(CSpeedUnit::ft_min());
            u.append(CSpeedUnit::ft_s());
            u.append(CSpeedUnit::km_h());
            u.append(CSpeedUnit::kts());
            u.append(CSpeedUnit::m_s());
            u.append(CSpeedUnit::NM_h());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for time units (ms, hour, min).
 */
class CTimeUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Time unit constructor
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CTimeUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

    /*!
     * \brief Time unit constructor
     * \param prefix
     * \param base
     * \param displayDigits
     * \param epsilon
     */
    CTimeUnit(const CMeasurementPrefix &prefix, const CTimeUnit &base, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(base, prefix, displayDigits, epsilon) {}

    /*!
     * \brief Time unit constructor
     * \param name
     * \param symbol
     * \param converter
     * \param displayDigits
     * \param epsilon
     */
    CTimeUnit(const QString &name, const QString &symbol, Converter *converter, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, converter, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CTimeUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * Constructor, allows to implement methods in base class
     * \param other
     */
    CTimeUnit(const CTimeUnit &other) : CMeasurementUnit(other) {}

    /*!
     * Default unit
     * \return
     */
    static const CTimeUnit &defaultUnit() { return s(); }

    /*!
     * \brief Override for subdivisional units.
     * \param value
     * \param digits
     * \param i18n
     * \return
     */
    virtual QString makeRoundedQString(double value, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Second s
     * \return
     */
    static const CTimeUnit &s()
    {
        static CTimeUnit s(QT_TRANSLATE_NOOP("CMeasurementUnit", "second"), "s", 1, 1);
        return s;
    }

    /*!
     * \brief Millisecond ms
     * \return
     */
    static const CTimeUnit &ms()
    {
        static CTimeUnit ms(CMeasurementPrefix::m(), s(), 0);
        return ms;
    }

    /*!
     * \brief Hour
     * \return
     */
    static const CTimeUnit &h()
    {
        static CTimeUnit h(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour"), "h", 3600, 1);
        return h;
    }

    /*!
     * \brief Minute
     * \return
     */
    static const CTimeUnit &min()
    {
        static CTimeUnit min(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute"), "min", 60, 2);
        return min;
    }

    /*!
     * \brief Day
     * \return
     */
    static const CTimeUnit &d()
    {
        static CTimeUnit day(QT_TRANSLATE_NOOP("CMeasurementUnit", "day"), "d", 3600 * 24, 1);
        return day;
    }

    /*!
     * \brief Hours, minutes, seconds
     */
    static const CTimeUnit &hms()
    {
        static CTimeUnit hms(QT_TRANSLATE_NOOP("CMeasurementUnit", "hours minutes seconds"), "hms", new SubdivisionConverter2<60, 100, 60, 100>(3600), 0);
        return hms;
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
    }

    /*!
     * \brief Hours, minutes
     */
    static const CTimeUnit &hrmin()
    {
        static CTimeUnit hrmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "hours minutes"), "hm", new SubdivisionConverter<60, 100>(3600), 1);
        return hrmin;
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
    }

    /*!
     * \brief Minutes, seconds
     */
    static const CTimeUnit &minsec()
    {
        static CTimeUnit minsec(QT_TRANSLATE_NOOP("CMeasurementUnit", "minutes seconds"), "minsec", new SubdivisionConverter<60, 100>(60), 0);
        return minsec;
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1m%L2s");
        (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1m%L2s");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CTimeUnit> &allUnits()
    {
        static QList<CTimeUnit> u;
        if (u.isEmpty())
        {
            u.append(CTimeUnit::d());
            u.append(CTimeUnit::h());
            u.append(CTimeUnit::min());
            u.append(CTimeUnit::ms());
            u.append(CTimeUnit::s());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CTimeUnit>(unitName);
    }
};

/*!
 * \brief Specialized class for acceleration units (m/s2, ft/s2).
 */
class CAccelerationUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Acceleration unit constructor
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CAccelerationUnit(const QString &name, const QString &symbol, double factor, int displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, symbol, factor, displayDigits, epsilon) {}

public:
    /*!
     * Default constructor, required for Qt Metasystem
     */
    CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

    /*!
     * Copy constructor
     * \param other
     */
    CAccelerationUnit(const CAccelerationUnit &other) : CMeasurementUnit(other) {}

    /*!
     * Default unit
     * \return
     */
    static const CAccelerationUnit &defaultUnit() { return m_s2(); }

    /*!
     * \brief Meter/second^2 (m/s^2)
     * \return
     */
    static const CAccelerationUnit &m_s2()
    {
        static CAccelerationUnit ms2(QT_TRANSLATE_NOOP("CMeasurementUnit", "meters/second^2"), QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2"), 1, 1);
        return ms2;
    }

    /*!
     * \brief Feet/second^2
     * \return
     */
    static const CAccelerationUnit &ft_s2()
    {
        static CAccelerationUnit fts2(QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/second^2"), QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2"), 3.28084, 0);
        return fts2;
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CAccelerationUnit> &allUnits()
    {
        static QList<CAccelerationUnit> u;
        if (u.isEmpty())
        {
            u.append(CAccelerationUnit::ft_s2());
            u.append(CAccelerationUnit::m_s2());
        }
        return u;
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString unitName;
        argument >> unitName;
        (*this) = CMeasurementUnit::unitFromSymbol<CAccelerationUnit>(unitName);
    }
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLengthUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAngleUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CFrequencyUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CMassUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressureUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTemperatureUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeedUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTimeUnit)
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAccelerationUnit)

#endif // guard
