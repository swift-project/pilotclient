/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQBASE_H
#define BLACKMISC_PQBASE_H

#include "blackmisc/streamable.h"
#include "blackmisc/debug.h"
#include "blackmisc/mathematics.h"
#include <QCoreApplication>
#include <QDBusArgument>
#include <QString>
#include <QtGlobal>
#include <QDebug>
#include <QSharedData>
#include <QSharedDataPointer>

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Typical prefixes (multipliers) such as kilo, mega, hecto.
 * See <a href="http://www.poynton.com/notes/units/index.html">here</a> for an overview.
 * Use the static values such as CMeasurementPrefix::k() to specify values.
 */
class CMeasurementPrefix : public CStreamable
{
private:
    QString m_name; //!< name, e.g. "kilo"
    QString m_symbol; //!< prefix, e.g. "k" for kilo
    double m_factor; //!< factor, e.g. 1000 for kilo 1/100 for centi

    /*!
     * Constructor by parameters
     * \brief CMeasurementMultiplier
     * \param name
     * \param prefixName
     * \param factor
     */
    CMeasurementPrefix(const QString &name, const QString &symbol, double factor) :
        m_name(name), m_symbol(symbol), m_factor(factor) {}

protected:
    /*!
     * \brief Name as string
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool /* i18n */ = false) const
    {
        return this->m_name;
    }

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->m_name;
    }

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument)
    {
        QString name;
        argument >> name;
        (*this) = CMeasurementPrefix::fromPrefixName(name);
    }

public:
    /*!
     * \brief Equal operator ==
     * \param other
     * \return
     */
    bool operator == (const CMeasurementPrefix &other) const;

    /*!
     * \brief Unequal operator !=
     * \param other
     * \return
     */
    bool operator != (const CMeasurementPrefix &other) const;

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
    QString getName(bool i18n = false) const
    {
        return i18n ? QCoreApplication::translate("CMeasurementPrefix", this->m_name.toStdString().c_str()) : this->m_name;
    }

    /*!
     * \brief Prefix, e.g. "k" for "kilo"
     * \return
     */
    QString getSymbol(bool i18n = false) const
    {
        return i18n ? QCoreApplication::translate("CMeasurementPrefix", this->m_symbol.toStdString().c_str()) : this->m_symbol;
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
        static CMeasurementPrefix one(QT_TR_NOOP("one"), "", 1.0);
        return one;
    }

    /*!
     * \brief Unit "mega"
     * \return
     */
    static const CMeasurementPrefix &M()
    {
        static CMeasurementPrefix mega(QT_TR_NOOP("mega"), "M", 1E6);
        return mega;
    }

    /*!
     * \brief Unit "kilo"
     * \return
     */
    static const CMeasurementPrefix &k()
    {
        static CMeasurementPrefix kilo(QT_TR_NOOP("kilo"), "k", 1000.0);
        return kilo;
    }

    /*!
     * \brief Unit "giga"
     * \return
     */
    static const CMeasurementPrefix &G()
    {
        static CMeasurementPrefix giga(QT_TR_NOOP("giga"), "G", 1E9);
        return giga;
    }

    /*!
     * \brief Unit "hecto"
     * \return
     */
    static const CMeasurementPrefix &h()
    {
        static CMeasurementPrefix hecto(QT_TR_NOOP("hecto"), "h", 100.0);
        return hecto;
    }

    /*!
     * \brief Unit "centi"
     * \return
     */
    static const CMeasurementPrefix &c()
    {
        static CMeasurementPrefix centi(QT_TR_NOOP("centi"), "c", 0.01);
        return centi;
    }

    /*!
     * \brief Unit "milli"
     * \return
     */
    static const CMeasurementPrefix &m()
    {
        static CMeasurementPrefix milli(QT_TR_NOOP("milli"), "m", 1E-03);
        return milli;
    }

    /*!
     * \brief All prefixes
     * \return
     */
    static const QList<CMeasurementPrefix> &prefixes()
    {
        static QList<CMeasurementPrefix> prefixes;
        if (prefixes.isEmpty())
        {
            prefixes.append(CMeasurementPrefix::c());
            prefixes.append(CMeasurementPrefix::G());
            prefixes.append(CMeasurementPrefix::h());
            prefixes.append(CMeasurementPrefix::k());
            prefixes.append(CMeasurementPrefix::M());
            prefixes.append(CMeasurementPrefix::m());
            prefixes.append(CMeasurementPrefix::None());
            prefixes.append(CMeasurementPrefix::One());
        }
        return prefixes;
    }

    /*!
     * \brief Prefix from name
     * \param prefixName must be valid!
     * \return
     */
    static const CMeasurementPrefix &fromPrefixName(const QString &prefixName)
    {
        const QList<CMeasurementPrefix> &prefixes = CMeasurementPrefix::prefixes();
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
class CMeasurementUnit : public CStreamable
{
protected:
    /*!
     * Abstract strategy pattern that encapsulates a unit conversion strategy.
     */
    class Converter : public QSharedData
    {
    public:
        /*!
         * Virtual destructor.
         */
        virtual ~Converter() {}
        /*!
         * Convert from this unit to default unit.
         * \param factor
         * \return
         */
        virtual double toDefault(double factor) const = 0;
        /*!
         * Convert from default unit to this unit.
         * \param factor
         * \return
         */
        virtual double fromDefault(double factor) const = 0;
        /*!
         * Make a copy of this object with a different prefix.
         * \param prefix
         * \return
         */
        virtual Converter *clone(const CMeasurementPrefix &prefix) const = 0;
    };

    /*!
     * Concrete strategy pattern for converting unit with linear conversion.
     */
    class LinearConverter : public Converter
    {
        double m_factor;
    public:
        /*!
         * Constructor
         * \param factor
         */
        LinearConverter(double factor) : m_factor(factor) {}
        virtual double toDefault(double factor) const { return factor * m_factor; }
        virtual double fromDefault(double factor) const { return factor / m_factor; }
        virtual Converter *clone(const CMeasurementPrefix &prefix) const { auto ret = new LinearConverter(*this); ret->m_factor *= prefix.getFactor(); return ret; }
    };

    /*!
     * Concrete strategy pattern for converting unit with affine conversion.
     */
    class AffineConverter : public Converter
    {
        double m_factor;
        double m_offset;
    public:
        /*!
         * Constructor
         * \param factor
         * \param offset
         */
        AffineConverter(double factor, double offset) : m_factor(factor), m_offset(offset) {}
        virtual double toDefault(double factor) const { return (factor - m_offset) * m_factor; }
        virtual double fromDefault(double factor) const { return factor / m_factor + m_offset; }
        virtual Converter *clone(const CMeasurementPrefix &prefix) const { auto ret = new AffineConverter(*this); ret->m_factor *= prefix.getFactor(); return ret; }
    };

    /*!
     * Concrete strategy pattern for converting unit with subdivision conversion.
     */
    template <int Num, int Den>
    class SubdivisionConverter : public Converter
    {
        double m_factor;
    public:
        /*!
         * Constructor
         */
        SubdivisionConverter(double factor = 1) : m_factor(factor) {}
        virtual double toDefault(double factor) const   { using BlackMisc::Math::CMath;
                                                          double ret  = CMath::trunc(factor); factor = CMath::fract(factor) * Den;
                                                                 ret +=              factor / Num;
                                                          return ret * m_factor; }
        virtual double fromDefault(double factor) const { using BlackMisc::Math::CMath;
                                                          factor /= m_factor;
                                                          double ret  = CMath::trunc(factor); factor = CMath::fract(factor) * Num;
                                                          return ret +=              factor / Den; }
        virtual Converter *clone(const CMeasurementPrefix &) const { qFatal("Not implemented"); return 0; }
    };

    /*!
     * Concrete strategy pattern for converting unit with subdivision conversion.
     */
    template <int Num1, int Den1, int Num2, int Den2>
    class SubdivisionConverter2 : public Converter
    {
        double m_factor;
    public:
        /*!
         * Constructor
         */
        SubdivisionConverter2(double factor = 1) : m_factor(factor) {}
        virtual double toDefault(double factor) const   { using BlackMisc::Math::CMath;
                                                          double ret  = CMath::trunc(factor);        factor = CMath::fract(factor) * Den1;
                                                                 ret += CMath::trunc(factor) / Num1; factor = CMath::fract(factor) * Den2;
                                                                 ret +=              factor  / (Num1 * Num2);
                                                          return ret * m_factor; }
        virtual double fromDefault(double factor) const { using BlackMisc::Math::CMath;
                                                          factor /= m_factor;
                                                          double ret  = CMath::trunc(factor);        factor = CMath::fract(factor) * Num1;
                                                                 ret += CMath::trunc(factor) / Den1; factor = CMath::fract(factor) * Num2;
                                                          return ret +=              factor  / (Den1 * Den2); }
        virtual Converter *clone(const CMeasurementPrefix &) const { qFatal("Not implemented"); return 0; }
    };

private:
    QString m_name; //!< name, e.g. "meter"
    QString m_symbol; //!< unit name, e.g. "m"
    double m_epsilon; //!< values with differences below epsilon are the equal
    int m_displayDigits; //!< standard rounding for string conversions
    QSharedDataPointer<Converter> m_converter; //!< strategy pattern allows an arbitrary conversion method as per object

protected:
    /*!
     * Construct a unit with linear conversion
     * \param name
     * \param symbol
     * \param factor
     * \param displayDigits
     * \param epsilon
     */
    CMeasurementUnit(const QString &name, const QString &symbol, double factor, int displayDigits, double epsilon);

    /*!
     * Construct a unit with affine conversion
     * \param name
     * \param symbol
     * \param factor
     * \param offset
     * \param displayDigits
     * \param epsilon
     */
    CMeasurementUnit(const QString &name, const QString &symbol, double factor, double offset, int displayDigits, double epsilon);

    /*!
     * Construct a unit with custom conversion
     * \param name
     * \param symbol
     * \param converter
     * \param displayDigits
     * \param epsilon
     */
    CMeasurementUnit(const QString &name, const QString &symbol, Converter *converter, int displayDigits, double epsilon);

    /*!
     * Construct from base unit and prefix
     * \param base
     * \param prefix
     * \param displayDigits
     * \param epsilon
     */
    CMeasurementUnit(const QString &name, const QString &symbol, const CMeasurementUnit &base, const CMeasurementPrefix &prefix, int displayDigits = 2, double epsilon = 1E-10);

    /*!
     * \brief Copy constructor
     * \param other
     */
    CMeasurementUnit(const CMeasurementUnit &other);

    /*!
     * \brief String for streaming operators is full name
     * \return
     */
    virtual QString stringForStreaming() const
    {
        return this->getName(false);
    }

    /*!
     * \brief Name as stringification
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const
    {
        return this->getSymbol(i18n);
    }

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const
    {
        argument << this->m_symbol;
    }

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &)
    {
        // the concrete implementations will override this default
        // this is required so I can also stream None
        (*this) = CMeasurementUnit::None();
    }

    /*!
     * \brief Unit from symbol
     * \param symbol must be a valid unit symbol (without i18n) or empty string (empty means default unit)
     * \return
     */
    template <class U> static const U &unitFromSymbol(const QString &symbol)
    {
        if (symbol.isEmpty()) return U::defaultUnit();
        const QList<U> &units = U::allUnits();
        for (int i = 0; i < units.size(); ++i) {
            if (units.at(i).getSymbol() == symbol) return units.at(i);
        }
        qFatal("Illegal unit name");
        return U::defaultUnit(); // just suppress "not all control paths return a value"
    }

public:
    /*!
     * \brief Equal operator ==
     * \param other
     * \return
     */
    bool operator == (const CMeasurementUnit &other) const;

    /*!
     * \brief Unequal operator !=
     * \param other
     * \return
     */
    bool operator != (const CMeasurementUnit &other) const;

    /*!
     * \brief Name such as "meter"
     * \param i18n
     * \return
     */
    QString getName(bool i18n = false) const
    {
        return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_name.toStdString().c_str()) : this->m_name;
    }

    /*!
     * \brief Unit name such as "m"
     * \param i18n
     * \return
     */
    QString getSymbol(bool i18n = false) const
    {
        return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_symbol.toStdString().c_str()) : this->m_symbol;
    }

    /*!
     * \brief Rounded value
     * \param value
     * \param digits
     * \return
     */
    double roundValue(double value, int digits = -1) const;

    /*!
     * Rounded string utility method, virtual so units can have
     * specialized formatting
     * \param value
     * \param digits
     * \param i18n
     * \return
     */
    virtual QString makeRoundedQString(double value, int digits = -1, bool i18n = false) const;

    /*!
     * \brief Value rounded with unit, e.g. "5.00m", "30kHz"
     * \param value
     * \param digits
     * \param i18n
     * \return
     */
    virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const;

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
    int getDisplayDigits() const
    {
        return this->m_displayDigits;
    }

    /*!
     * Convert from other unit to this unit.
     * \param
     */
    double convertFrom(double value, const CMeasurementUnit &unit) const;

    /*!
     * \brief Is given value <= epsilon?
     * \param value
     * \return
     */
    bool isEpsilon(double value) const
    {
        if (value == 0) return true;
        return abs(value) <= this->m_epsilon;
    }

    // --------------------------------------------------------------------
    // -- static
    // --------------------------------------------------------------------

    /*!
     * \brief Dimensionless unit
     * \return
     */
    static CMeasurementUnit &None()
    {
        static CMeasurementUnit none("none", "", 0.0, 0, 0);
        return none;
    }
};

} // namespace
} // namespace

#endif // BLACKMISC_PQBASE_H
