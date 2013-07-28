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
     * \param unitName
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CLengthUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "distance", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon)
    {
        // void
    }

public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CLengthUnit() : CMeasurementUnit("meter", "m", "distance", true, true) {}

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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "meter");
    }


    /*!
     * \brief Nautical miles NM
     * \return
     */
    static const CLengthUnit &NM()
    {
        static CLengthUnit NM("nautical mile", "NM", false, false, 1000.0 * 1.85200, CMeasurementPrefix::One(), 3);
        return NM;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile");
    }

    /*!
     * \brief Foot ft
     * \return
     */
    static const CLengthUnit &ft()
    {
        static CLengthUnit ft("foot", "ft", false, false, 0.3048, CMeasurementPrefix::One(), 0);
        return ft;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "foot");
    }


    /*!
     * \brief Kilometer km
     * \return
     */
    static const CLengthUnit &km()
    {
        static CLengthUnit km("kilometer", "km", true, false, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 3);
        return km;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer");
    }

    /*!
     * \brief Centimeter cm
     * \return
     */
    static const CLengthUnit &cm()
    {
        static CLengthUnit cm("centimeter", "cm", true, false, CMeasurementPrefix::c().getFactor(), CMeasurementPrefix::c(), 1);
        return cm;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter");
    }

    /*!
     * \brief International mile
     * \return
     */
    static const CLengthUnit &mi()
    {
        static CLengthUnit mi("mile", "mi", false, false, 1609.344, CMeasurementPrefix::None(), 3);
        return mi;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "mile");
    }

    /*!
     * \brief Statute mile
     * \return
     */
    static const CLengthUnit &miStatute()
    {
        static CLengthUnit mi("mile(statute)", "mi(statute)", false, false, 1609.3472, CMeasurementPrefix::None(), 3);
        return mi;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "mile(statute)");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CLengthUnit> &units()
    {
        static QList<CLengthUnit> u;
        u.append(CLengthUnit::cm());
        u.append(CLengthUnit::ft());
        u.append(CLengthUnit::km());
        u.append(CLengthUnit::m());
        u.append(CLengthUnit::mi());
        u.append(CLengthUnit::miStatute());
        u.append(CLengthUnit::NM());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CLengthUnit &fromUnitName(const QString &unitName) {
        QList<CLengthUnit> units = CLengthUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CLengthUnit::m(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CLengthUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CLengthUnit)

/*!
 * \brief Specialized class for angles (degrees, radian).
 */
class CAngleUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor angle units: Radian, degree
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param conversionFactorToSI
     * \param multiplier
     * \param displayDigits
     * \param epsilon
     */
    CAngleUnit(const QString &name, const QString &unitName, bool isSiUnit, double conversionFactorToSI = 1.0,
               const CMeasurementPrefix &multiplier = CMeasurementPrefix::One(), qint32 displayDigits = 2,
               double epsilon = 1E-9, UnitConverter converterToSi = 0,  UnitConverter converterFromSi = 0) :
        CMeasurementUnit(name, unitName, "angle", isSiUnit, false, conversionFactorToSI,
                         multiplier, displayDigits, epsilon, converterToSi, converterFromSi)
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
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CAngleUnit() : CMeasurementUnit("radian", "rad", "angle", true, false) {}

    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CAngleUnit(const CAngleUnit &otherUnit) : CMeasurementUnit(otherUnit) { }

    /*!
     * \brief Special conversion to QString for sexagesimal degrees.
     * \param value
     * \param digits
     * \param i18n
     * \return
     */
    virtual QString toQStringRounded(double value, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Radians
     * \return
     */
    static const CAngleUnit &rad()
    {
        static CAngleUnit rad("radian", "rad", true);
        return rad;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "radian");
    }

    /*!
     * \brief Degrees
     * \return
     */
    static const CAngleUnit &deg()
    {
        static CAngleUnit deg("degree", "deg", false, M_PI / 180);
        return deg;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "degree");
        QT_TRANSLATE_NOOP("CMeasurementUnit", "deg");
    }

    /*!
     * \brief Sexagesimal degree (degree, minute, seconds)
     * \return
     */
    static const CAngleUnit &sexagesimalDeg()
    {
        static CAngleUnit deg("segadecimal degree", "segd", false, M_PI / 180,
                              CMeasurementPrefix::One(), 0, 1E-9, CAngleUnit::conversionSexagesimalToSi, CAngleUnit::conversionSexagesimalFromSi); return deg;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "segadecimal degree");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CAngleUnit> &units()
    {
        static QList<CAngleUnit> u;
        u.append(CAngleUnit::deg());
        u.append(CAngleUnit::rad());
        u.append(CAngleUnit::sexagesimalDeg());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CAngleUnit &fromUnitName(const QString &unitName) {
        QList<CAngleUnit> units = CAngleUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CAngleUnit::rad(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CAngleUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAngleUnit)

/*!
 * \brief Specialized class for frequency (hertz, mega hertz, kilo hertz).
 */
class CFrequencyUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief CFrequencyUnit
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CFrequencyUnit(const QString &name, const QString &unitName, bool isSiUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "frequency", isSiUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CFrequencyUnit() : CMeasurementUnit("hertz", "Hz", "frequency", true, false) {}

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

    /*!
     * \brief All units
     * \return
     */
    static const QList<CFrequencyUnit> &units()
    {
        static QList<CFrequencyUnit> u;
        u.append(CFrequencyUnit::GHz());
        u.append(CFrequencyUnit::Hz());
        u.append(CFrequencyUnit::kHz());
        u.append(CFrequencyUnit::MHz());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CFrequencyUnit &fromUnitName(const QString &unitName) {
        QList<CFrequencyUnit> units = CFrequencyUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CFrequencyUnit::Hz(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CFrequencyUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CFrequencyUnit)

/*!
 * \brief Specialized class for mass units (kg, lbs).
 */
class CMassUnit : public CMeasurementUnit
{
private:
    /*!
     * \brief Constructor mass units
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CMassUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "mass", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CMassUnit() : CMeasurementUnit("kilogram", "kg", "mass", true, true, 1.0, CMeasurementPrefix::k(), 1) {}

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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram");
    }

    /*!
     * \brief Gram, SI unit
     * \return
     */
    static const CMassUnit &g()
    {
        static CMassUnit g("gram", "g", true, false, 1.0 / 1000.0, CMeasurementPrefix::One(), 0);
        return g;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "gram");
    }

    /*!
     * \brief Tonne, aka metric tonne (1000kg)
     * \return
     */
    static const CMassUnit &t()
    {
        static CMassUnit t("tonne", "t", false, false, 1000.0, CMeasurementPrefix::One(), 3);
        return t;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne");
    }

    /*!
     * \brief Pound, aka mass pound
     * \return
     */
    static const CMassUnit &lb()
    {
        static CMassUnit lbs("pound", "lb", false, false, 0.45359237, CMeasurementPrefix::One(), 1);
        return lbs;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "pound");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CMassUnit> &units()
    {
        static QList<CMassUnit> u;
        u.append(CMassUnit::g());
        u.append(CMassUnit::kg());
        u.append(CMassUnit::lb());
        u.append(CMassUnit::t());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CMassUnit &fromUnitName(const QString &unitName) {
        QList<CMassUnit> units = CMassUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CMassUnit::kg(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CMassUnit::fromUnitName(unitName);
    }

};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CMassUnit)

/*!
 * \brief Specialized class for pressure (psi, hPa, bar).
 */
class CPressureUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Pressure unit
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CPressureUnit(const QString &name, const QString &unitName, bool isSiUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "pressure", isSiUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CPressureUnit() : CMeasurementUnit("pascal", "Pa", "pressure", true, false) {}

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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal");
    }

    /*!
     * \brief Hectopascal
     * \return
     */
    static const CPressureUnit &hPa()
    {
        static CPressureUnit hPa("hectopascal", "hPa", true, CMeasurementPrefix::h().getFactor(), CMeasurementPrefix::h());
        return hPa;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal");
    }

    /*!
     * \brief Pounds per square inch
     * \return
     */
    static const CPressureUnit &psi()
    {
        static CPressureUnit psi("pounds per square inch", "psi", false, 6894.8, CMeasurementPrefix::One(), 2);
        return psi;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "pounds per square inch");
    }

    /*!
     * \brief Bar
     * \return
     */
    static const CPressureUnit &bar()
    {
        static CPressureUnit bar("bar", "bar", false, 1E5);
        return bar;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "bar");
    }

    /*!
     * \brief Millibar, actually the same as hPa
     * \return
     */
    static const CPressureUnit &mbar()
    {
        static CPressureUnit bar("millibar", "bar", false, 1E2);
        return bar;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar");
    }

    /*!
     * \brief Inch of mercury at 0°C
     * \return
     */
    static const CPressureUnit &inHg()
    {
        static CPressureUnit inhg("inch of mercury 0C", "inHg", false, 3386.389);
        return inhg;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury 0C");
    }

    /*!
     * \brief Inch of mercury for flight level 29,92inHg = 1013,25mbar = 1013,25hPa
     * \return
     */
    static const CPressureUnit &inHgFL()
    {
        static CPressureUnit inhg("inch of mercury", "inHg", false, 3386.5307486631);
        return inhg;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury 0C");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CPressureUnit> &units()
    {
        static QList<CPressureUnit> u;
        u.append(CPressureUnit::bar());
        u.append(CPressureUnit::hPa());
        u.append(CPressureUnit::inHg());
        u.append(CPressureUnit::inHgFL());
        u.append(CPressureUnit::mbar());
        u.append(CPressureUnit::psi());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CPressureUnit &fromUnitName(const QString &unitName) {
        QList<CPressureUnit> units = CPressureUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CPressureUnit::Pa(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CPressureUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressureUnit)

/*!
 * \brief Specialized class for temperatur units (kelvin, centidegree).
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
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param temperatureOffsetToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CTemperatureUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, double temperatureOffsetToSI = 0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "temperature", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon), m_conversionOffsetToSi(temperatureOffsetToSI) {}
protected:
    /*!
     * \brief Convert to SI conversion unit, specific for temperature
     * \param value
     * \return
     */
    virtual double conversionToSiConversionUnit(double value) const;

    /*!
     * \brief Convert from SI conversion unit, specific for temperature
     * \param value
     * \return
     */
    virtual double conversionFromSiConversionUnit(double value) const;

public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CTemperatureUnit() : CMeasurementUnit("Kelvin", "K", "temperature", true, true) {}

    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CTemperatureUnit(const CTemperatureUnit &otherUnit) : CMeasurementUnit(otherUnit), m_conversionOffsetToSi(otherUnit.m_conversionOffsetToSi) {}

    /*!
     * Assigment operator
     */
    CTemperatureUnit &operator =(const CTemperatureUnit &otherUnit)
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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin");
    }

    /*!
     * \brief Centigrade C
     * \return
     */
    static const CTemperatureUnit &C()
    {
        static CTemperatureUnit C("centigrade", "C", false, false, 1.0, 273.15);
        return C;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade");
        QT_TRANSLATE_NOOP("CMeasurementUnit", "C");
    }

    /*!
     * \brief Fahrenheit F
     * \return
     */
    static const CTemperatureUnit &F()
    {
        static CTemperatureUnit F("Fahrenheit", "F", false, false, 5.0 / 9.0, 459.67);
        return F;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit");
        QT_TRANSLATE_NOOP("CMeasurementUnit", "F");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CTemperatureUnit> &units()
    {
        static QList<CTemperatureUnit> u;
        u.append(CTemperatureUnit::C());
        u.append(CTemperatureUnit::F());
        u.append(CTemperatureUnit::K());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CTemperatureUnit &fromUnitName(const QString &unitName) {
        QList<CTemperatureUnit> units = CTemperatureUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CTemperatureUnit::K(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CTemperatureUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTemperatureUnit)

/*!
 * \brief Specialized class for speed units (m/s, ft/s, NM/h).
 */
class CSpeedUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Speed unit constructor
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CSpeedUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "speed", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CSpeedUnit() : CMeasurementUnit("meters/second", "m/s", "speed", true, false) {}

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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "meters/second");
    }

    /*!
     * \brief Knots
     * \return
     */
    static const CSpeedUnit &kts()
    {
        static CSpeedUnit kts("knot", "kts", false, false, 1852.0 / 3600.0, CMeasurementPrefix::One(), 1);
        return kts;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "knot");
    }

    /*!
     * \brief Nautical miles per hour NM/h (same as kts)
     * \return
     */
    static const CSpeedUnit &NM_h()
    {
        static CSpeedUnit NMh("nautical miles/hour", "NM/h", false, false, 1852.0 / 3600.0, CMeasurementPrefix::One(), 1);
        return NMh;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical miles/hour");
    }

    /*!
     * \brief Feet/second ft/s
     * \return
     */
    static const CSpeedUnit &ft_s()
    {
        static CSpeedUnit fts("feet/second", "ft/s", false, false, 0.3048, CMeasurementPrefix::One(), 0);
        return fts;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/second");
    }

    /*!
     * \brief Feet/min ft/min
     * \return
     */
    static const CSpeedUnit &ft_min()
    {
        static CSpeedUnit ftmin("feet/minute", "ft/min", false, false, 0.3048 / 60.0, CMeasurementPrefix::One(), 0);
        return ftmin;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/minute");
    }

    /*!
     * \brief Kilometer/hour km/h
     * \return
     */
    static const CSpeedUnit &km_h()
    {
        static CSpeedUnit kmh("kilometers/hour", "km/h", false, false, 1.0 / 3.6, CMeasurementPrefix::One(), 1);
        return kmh;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometers/hour");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CSpeedUnit> &units()
    {
        static QList<CSpeedUnit> u;
        u.append(CSpeedUnit::ft_min());
        u.append(CSpeedUnit::ft_s());
        u.append(CSpeedUnit::km_h());
        u.append(CSpeedUnit::kts());
        u.append(CSpeedUnit::m_s());
        u.append(CSpeedUnit::NM_h());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CSpeedUnit &fromUnitName(const QString &unitName) {
        QList<CSpeedUnit> units = CSpeedUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CSpeedUnit::m_s(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CSpeedUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CSpeedUnit)

/*!
 * \brief Specialized class for time units (ms, hour, min).
 */
class CTimeUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Time unit constructor
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CTimeUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "time", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CTimeUnit() : CMeasurementUnit("second", "s", "time", true, true, 1, CMeasurementPrefix::None()) {}

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
        QT_TRANSLATE_NOOP("CMeasurementUnit", "second");
    }

    /*!
     * \brief Millisecond ms
     * \return
     */
    static const CTimeUnit &ms()
    {
        static CTimeUnit ms("millisecond", "ms", true, false, 1E-03, CMeasurementPrefix::m(), 0);
        return ms;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond");
    }

    /*!
     * \brief Hour
     * \return
     */
    static const CTimeUnit &h()
    {
        static CTimeUnit h("hour", "h", false, false, 3600, CMeasurementPrefix::None(), 1);
        return h;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "hour");
    }

    /*!
     * \brief Minute
     * \return
     */
    static const CTimeUnit &min()
    {
        static CTimeUnit min("minute", "min", false, false, 60, CMeasurementPrefix::None(), 2);
        return min;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "minute");
    }

    /*!
     * \brief Day
     * \return
     */
    static const CTimeUnit &d()
    {
        static CTimeUnit day("day", "d", false, false, 3600 * 24, CMeasurementPrefix::None(), 1);
        return day;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "day");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CTimeUnit> &units()
    {
        static QList<CTimeUnit> u;
        u.append(CTimeUnit::d());
        u.append(CTimeUnit::h());
        u.append(CTimeUnit::min());
        u.append(CTimeUnit::ms());
        u.append(CTimeUnit::s());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CTimeUnit &fromUnitName(const QString &unitName) {
        QList<CTimeUnit> units = CTimeUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CTimeUnit::s(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CTimeUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CTimeUnit)

/*!
 * \brief Specialized class for acceleration units (m/s2, ft/s2).
 */
class CAccelerationUnit : public CMeasurementUnit
{
private:
    /*!
     * Constructor
     * \brief Acceleration unit constructor
     * \param name
     * \param unitName
     * \param isSiUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CAccelerationUnit(const QString &name, const QString &unitName, bool isSiUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "acceleration", isSiUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Default constructor, we do not want this, but required for Qt Metasystem
     */
    CAccelerationUnit() : CMeasurementUnit("meter/second²", "m/s^2", "acceleration", true, false, 1, CMeasurementPrefix::None(), 1) {}

    /*!
     * Constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CAccelerationUnit(const CAccelerationUnit &otherUnit) : CMeasurementUnit(otherUnit) {}

    /*!
     * \brief Meter/second^2 (m/s^2)
     * \return
     */
    static const CAccelerationUnit &m_s2()
    {
        static CAccelerationUnit ms2("meters/second^2", "m/s^2", true, false, 1, CMeasurementPrefix::None(), 1);
        return ms2;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "meters/second^2");
        QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2");
    }

    /*!
     * \brief Feet/second^2
     * \return
     */
    static const CAccelerationUnit &ft_s2()
    {
        static CAccelerationUnit fts2("feet/seconds²", "ft/s^2", true, false, 3.28084, CMeasurementPrefix::m(), 0);
        return fts2;
        QT_TRANSLATE_NOOP("CMeasurementUnit", "feet/second^2");
        QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2");
    }

    /*!
     * \brief All units
     * \return
     */
    static const QList<CAccelerationUnit> &units()
    {
        static QList<CAccelerationUnit> u;
        u.append(CAccelerationUnit::ft_s2());
        u.append(CAccelerationUnit::m_s2());
        return u;
    }

    /*!
     * \brief Unit from name
     * \param unitName must be valid!
     * \return
     */
    static const CAccelerationUnit &fromUnitName(const QString &unitName) {
        QList<CAccelerationUnit> units = CAccelerationUnit::units();
        // read only, avoid deep copy
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getUnitName() == unitName) return (units.at(i));
        }
        qFatal("Illegal unit name");
        return CAccelerationUnit::m_s2(); // just suppress "not all control paths return a value"
    }

protected:
    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString unitName;
        argument >> unitName;
        (*this) = CAccelerationUnit::fromUnitName(unitName);
    }
};
Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CAccelerationUnit)


} // namespace
} // namespace
#endif // guard
