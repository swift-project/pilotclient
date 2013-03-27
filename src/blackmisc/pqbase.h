//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef PQBASE_H
#define PQBASE_H

#include <QString>
#include <QtGlobal>
#include <QDebug>
#include "blackmisc/debug.h"

namespace BlackMisc {

/*!
 * Typical prefixes (multipliers) such as kilo, mega, hecto.
 * See <a href="http://www.poynton.com/notes/units/index.html">here</a> for an overview.
 * Use the static values such CMeasurementMultiplier::k() as to specify values.
 * \author KWB
 */
class CMeasurementPrefix {
    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param d
     * \param multiplier
     * \return
     */
    friend QDebug operator<<(QDebug d, const CMeasurementPrefix &multiplier);
    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param multiplier
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CMeasurementPrefix &multiplier);

private:
    QString _name; //!< name, e.g. "kilo"
    QString _prefix; //!< prefix, e.g. "k" for kilo
    double _factor; //!< factor, e.g. 1000 for kilo 1/100 for centi
    /*!
     * Constructor by parameters
     * \brief CMeasurementMultiplier
     * \param name
     * \param prefixName
     * \param factor
     */
    CMeasurementPrefix(const QString &name, const QString &prefixName, double factor);
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
    bool operator >  (const CMeasurementPrefix &otherMultiplier) const;
    /*!
     * \brief Less operator <
     * \param otherMultiplier
     * \return
     */
    bool operator <  (const CMeasurementPrefix &otherMultiplier) const;
    /*!
     * \brief Cast as double
     */
    operator double() const { return this->_factor; }
    /*!
     * \brief Cast as QString
     */
    operator QString() const { return this->_name;}
    /*!
     * \brief Factor, e.g.1000 for "kilo"
     * \return
     */
    double getFactor() const { return this->_factor;}
    /*!
     * \brief Name, e.g. "kilo"
     * \return
     */
    QString getName() const { return this->_name; }
    /*!
     * \brief Prefix, e.g. "k" for "kilo"
     * \return
     */
    QString getPrefix() const { return this->_prefix; }

    // --- static units, always use these for initialization
    // --- Remark: Static initialization in C++ is random, this is why no static members
    // --- are used

    /*!
     * \brief Unit "None"
     * \return
     */
    static CMeasurementPrefix& None() { static CMeasurementPrefix none("", "", 0.0); return none;}
    /*!
     * \brief Unit "One"
     * \return
     */
    static CMeasurementPrefix& One() { static CMeasurementPrefix one("one", "", 1.0); return one;}
    /*!
     * \brief Unit "mega"
     * \return
     */
    static CMeasurementPrefix& M() { static CMeasurementPrefix mega("mega", "M", 1E6); return mega;}
    /*!
     * \brief Unit "kilo"
     * \return
     */
    static CMeasurementPrefix& k() { static CMeasurementPrefix kilo("kilo", "k", 1000.0); return kilo;}
    /*!
     * \brief Unit "giga"
     * \return
     */
    static CMeasurementPrefix& G() { static CMeasurementPrefix giga("giga", "G", 1E9); return giga;}
    /*!
     * \brief Unit "hecto"
     * \return
     */
    static CMeasurementPrefix& h() { static CMeasurementPrefix hecto("hecto", "h", 100.0); return hecto;}
    /*!
     * \brief Unit "centi"
     * \return
     */
    static CMeasurementPrefix& c() { static CMeasurementPrefix centi("centi", "c", 0.01); return centi;}
};

// ---------------------------------------------------------------------------------
// --- Unit
// ---------------------------------------------------------------------------------

/**
 * Base class for all units, such as meter, hertz.
 */
class CMeasurementUnit {
    /*!
     * \brief Stream << overload to be used in debugging messages
     * \param d
     * \param unit
     * \return
     */
    friend QDebug operator<<(QDebug d, const CMeasurementUnit &unit);

    /*!
     * Stream operator for log messages
     * \brief operator <<
     * \param log
     * \param unit
     * \return
     */
    friend CLogMessage operator<<(CLogMessage log, const CMeasurementUnit &unit);

private:
    QString _name; //!< name, e.g. "meter"
    QString _unitName; //!< unit name, e.g. "m"
    QString _type; //!< type,such as distance. Somehow redundant, but simplifies unit comparisons
    bool _isSIUnit; //!< is this a SI unit?
    bool _isSIBaseUnit; //!< SI base unit?
    double _conversionFactorToSIConversionUnit; //!< factor to convert to SI, set to 0 if not applicable (rare cases, e.g. temperature)
    double _epsilon; //!< values with differences below epsilon are the equal
    qint32 _displayDigits; //!< standard rounding dor string conversions
    CMeasurementPrefix _multiplier; //!< multiplier

protected:
    /*!
     * Constructor by parameter
     *\brief CMeasurementUnit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param multiplier
     * \param displayDigits
     * \param epsilon
     */
    CMeasurementUnit(const QString &name, const QString &unitName, const QString &type, bool isSiUnit, bool isSiBaseUnit, double conversionFactorToSI = 1, const CMeasurementPrefix &multiplier = CMeasurementPrefix::None(), qint32 displayDigits = 2, double epsilon = 1E-10);
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
    bool isSiUnit() const { return this->_isSIUnit;}
    /*!
     * \brief Representing an base SI unit? Examples: second, meter
     * \return
     */
    bool isSiBaseUnit() const { return this->_isSIUnit;}
    /*!
     * \brief Representing an SI base unit? Example: meter
     * \return
     */
    bool isUnprefixedSiUnit() const { return this->_isSIUnit && this->_multiplier.getFactor() == 1; }
    /*!
     * \brief Name such as "meter"
     * \return
     */
    QString getName() const { return this->_name; }
    /*!
     * \brief Unit name such as "m"
     * \return
     */
    QString getUnitName() const { return this->_unitName; }
    /*!
     * \brief Factor toconvert to SI unit (e.g.meter,hertz)
     * \return
     */
    QString getType() const { return this->_type; }
    /*!
     * \brief Type such as "distance", "frequency"
     * \return
     */
    double getConversionFactorToSIConversionUnit() const { return this->_conversionFactorToSIConversionUnit;}
    /*!
     * \brief Threshold for rounding
     * \return
     */
    double getEpsilon() const { return this->_epsilon;}
    /*!
     * \brief getDisplayDigits
     * \return
     */
    qint32 getDisplayDigits() const { return this->_displayDigits; }
    /*!
     * \brief Multiplier such as "kilo"
     * \return
     */
    CMeasurementPrefix getMultiplier() const { return this->_multiplier; }
    /*!
     * \brief Factor to convert to given unit
     * \param to
     * \return
     */
    double conversionFactor(const CMeasurementUnit &to) const;
    /*!
     * \brief Factor to convert between given units
     * \param from
     * \param to
     * \return
     */
    static double conversionFactor(const CMeasurementUnit &from, const CMeasurementUnit &to);
    /*!
     * \brief Unit is not specified
     * \return
     */
    static CMeasurementUnit& None() { static CMeasurementUnit none("none", "", "", false, false, 0.0, CMeasurementPrefix::None(), 0, 0); return none;}
};

} // namespace BlackMisc

#endif // PQBASE_H
