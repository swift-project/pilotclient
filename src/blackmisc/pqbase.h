/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQBASE_H
#define BLACKMISC_PQBASE_H

#include "blackmisc/basestreamstringifier.h"
#include "blackmisc/debug.h"
#include <QDBusArgument>
#include <QString>
#include <QtGlobal>
#include <QDebug>


namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Typical prefixes (multipliers) such as kilo, mega, hecto.
 * See <a href="http://www.poynton.com/notes/units/index.html">here</a> for an overview.
 * Use the static values such CMeasurementMultiplier::k() as to specify values.
 * \author KWB
 */
class CMeasurementPrefix : public CBaseStreamStringifier
{
private:
    QString m_name; //!< name, e.g. "kilo"
    QString m_prefix; //!< prefix, e.g. "k" for kilo
    double m_factor; //!< factor, e.g. 1000 for kilo 1/100 for centi
    /*!
     * Constructor by parameters
     * \brief CMeasurementMultiplier
     * \param name
     * \param prefixName
     * \param factor
     */
    CMeasurementPrefix(const QString &name, const QString &prefixName, double factor);

protected:
    /*!
     * \brief Name as stringification
     * \return
     */
    virtual QString stringForConverter() const
    {
        return this->m_name;
    }

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const {
        argument << this->m_name;
    }

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument) {
        QString name;
        argument >> name;
        (*this) = CMeasurementPrefix::fromPrefixName(name);
    }

public:
    /*!
     * \brief Copy constructor
     * \param otherMultiplier
     */
    CMeasurementPrefix(const CMeasurementPrefix &otherMultiplier);

    /*!
     * \brief Assigmnet operator =
     * \param otherMultiplier
     * \return
     */
    CMeasurementPrefix &operator =(const CMeasurementPrefix &otherMultiplier);

    /*!
     * \brief Equal operator ==
     * \param otherMultiplier
     * \return
     */
    bool operator == (const CMeasurementPrefix &otherMultiplier) const;

    /*!
     * \brief Unequal operator !=
     * \param otherMultiplier
     * \return
     */
    bool operator != (const CMeasurementPrefix &otherMultiplier) const;

    /*!
     * \brief Greater operator >
     * \param otherMultiplier
     * \return
     */
    bool operator > (const CMeasurementPrefix &otherMultiplier) const;

    /*!
     * \brief Less operator <
     * \param otherMultiplier
     * \return
     */
    bool operator < (const CMeasurementPrefix &otherMultiplier) const;

    /*!
     * \brief Cast as double
     */

    /*!
     * \brief Factor, e.g.1000 for "kilo"
     * \return
     */
    double getFactor() const
    {
        return this->m_factor;
    }

    /*!
     * \brief Factor
     * \return
     */
    double toDouble() const
    {
        return this->getFactor();
    }

    /*!
     * \brief Name, e.g. "kilo"
     * \return
     */
    QString getName() const
    {
        return this->m_name;
    }

    /*!
     * \brief Prefix, e.g. "k" for "kilo"
     * \return
     */
    QString getPrefix() const
    {
        return this->m_prefix;
    }

    /*!
     * \brief Operator as double
     */
    operator double() const
    {
        return this->m_factor;
    }

    // --- static units, always use these for initialization
    // --- Remark: Static initialization in C++ is random, this is why no static members
    // --- are used

    /*!
     * \brief Unit "None"
     * \return
     */
    static const CMeasurementPrefix &None()
    {
        static CMeasurementPrefix none("", "", 0.0);
        return none;
    }

    /*!
     * \brief Unit "One"
     * \return
     */
    static const CMeasurementPrefix &One()
    {
        static CMeasurementPrefix one("one", "", 1.0);
        return one;
    }

    /*!
     * \brief Unit "mega"
     * \return
     */
    static const CMeasurementPrefix &M()
    {
        static CMeasurementPrefix mega("mega", "M", 1E6);
        return mega;
    }

    /*!
     * \brief Unit "kilo"
     * \return
     */
    static const CMeasurementPrefix &k()
    {
        static CMeasurementPrefix kilo("kilo", "k", 1000.0);
        return kilo;
    }

    /*!
     * \brief Unit "giga"
     * \return
     */
    static const CMeasurementPrefix &G()
    {
        static CMeasurementPrefix giga("giga", "G", 1E9);
        return giga;
    }

    /*!
     * \brief Unit "hecto"
     * \return
     */
    static const CMeasurementPrefix &h()
    {
        static CMeasurementPrefix hecto("hecto", "h", 100.0);
        return hecto;
    }

    /*!
     * \brief Unit "centi"
     * \return
     */
    static const CMeasurementPrefix &c()
    {
        static CMeasurementPrefix centi("centi", "c", 0.01);
        return centi;
    }

    /*!
     * \brief Unit "milli"
     * \return
     */
    static const CMeasurementPrefix &m()
    {
        static CMeasurementPrefix milli("milli", "m", 1E-03);
        return milli;
    }


    /*!
     * \brief All prefixes
     * \return
     */
    static const QList<CMeasurementPrefix> &prefixes()
    {
        static QList<CMeasurementPrefix> prefixes;
        prefixes.append(CMeasurementPrefix::c());
        prefixes.append(CMeasurementPrefix::G());
        prefixes.append(CMeasurementPrefix::h());
        prefixes.append(CMeasurementPrefix::k());
        prefixes.append(CMeasurementPrefix::M());
        prefixes.append(CMeasurementPrefix::m());
        prefixes.append(CMeasurementPrefix::None());
        prefixes.append(CMeasurementPrefix::One());
        return prefixes;
    }

    /*!
     * \brief Prefix from name
     * \param prefixName must be valid!
     * \return
     */
    static const CMeasurementPrefix &fromPrefixName(const QString &prefixName) {
        QList<CMeasurementPrefix> prefixes = CMeasurementPrefix::prefixes();
        // read only, avoid deep copy
        for (int i = 0; i < prefixes.size(); ++i) {
            if (prefixes.at(i).getName() == prefixName) return (prefixes.at(i));
        }
        qFatal("Illegal unit name");
        return CMeasurementPrefix::None(); // just suppress "not all control paths return a value"
    }

};

// ---------------------------------------------------------------------------------
// --- Unit
// ---------------------------------------------------------------------------------

/*!
 * \brief Base class for all units, such as meter, hertz.
 */
class CMeasurementUnit: public CBaseStreamStringifier
{
protected:
    /*!
     * \brief Points to an individual converter method
     * Conversion as perobject, as required for CAnglewith sexagesimal conversion
     */
    typedef double(*UnitConverter)(const CMeasurementUnit &, double);

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const {
        argument << this->m_unitName;
    }

private:
    QString m_name; //!< name, e.g. "meter"
    QString m_unitName; //!< unit name, e.g. "m"
    QString m_type; //!< type,such as distance. Somehow redundant, but simplifies unit comparisons
    bool m_isSiUnit; //!< is this a SI unit?
    bool m_isSiBaseUnit; //!< SI base unit?
    double m_conversionFactorToSIConversionUnit; //!< factor to convert to SI, set to 0 if not applicable (rare cases, e.g. temperature)
    double m_epsilon; //!< values with differences below epsilon are the equal
    int m_displayDigits; //!< standard rounding for string conversions
    CMeasurementPrefix m_multiplier; //!< multiplier (kilo, Mega)
    UnitConverter m_fromSiConverter; //! allows an arbitrary conversion method as per object (e.g. angle, sexagesimal)
    UnitConverter m_toSiConverter; //! allows an arbitrary conversion method as per object (e.g. angle, sexagesimal)

protected:
    /*!
     * Constructor by parameter
     * \param name
     * \param unitName
     * \param type
     * \param isSiUnit
     * \param isSiBaseUnit
     * \param conversionFactorToSI
     * \param multiplier
     * \param displayDigits
     * \param epsilon
     * \param toSiConverter
     * \param fromSiConverter
     */
    CMeasurementUnit(const QString &name, const QString &unitName, const QString &type, bool isSiUnit, bool isSiBaseUnit, double conversionFactorToSI = 1,
                     const CMeasurementPrefix &multiplier = CMeasurementPrefix::None(), qint32 displayDigits = 2,
                     double epsilon = 1E-10, UnitConverter toSiConverter = 0, UnitConverter fromSiConverter = 0);

    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CMeasurementUnit(const CMeasurementUnit &otherUnit);

    /*!
     * \brief Assignment operator =
     * \param otherUnit
     * \return
     */
    CMeasurementUnit &operator =(const CMeasurementUnit &otherUnit);

protected:
    /*!
     * \brief String for streaming operators is full name
     * \return
     */
    virtual QString stringForStreaming() const
    {
        return this->m_name;
    }

    /*!
     * \brief String for converter is unit
     * \return
     */
    virtual QString stringForConverter() const
    {
        return this->m_unitName;
    }

    /*!
     * \brief Conversion factor to SI conversion unit
     * \return
     */
    double getConversionFactorToSI() const
    {
        return this->m_conversionFactorToSIConversionUnit;
    }

    /*!
     * Given value to conversion SI conversion unit (e.g. meter, hertz).
     * Standard implementaion is simply factor based.
     * \param value
     * \return
     */
    virtual double conversionToSiConversionUnit(double value) const
    {
        return value * this->m_conversionFactorToSIConversionUnit;
    }

    /*!
     * \brief Value from SI conversion unit to this unit.
     * Standard implementation is simply factor based.
     * \param value
     * \return
     */
    virtual double conversionFromSiConversionUnit(double value) const
    {
        return value / this->m_conversionFactorToSIConversionUnit;
    }

public:
    /*!
     * \brief Equal operator ==
     * \param otherUnit
     * \return
     */
    bool operator == (const CMeasurementUnit &otherUnit) const;

    /*!
     * \brief Unequal operator !=
     * \param otherUnit
     * \return
     */
    bool operator != (const CMeasurementUnit &otherUnit) const;

    /*!
     * \brief Representing an SI unit? Examples: kilometer, meter, hertz
     * \return
     */
    bool isSiUnit() const
    {
        return this->m_isSiUnit;
    }

    /*!
     * \brief Representing an base SI unit? Examples: second, meter
     * \return
     */
    bool isSiBaseUnit() const
    {
        return this->m_isSiUnit;
    }

    /*!
     * \brief Representing an SI base unit? Example: meter
     * \return
     */
    bool isUnprefixedSiUnit() const
    {
        return this->m_isSiUnit && this->m_multiplier.getFactor() == 1;
    }

    /*!
     * \brief Name such as "meter"
     * \return
     */
    QString getName() const
    {
        return this->m_name;
    }

    /*!
     * \brief Unit name such as "m"
     * \return
     */
    QString getUnitName() const
    {
        return this->m_unitName;
    }

    /*!
     * \brief Type such as "distance", "frequency"
     * \return
     */
    QString getType() const
    {
        return this->m_type;
    }

    /*!
     * Given value to conversion SI conversion unit (e.g. meter, hertz).
     * Standard implementation is simply factor based.
     * \param value
     * \return
     */
    double convertToSiConversionUnit(double value) const
    {
        return (this->m_toSiConverter) ? this->m_toSiConverter((*this), value) : this->conversionToSiConversionUnit(value);
    }

    /*!
     * Value from SI conversion unit to this unit.
     * Standard implementation is simply factor based.
     * \param value
     * \return
     */
    double convertFromSiConversionUnit(double value) const
    {
        return (this->m_fromSiConverter) ? this->m_fromSiConverter((*this), value) : this->conversionFromSiConversionUnit(value);
    }

    /*!
     * Rounded string utility method, virtual so units can have
     * specialized formatting
     * \param value
     * \param digits
     * \return
     */
    virtual QString toQStringRounded(double value, int digits = -1) const;

    /*!
     * \brief Rounded value
     * \param value
     * \param digits
     * \return
     */
    double valueRounded(double value, int digits = -1) const;

    /*!
     * \brief Value rounded with unit, e.g. "5.00m", "30kHz"
     * \param value
     * \param digits
     * \return
     */
    virtual QString valueRoundedWithUnit(double value, int digits = -1) const;

    /*!
     * \brief Threshold for rounding
     * \return
     */
    double getEpsilon() const
    {
        return this->m_epsilon;
    }

    /*!
     * \brief getDisplayDigits
     * \return
     */
    qint32 getDisplayDigits() const
    {
        return this->m_displayDigits;
    }

    /*!
     * \brief Multiplier such as "kilo"
     * \return
     */
    CMeasurementPrefix getMultiplier() const
    {
        return this->m_multiplier;
    }

    /*!
     * \brief Factor to convert to given unit
     * \param value
     * \param to
     * \return
     */
    double conversionToUnit(double value, const CMeasurementUnit &to) const;

    /*!
     * Epsilon rounding. In some conversion rouding is required to avoid
     * periodical numbers.
     * \param value
     * \return
     */
    double epsilonUpRounding(double value) const;

    /*!
     * \brief Is given value <= epsilon?
     * \param checkValue
     * \return
     */
    bool isEpsilon(double checkValue) const
    {
        if (checkValue == 0) return true;
        return abs(checkValue) <= this->m_epsilon;
    }

    // --------------------------------------------------------------------
    // -- static
    // --------------------------------------------------------------------

    /*!
     * \brief Unit is not specified
     * \return
     */
    static CMeasurementUnit &None()
    {
        static CMeasurementUnit none("none", "", "", false, false, 0.0, CMeasurementPrefix::None(), 0, 0);
        return none;
    }

};

} // namespace
} // namespace

#endif // BLACKMISC_PQBASE_H
