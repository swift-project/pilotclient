/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQBASE_H
#define BLACKMISC_PQBASE_H

#include "blackmisc/valueobject.h"
#include "blackmisc/mathematics.h"
#include <QCoreApplication>
#include <QtDBus/QDBusArgument>
#include <QString>
#include <QtGlobal>
#include <QDebug>
#include <QSharedData>
#include <QSharedDataPointer>
#include <QHash>
#include <cstddef>

namespace BlackMisc
{
    namespace PhysicalQuantities
    {
        /*!
         * \brief Base class for all units, such as meter, hertz.
         */
        class CMeasurementUnit : public CValueObject
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
            };

            /*!
             * \brief Converter for default values, such as None, used with public constructor
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
                    using BlackMisc::Math::CMath;
                    double part2 = CMath::fract(factor) * SubdivPolicy::fraction();
                    factor = CMath::trunc(factor) + part2 / SubdivPolicy::subfactor();
                    return factor * FactorPolicy::factor();
                }
                virtual double fromDefault(double factor) const override
                {
                    using BlackMisc::Math::CMath;
                    factor /= FactorPolicy::factor();
                    double part2 = CMath::fract(factor) * SubdivPolicy::subfactor();
                    return CMath::trunc(factor) + part2 / SubdivPolicy::fraction();
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
                    using BlackMisc::Math::CMath;
                    double part2 = CMath::fract(factor) * SubdivPolicy::fraction();
                    double part3 = CMath::fract(part2) * SubdivPolicy::fraction();
                    factor = CMath::trunc(factor) + (CMath::trunc(part2) + part3 / SubdivPolicy::subfactor()) / SubdivPolicy::subfactor();
                    return factor * FactorPolicy::factor();
                }
                virtual double fromDefault(double factor) const override
                {
                    using BlackMisc::Math::CMath;
                    factor /= FactorPolicy::factor();
                    double part2 = CMath::fract(factor) * SubdivPolicy::subfactor();
                    double part3 = CMath::fract(part2) * SubdivPolicy::subfactor();
                    return CMath::trunc(factor) + (CMath::trunc(part2) + part3 / SubdivPolicy::fraction()) / SubdivPolicy::fraction();
                }
            };

            //! Metapolicy that can be used to modify template parameters of converters
            //! @{
            struct One
            {
                static double factor() { return 1; } //!< factor \return
            };
            //! \brief 2 (two)
            template <class Policy>
            struct Two
            {
                static double factor() { return Policy::factor() * 2.0; } //!< factor \return
            };
            //! \brief 10^-3
            template <class Policy>
            struct Milli
            {
                static double factor() { return Policy::factor() / 1000.0; } //!< factor \return
            };
            template <class Policy>
            //! \brief 10^-2
            struct Centi
            {
                static double factor() { return Policy::factor() / 100.0; } //!< factor \return
            };
            //! \brief 10^2
            template <class Policy>
            struct Hecto
            {
                static double factor() { return Policy::factor() * 100.0; } //!< factor \return
            };
            //! \brief 10^3
            template <class Policy>
            struct Kilo
            {
                static double factor() { return Policy::factor() * 1000.0; } //!< factor \return
            };
            //! \brief 10^6
            template <class Policy>
            struct Mega
            {
                static double factor() { return Policy::factor() * 1e+6; } //!< factor \return
            };
            //! \brief 10^9
            template <class Policy>
            struct Giga
            {
                static double factor() { return Policy::factor() * 1e+9; } //!< factor \return
            };
            //! \brief in each hundred
            template <int Subfactor>
            struct InEachHundred
            {
                static double fraction() { return 100.0f; } //!< fraction \return
                static double subfactor() { return float(Subfactor); } //!< subfactor \return
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

            //! \copydoc CValueObject::stringForStreaming
            virtual QString stringForStreaming() const override
            {
                return this->getName(false);
            }

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override
            {
                return this->getSymbol(i18n);
            }

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override
            {
                argument << this->m_symbol;
            }

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &) override
            {
                // the concrete implementations will override this default
                // this is required so I can also stream None
                (*this) = CMeasurementUnit::None();
            }

        public:
            /*!
             * \brief Default constructor for meta system
             * \remarks Only public because the need, to use this with the metasystem
             */
            CMeasurementUnit() : m_name("none"), m_symbol(""),  m_epsilon(0), m_displayDigits(0)
            {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                // used with None!
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Equal operator ==
             */
            bool operator == (const CMeasurementUnit &other) const;

            /*!
             * \brief Unequal operator !=
             */
            bool operator != (const CMeasurementUnit &other) const;

            /*!
             * \brief Name such as "meter"
             */
            QString getName(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_name.toStdString().c_str()) : this->m_name;
            }

            /*!
             * \brief Unit name such as "m"
             */
            QString getSymbol(bool i18n = false) const
            {
                return i18n ? QCoreApplication::translate("CMeasurementUnit", this->m_symbol.toStdString().c_str()) : this->m_symbol;
            }

            /*!
             * \copydoc CValueObject::getValueHash
             */
            virtual uint getValueHash() const override
            {
                return qHash(this->getName());
            }

            /*!
             * \brief Rounded value
             */
            double roundValue(double value, int digits = -1) const;

            /*!
             * Rounded string utility method, virtual so units can have
             * specialized formatting
             */
            virtual QString makeRoundedQString(double value, int digits = -1, bool i18n = false) const;

            /*!
             * \brief Value rounded with unit, e.g. "5.00m", "30kHz"
             */
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const;

            /*!
             * \brief Threshold for rounding
             */
            double getEpsilon() const
            {
                return this->m_epsilon;
            }

            /*!
             * \brief getDisplayDigits
             */
            int getDisplayDigits() const
            {
                return this->m_displayDigits;
            }

            /*!
             * Convert from other unit to this unit.
             * \param value other value
             * \param unit  other unit
             */
            double convertFrom(double value, const CMeasurementUnit &unit) const;

            /*!
             * \brief Is given value <= epsilon?
             */
            bool isEpsilon(double value) const
            {
                if (this->isNull()) return false;
                if (value == 0) return true;
                return abs(value) <= this->m_epsilon;
            }

            /*!
             * \brief Is unit null?
             */
            bool isNull() const
            {
                return this->m_converter.data() == nullptr;
            }

            // --------------------------------------------------------------------
            // -- static
            // --------------------------------------------------------------------

            /*!
             * \brief Unit from symbol
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
             * \brief Valid unit symbol?
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

            /*!
             * \brief Dimensionless unit
             */
            static CMeasurementUnit &None()
            {
                static CMeasurementUnit none("none", "", NilConverter(), 0, 0);
                return none;
            }

            /*!
             * \brief Metadata, mainly needed for None
             */
            static void registerMetadata();
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CMeasurementUnit)

#endif // guard
