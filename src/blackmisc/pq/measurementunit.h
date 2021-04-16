/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_MEASUREMENTUNIT_H
#define BLACKMISC_PQ_MEASUREMENTUNIT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/mixin/mixinstring.h"
#include "blackmisc/stringutils.h"

#include <QCoreApplication>
#include <QDBusArgument>
#include <QHash>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QString>
#include <Qt>
#include <QtDebug>
#include <QtGlobal>
#include <QList>
#include <cmath>
#include <cstddef>
#include <string>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*!
         * Base class for all units, such as meter, hertz.
         */
        class BLACKMISC_EXPORT CMeasurementUnit :
            public Mixin::String<CMeasurementUnit>,
            public Mixin::Icon<CMeasurementUnit>
        {
        protected:
            /*!
             * Pointer to function for converting between units.
             */
            using ConverterFunction = double (*)(double);

            /*!
             * Converter for default values, such as None, used with public constructor
             */
            struct NilConverter
            {
                static double toDefault(double) { return 0.0; } //!< convert from this unit to the default unit
                static double fromDefault(double) { return 0.0; } //!< convert to this unit from the default unit
            };

            /*!
             * Concrete strategy pattern for converting unit that does nothing.
             */
            struct IdentityConverter
            {
                static double toDefault(double value) { return value; } //!< convert from this unit to the default unit
                static double fromDefault(double value) { return value; } //!< convert to this unit from the default unit
            };

            /*!
             * Concrete strategy pattern for converting unit with linear conversion.
             * \tparam Policy a policy class with static method factor() returning double
             */
            template <class Policy>
            struct LinearConverter
            {
                static double toDefault(double value) { return value * Policy::factor(); } //!< convert from this unit to the default unit
                static double fromDefault(double value) { return value / Policy::factor(); } //!< convert to this unit from the default unit
            };

            /*!
             * Concrete strategy pattern for converting unit with offset linear conversion.
             * \tparam Policy a policy class with static methods factor() and offset() returning double
             */
            template <class Policy>
            struct AffineConverter
            {
                static double toDefault(double value) { return (value - Policy::offset()) * Policy::factor(); } //!< convert from this unit to the default unit
                static double fromDefault(double value) { return value / Policy::factor() + Policy::offset(); } //!< convert to this unit from the default unit
            };

            /*!
             * Concrete strategy pattern for converting unit with one subdivision conversion.
             * \tparam FactorPolicy a policy class with static method factor() returning double
             * \tparam SubdivPolicy a policy class with static methods fraction() and subfactor() returning double
             */
            template <class FactorPolicy, class SubdivPolicy>
            struct SubdivisionConverter
            {
                //! convert from this unit to the default unit
                static double toDefault(double value)
                {
                    using BlackMisc::Math::CMathUtils;
                    double part2 = CMathUtils::fract(value) * SubdivPolicy::fraction();
                    value = CMathUtils::trunc(value) + part2 / SubdivPolicy::subfactor();
                    return value * FactorPolicy::factor();
                }
                //! convert to this unit from the default unit
                static double fromDefault(double value)
                {
                    using BlackMisc::Math::CMathUtils;
                    double part1 = CMathUtils::trunc(value / FactorPolicy::factor());
                    double remaining = std::fmod(value / FactorPolicy::factor(), 1.0);
                    double part2 = remaining * SubdivPolicy::subfactor();
                    return part1 + part2 / SubdivPolicy::fraction();
                }
            };

            /*!
             * Concrete strategy pattern for converting unit with two subdivision conversions.
             * \tparam FactorPolicy a policy class with static method factor() returning double
             * \tparam SubdivPolicy a policy class with static methods fraction() and subfactor() returning double
             */
            template <class FactorPolicy, class SubdivPolicy>
            struct SubdivisionConverter2
            {
                //! convert from this unit to the default unit
                static double toDefault(double value)
                {
                    using BlackMisc::Math::CMathUtils;
                    double part2 = CMathUtils::fract(value) * SubdivPolicy::fraction();
                    double part3 = CMathUtils::fract(part2) * SubdivPolicy::fraction();
                    value = CMathUtils::trunc(value) + (CMathUtils::trunc(part2) + part3 / SubdivPolicy::subfactor()) / SubdivPolicy::subfactor();
                    return value * FactorPolicy::factor();
                }
                //! convert to this unit from the default unit
                static double fromDefault(double value)
                {
                    using BlackMisc::Math::CMathUtils;
                    double part1 = CMathUtils::trunc(value / FactorPolicy::factor());
                    double remaining = std::fmod(value / FactorPolicy::factor(), 1.0);
                    double part2 = CMathUtils::trunc(remaining * SubdivPolicy::subfactor());
                    remaining = std::fmod(remaining * SubdivPolicy::subfactor(), 1.0);
                    double part3 = remaining * SubdivPolicy::subfactor();
                    return part1 + part2 / SubdivPolicy::fraction() + part3 / (SubdivPolicy::fraction() * SubdivPolicy::fraction());
                }
            };

            //! Metapolicy that can be used to modify template parameters of converters
            //! @{
            struct One
            {
                static double factor() { return 1; } //!< factor
            };
            //! 2 (two)
            template <class Policy>
            struct Two
            {
                static double factor() { return Policy::factor() * 2.0; } //!< factor
            };
            //! 10^-3
            template <class Policy>
            struct Milli
            {
                static double factor() { return Policy::factor() / 1000.0; } //!< factor
            };
            template <class Policy>
            //! 10^-2
            struct Centi
            {
                static double factor() { return Policy::factor() / 100.0; } //!< factor
            };
            //! 10^2
            template <class Policy>
            struct Hecto
            {
                static double factor() { return Policy::factor() * 100.0; } //!< factor
            };
            //! 10^3
            template <class Policy>
            struct Kilo
            {
                static double factor() { return Policy::factor() * 1000.0; } //!< factor
            };
            //! 10^6
            template <class Policy>
            struct Mega
            {
                static double factor() { return Policy::factor() * 1e+6; } //!< factor
            };
            //! 10^9
            template <class Policy>
            struct Giga
            {
                static double factor() { return Policy::factor() * 1e+9; } //!< factor
            };
            //! in each hundred
            template <int Subfactor>
            struct InEachHundred
            {
                static double fraction() { return 100.0; } //!< fraction
                static double subfactor() { return float(Subfactor); } //!< subfactor
            };
            //! @}

        protected:
            //! Pimpl class
            struct Data
            {
                //! Construct a unit with custom conversion
                template <class Converter>
                constexpr Data(QLatin1String name, QLatin1String symbol, Converter, int displayDigits = 2, double epsilon = 1e-9)
                    : m_name(name), m_symbol(symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_toDefault(Converter::toDefault), m_fromDefault(Converter::fromDefault)
                {}

                //! Construct a null unit
                constexpr Data(QLatin1String name, QLatin1String symbol)
                    : m_name(name), m_symbol(symbol)
                {}

                QLatin1String m_name;     //!< name, e.g. "meter"
                QLatin1String m_symbol;   //!< unit name, e.g. "m"
                double m_epsilon = 0.0;   //!< values with differences below epsilon are the equal
                int m_displayDigits = 0;  //!< standard rounding for string conversions
                ConverterFunction m_toDefault = nullptr;   //!< convert from this unit to default unit
                ConverterFunction m_fromDefault = nullptr; //!< convert to this unit from default unit
            };

            //! Constant-initialize QLatin1String without using strlen.
            template <int N>
            static constexpr QLatin1String constQLatin1(const char (&str)[N])
            {
                return QLatin1String(str, N - 1); // -1 because N includes the null terminator
            }

            //! Constructor
            CMeasurementUnit(const Data &data) : m_data(&data) {}

            //! Constructor saves the address of its argument, so forbid rvalues
            CMeasurementUnit(const Data &&) = delete;

            //! Destructor
            ~CMeasurementUnit() = default;

            //! Copy constructor
            CMeasurementUnit(const CMeasurementUnit &) = default;

            //! Copy assignment operator
            CMeasurementUnit &operator =(const CMeasurementUnit &) = default;

        private:
            const Data *m_data = (static_cast<void>(throw std::logic_error("Uninitialized pimpl")), nullptr);

        public:
            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const
            {
                return this->getSymbol(i18n);
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::marshallToDbus
            void marshallToDbus(QDBusArgument &argument) const
            {
                argument << m_data->m_symbol;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &)
            {
                // the concrete implementations will override this default
                // this is required so I can also stream None
                (*this) = CMeasurementUnit::None();
            }

            //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
            void marshalToDataStream(QDataStream &stream) const
            {
                stream << QString(m_data->m_symbol);
            }

            //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
            void unmarshalFromDataStream(QDataStream &)
            {
                // the concrete implementations will override this default
                // this is required so that None can be marshalled
                *this = CMeasurementUnit::None();
            }

            //! Equal operator ==
            friend bool operator == (const CMeasurementUnit &a, const CMeasurementUnit &b)
            {
                if (&a == &b) return true;
                return a.m_data->m_name == b.m_data->m_name;
            }

            //! Unequal operator !=
            friend bool operator != (const CMeasurementUnit &a, const CMeasurementUnit &b)
            {
                return !(a == b);
            }

            //! \copydoc CValueObject::qHash
            friend uint qHash(const CMeasurementUnit &unit)
            {
                return ::qHash(unit.getName());
            }

            //! Name such as "meter"
            QString getName(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", m_data->m_name.latin1()) : m_data->m_name;
            }

            //! Unit name such as "m"
            QString getSymbol(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", m_data->m_symbol.latin1()) : m_data->m_symbol;
            }

            //! Does a string end with name or symbol? E.g. 3meter, 3m, 3deg
            bool endsStringWithNameOrSymbol(const QString &candidate, Qt::CaseSensitivity cs = Qt::CaseSensitive) const
            {
                const QString c = candidate.trimmed();
                return c.endsWith(this->getName(false), cs) || c.endsWith(this->getName(true)) ||
                       c.endsWith(this->getSymbol(false), cs) || c.endsWith(this->getSymbol(true));
            }

            //! Rounded value
            //! \note default digits is CMeasurementUnit::getDisplayDigits
            double roundValue(double value, int digits = -1) const;

            //! Rounded to the nearest CMeasurementUnit::getEpsilon
            //! \remark uses CMathUtils::roundEpsilon
            double roundToEpsilon(double value) const;

            //! Rounded string utility method, virtual so units can have specialized formatting
            //! \note default digits is CMeasurementUnit::getDisplayDigits
            virtual QString makeRoundedQString(double value, int digits = -1, bool withGroupSeparator = false, bool i18n = false) const;

            //! Value rounded with unit, e.g. "5.00m", "30kHz"
            //! \note default digits is CMeasurementUnit::getDisplayDigits
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool withGroupSeparator = false, bool i18n = false) const;

            //! Threshold for comparions
            double getEpsilon() const
            {
                return m_data->m_epsilon;
            }

            //! Display digits
            int getDisplayDigits() const
            {
                return m_data->m_displayDigits;
            }

            //! Convert from other unit to this unit.
            double convertFrom(double value, const CMeasurementUnit &unit) const;

            //! Is given value <= epsilon?
            bool isEpsilon(double value) const
            {
                if (this->isNull()) return false;
                if (qFuzzyIsNull(value)) return true;
                return std::abs(value) <= m_data->m_epsilon;
            }

            //! Is unit null?
            bool isNull() const
            {
                return m_data->m_toDefault == nullptr;
            }

            // --------------------------------------------------------------------
            // -- static
            // --------------------------------------------------------------------

            /*!
             * Unit from symbol
             * \param symbol must be a valid unit symbol (without i18n) or empty string (empty means default unit)
             * \param strict strict check means if unit is not found, program terminates
             */
            template <class U> static U unitFromSymbol(const QString &symbol, bool strict = true)
            {
                if (symbol.isEmpty()) { return U::defaultUnit(); }

                static const bool cs = hasCaseSensitiveSymbols<U>();
                for (const auto &unit : U::allUnits())
                {
                    if (strict && cs)
                    {
                        if (unit.getSymbol() == symbol) { return unit; }
                    }
                    else
                    {
                        if (stringCompare(unit.getSymbol(), symbol, Qt::CaseInsensitive)) { return unit; }
                    }
                }
                if (strict) qFatal("Illegal unit name");
                return U::defaultUnit();
            }

            /**
             * All symbols
             */
            template <class U> static const QStringList &allSymbols()
            {
                static const QStringList symbols = []
                {
                    QStringList s;
                    for (const auto &unit : U::allUnits())
                    {
                        s.push_back(unit.getSymbol());
                    }
                    return s;
                }();
                return symbols;
            }

            /**
             * All symbols case insensitive
             */
            template <class U> static const QStringList &allSymbolsLowerCase()
            {
                static const QStringList symbols = []
                {
                    QSet<QString> s;
                    for (const QString &symbol : allSymbols<U>())
                    {
                        s.insert(symbol.toLower());
                    }
                    return s.values();
                }();
                return symbols;
            }

            /**
             * Are symbols case sensitive?
             */
            template <class U> static bool hasCaseSensitiveSymbols()
            {
                static const bool cs = []
                {
                    return (allSymbolsLowerCase<U>().size() != allSymbols<U>().size());
                }();
                return cs;
            }

            /*!
             * Valid unit symbol?
             * \param symbol to be tested
             */
            template <class U> static bool isValidUnitSymbol(const QString &symbol)
            {
                static const bool cs = hasCaseSensitiveSymbols<U>();
                return cs ?
                       isValidUnitSymbol<U>(symbol, Qt::CaseSensitive) :
                       isValidUnitSymbol<U>(symbol, Qt::CaseInsensitive);
            }

            /*!
             * Valid unit symbol?
             * \param symbol to be tested
             * \param caseSensitivity check case sensitiv?
             */
            template <class U> static bool isValidUnitSymbol(const QString &symbol, Qt::CaseSensitivity caseSensitivity)
            {
                if (symbol.isEmpty()) return false;
                for (const auto &unit : U::allUnits())
                {
                    if (stringCompare(unit.getSymbol(), symbol, caseSensitivity)) { return true; }
                }
                return false;
            }

            /*!
             * Contains valid unit symbol?
             * \param candidate to be tested
             * \param caseSensitivity check case sensitiv?
             */
            template <class U> static bool containsValidUnitSymbol(const QString &candidate, Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive)
            {
                if (candidate.isEmpty()) return false;
                for (const auto &unit : U::allUnits())
                {
                    if (candidate.contains(unit.getSymbol(), caseSensitivity)) { return true; }
                }
                return false;
            }

            /*!
             * Ends with valid unit symbol?
             * \param candidate to be tested
             * \param caseSensitivity check case sensitiv?
             */
            template <class U> static bool endWithValidUnitSymbol(const QString &candidate, Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive)
            {
                if (candidate.isEmpty()) return false;
                for (const auto &unit : U::allUnits())
                {
                    if (candidate.endsWith(unit.getSymbol(), caseSensitivity)) { return true; }
                }
                return false;
            }

            //! Dimensionless unit
            static CMeasurementUnit None()
            {
                static constexpr CMeasurementUnit::Data none(constQLatin1("none"), constQLatin1(""), NilConverter(), 0, 0);
                return none;
            }
        };
    } // ns
} // ns

#endif // guard
