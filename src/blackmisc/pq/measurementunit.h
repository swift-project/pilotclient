/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_MEASUREMENTUNIT_H
#define BLACKMISC_PQ_MEASUREMENTUNIT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QCoreApplication>
#include <QtDBus/QDBusArgument>
#include <QString>
#include <QtGlobal>
#include <QDebug>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QHash>
#include <cstddef>
#include <cmath>

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
                 */
                virtual double toDefault(double factor) const = 0;
                /*!
                 * Convert from default unit to this unit.
                 */
                virtual double fromDefault(double factor) const = 0;
            };

            /*!
             * Converter for default values, such as None, used with public constructor
             */
            struct NilConverter : public Converter
            {
                virtual double toDefault(double) const override { return 0.0; }
                virtual double fromDefault(double) const override { return 0.0; }
            };

            /*!
             * Concrete strategy pattern for converting unit that does nothing.
             */
            struct IdentityConverter : public Converter
            {
                virtual double toDefault(double factor) const override { return factor; }
                virtual double fromDefault(double factor) const override { return factor; }
            };

            /*!
             * Concrete strategy pattern for converting unit with linear conversion.
             * \tparam Policy a policy class with static method factor() returning double
             */
            template <class Policy>
            struct LinearConverter : public Converter
            {
                virtual double toDefault(double factor) const override { return factor * Policy::factor(); }
                virtual double fromDefault(double factor) const override { return factor / Policy::factor(); }
            };

            /*!
             * Concrete strategy pattern for converting unit with offset linear conversion.
             * \tparam Policy a policy class with static methods factor() and offset() returning double
             */
            template <class Policy>
            struct AffineConverter : public Converter
            {
                virtual double toDefault(double factor) const override { return (factor - Policy::offset()) * Policy::factor(); }
                virtual double fromDefault(double factor) const override { return factor / Policy::factor() + Policy::offset(); }
            };

            /*!
             * Concrete strategy pattern for converting unit with one subdivision conversion.
             * \tparam FactorPolicy a policy class with static method factor() returning double
             * \tparam SubdivPolicy a policy class with static methods fraction() and subfactor() returning double
             */
            template <class FactorPolicy, class SubdivPolicy>
            struct SubdivisionConverter : public Converter
            {
                virtual double toDefault(double factor) const override
                {
                    using BlackMisc::Math::CMathUtils;
                    double part2 = CMathUtils::fract(factor) * SubdivPolicy::fraction();
                    factor = CMathUtils::trunc(factor) + part2 / SubdivPolicy::subfactor();
                    return factor * FactorPolicy::factor();
                }
                virtual double fromDefault(double factor) const override
                {
                    using BlackMisc::Math::CMathUtils;
                    double part1 = CMathUtils::trunc(factor / FactorPolicy::factor());
                    double remaining = std::fmod(factor / FactorPolicy::factor(), 1.0);
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
            struct SubdivisionConverter2 : public Converter
            {
                virtual double toDefault(double factor) const override
                {
                    using BlackMisc::Math::CMathUtils;
                    double part2 = CMathUtils::fract(factor) * SubdivPolicy::fraction();
                    double part3 = CMathUtils::fract(part2) * SubdivPolicy::fraction();
                    factor = CMathUtils::trunc(factor) + (CMathUtils::trunc(part2) + part3 / SubdivPolicy::subfactor()) / SubdivPolicy::subfactor();
                    return factor * FactorPolicy::factor();
                }
                virtual double fromDefault(double factor) const override
                {
                    using BlackMisc::Math::CMathUtils;
                    double part1 = CMathUtils::trunc(factor / FactorPolicy::factor());
                    double remaining = std::fmod(factor / FactorPolicy::factor(), 1.0);
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
                static double fraction() { return 100.0f; } //!< fraction
                static double subfactor() { return float(Subfactor); } //!< subfactor
            };
            //! @}

        private:
            QString m_name; //!< name, e.g. "meter"
            QString m_symbol; //!< unit name, e.g. "m"
            double m_epsilon; //!< values with differences below epsilon are the equal
            int m_displayDigits; //!< standard rounding for string conversions
            QSharedDataPointer<Converter> m_converter; //!< strategy pattern allows an arbitrary conversion method as per object

        protected:
            /*!
             * Construct a unit with custom conversion
             */
            template <class Converter>
            CMeasurementUnit(const QString &name, const QString &symbol, const Converter &, int displayDigits, double epsilon)
                : m_name(name), m_symbol(symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_converter(new Converter)
            {}

            /*!
             * Construct a null unit
             */
            CMeasurementUnit(const QString &name, const QString &symbol, std::nullptr_t)
                : m_name(name), m_symbol(symbol), m_epsilon(0.0), m_displayDigits(0)
            {}

        public:
            //! Destructor
            virtual ~CMeasurementUnit() = default;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const
            {
                return this->getSymbol(i18n);
            }

            //! \copydoc CValueObject::marshallToDbus
            void marshallToDbus(QDBusArgument &argument) const
            {
                argument << this->m_symbol;
            }

            //! \copydoc CValueObject::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &)
            {
                // the concrete implementations will override this default
                // this is required so I can also stream None
                (*this) = CMeasurementUnit::None();
            }

            //! Default constructor for meta system
            //! \remarks Only public because the need, to use this with the metasystem
            CMeasurementUnit() : m_name("none"), m_symbol(""),  m_epsilon(0), m_displayDigits(0)
            {}

            //! Equal operator ==
            bool operator == (const CMeasurementUnit &other) const;

            //! Unequal operator !=
            bool operator != (const CMeasurementUnit &other) const;

            //! \copydoc CValueObject::qHash
            friend uint qHash(const CMeasurementUnit &unit)
            {
                return ::qHash(unit.getName());
            }

            //! Name such as "meter"
            QString getName(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_name.toStdString().c_str()) : this->m_name;
            }

            //! Unit name such as "m"
            QString getSymbol(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_symbol.toStdString().c_str()) : this->m_symbol;
            }

            //! Rounded value
            double roundValue(double value, int digits = -1) const;

            //! Rounded string utility method, virtual so units can have specialized formatting
            virtual QString makeRoundedQString(double value, int digits = -1, bool i18n = false) const;

            //! Value rounded with unit, e.g. "5.00m", "30kHz"
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const;

            //! Threshold for rounding
            double getEpsilon() const
            {
                return this->m_epsilon;
            }

            //! Display digits
            int getDisplayDigits() const
            {
                return this->m_displayDigits;
            }

            //! Convert from other unit to this unit.
            double convertFrom(double value, const CMeasurementUnit &unit) const;

            //! Is given value <= epsilon?
            bool isEpsilon(double value) const
            {
                if (this->isNull()) return false;
                if (value == 0) return true;
                return abs(value) <= this->m_epsilon;
            }

            //! Is unit null?
            bool isNull() const
            {
                return this->m_converter.data() == nullptr;
            }

            // --------------------------------------------------------------------
            // -- static
            // --------------------------------------------------------------------

            /*!
             * Unit from symbol
             * \param symbol must be a valid unit symbol (without i18n) or empty string (empty means default unit)
             * \param strict strict check means if unit is not found, program terminates
             */
            template <class U> static const U &unitFromSymbol(const QString &symbol, bool strict = true)
            {
                if (symbol.isEmpty()) return U::defaultUnit();
                const QList<U> &units = U::allUnits();
                for (int i = 0; i < units.size(); ++i)
                {
                    if (units.at(i).getSymbol() == symbol) return units.at(i);
                }
                if (strict) qFatal("Illegal unit name");
                return U::defaultUnit();
            }

            /*!
             * Valid unit symbol?
             * \param symbol to be tested
             * \param caseSensitivity check case sensitiv?
             */
            template <class U> static bool isValidUnitSymbol(const QString &symbol, Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive)
            {
                if (symbol.isEmpty()) return false;
                const QList<U> &units = U::allUnits();
                for (int i = 0; i < units.size(); ++i)
                {
                    if (caseSensitivity == Qt::CaseSensitive && units.at(i).getSymbol() == symbol) return true;
                    if (units.at(i).getSymbol().compare(symbol, Qt::CaseInsensitive) == 0) return 0;
                }
                return false;
            }

            //! Dimensionless unit
            static CMeasurementUnit &None()
            {
                static CMeasurementUnit none("none", "", NilConverter(), 0, 0);
                return none;
            }
        };

    }
}

#endif // guard
