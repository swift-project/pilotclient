/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQUNITS_H
#define BLACKMISC_PQUNITS_H

#pragma push_macro("min")
#undef min

#include "blackmisc/pqbase.h"
#include <QtDBus/QDBusArgument>
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
         * Specialized class for distance units (meter, foot, nautical miles).
         */
        class CLengthUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CLengthUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            //! null constructor
            CLengthUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            struct NauticalMilesToMeters    { static double factor() { return 1852.0;    } };
            struct FeetToMeters             { static double factor() { return    0.3048; } };
            struct MilesToMeters            { static double factor() { return 1609.344;  } };
            struct StatuteMilesToMeters     { static double factor() { return 1609.3472; } };
            typedef One MetersToMeters;

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CLengthUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CLengthUnit &defaultUnit() { return m(); }

            /*!
             * Null unit
             */
            static const CLengthUnit &nullUnit()
            {
                static CLengthUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Meter m
             */
            static const CLengthUnit &m()
            {
                static CLengthUnit m(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter"), "m", IdentityConverter());
                return m;
            }

            /*!
             * Nautical miles NM
             */
            static const CLengthUnit &NM()
            {
                static CLengthUnit NM(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile"), "NM", LinearConverter<NauticalMilesToMeters>(), 3);
                return NM;
            }

            /*!
             * Foot ft
             */
            static const CLengthUnit &ft()
            {
                static CLengthUnit ft(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot"), "ft", LinearConverter<FeetToMeters>(), 1);
                return ft;
            }

            /*!
             * Kilometer km
             */
            static const CLengthUnit &km()
            {
                static CLengthUnit km(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer"), "km", LinearConverter<Kilo<MetersToMeters> >(), 3);
                return km;
            }

            /*!
             * Centimeter cm
             */
            static const CLengthUnit &cm()
            {
                static CLengthUnit cm(QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter"), "cm", LinearConverter<Centi<MetersToMeters> >(), 1);
                return cm;
            }

            /*!
             * International mile
             */
            static const CLengthUnit &mi()
            {
                static CLengthUnit mi(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile"), "mi", LinearConverter<MilesToMeters>(), 3);
                return mi;
            }

            /*!
             * Statute mile
             */
            static const CLengthUnit &SM()
            {
                static CLengthUnit sm(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute mile"), "SM", LinearConverter<StatuteMilesToMeters>(), 3);
                return sm;
            }

            /*!
             * All units
             */
            static const QList<CLengthUnit> &allUnits()
            {
                static QList<CLengthUnit> u;
                if (u.isEmpty())
                {
                    u.append(CLengthUnit::nullUnit());
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
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CLengthUnit>(unitName);
            }
        };

        /*!
         * Specialized class for angles (degrees, radian).
         */
        class CAngleUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CAngleUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CAngleUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            struct RadiansToDegrees { static double factor() { return 180.0 / M_PI; } };
            typedef One DegreesToDegrees;

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CAngleUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CAngleUnit &defaultUnit() { return deg(); }

            /*!
             * Null unit
             */
            static const CAngleUnit &nullUnit()
            {
                static CAngleUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
             */
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            /*!
             * Radians
             */
            static const CAngleUnit &rad()
            {
                static CAngleUnit rad(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian"), "rad", LinearConverter<RadiansToDegrees>());
                return rad;
            }

            /*!
             * Degrees
             */
            static const CAngleUnit &deg()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree"), QT_TRANSLATE_NOOP("CMeasurementUnit", "deg"), IdentityConverter());
                return deg;
            }

            /*!
             * Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
             */
            static const CAngleUnit &sexagesimalDeg()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute, second"), "DMS", SubdivisionConverter2<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2 %L3");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2 %L3");
            }

            /*!
             * Sexagesimal degree (degrees, minutes, decimal minutes)
             */
            static const CAngleUnit &sexagesimalDegMin()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute"), "MinDec", SubdivisionConverter<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2");
            }

            /*!
             * All units
             */
            static const QList<CAngleUnit> &allUnits()
            {
                static QList<CAngleUnit> u;
                if (u.isEmpty())
                {
                    u.append(CAngleUnit::nullUnit());
                    u.append(CAngleUnit::deg());
                    u.append(CAngleUnit::rad());
                    u.append(CAngleUnit::sexagesimalDeg());
                    u.append(CAngleUnit::sexagesimalDegMin());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CAngleUnit>(unitName);
            }
        };

        /*!
         * Specialized class for frequency (hertz, mega hertz, kilo hertz).
         */
        class CFrequencyUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CFrequencyUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CFrequencyUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            typedef One HertzToHertz;

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CFrequencyUnit &defaultUnit() { return Hz(); }

            /*!
             * Null unit
             */
            static const CFrequencyUnit &nullUnit()
            {
                static CFrequencyUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Hertz
             */
            static const CFrequencyUnit &Hz()
            {
                static CFrequencyUnit Hz(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz"), "Hz", IdentityConverter());
                return Hz;
            }

            /*!
             * Kilohertz
             */
            static const CFrequencyUnit &kHz()
            {
                static CFrequencyUnit kHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilohertz"), "kHz", LinearConverter<Kilo<HertzToHertz> >(), 1);
                return kHz;
            }

            /*!
             * Megahertz
             */
            static const CFrequencyUnit &MHz()
            {
                static CFrequencyUnit MHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "megahertz"), "MHz", LinearConverter<Mega<HertzToHertz> >(), 2);
                return MHz;
            }

            /*!
             * Gigahertz
             */
            static const CFrequencyUnit &GHz()
            {
                static CFrequencyUnit GHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "gigahertz"), "GHz", LinearConverter<Giga<HertzToHertz> >(), 2);
                return GHz;
            }

            /*!
             * All units
             */
            static const QList<CFrequencyUnit> &allUnits()
            {
                static QList<CFrequencyUnit> u;
                if (u.isEmpty())
                {
                    u.append(CFrequencyUnit::nullUnit());
                    u.append(CFrequencyUnit::GHz());
                    u.append(CFrequencyUnit::Hz());
                    u.append(CFrequencyUnit::kHz());
                    u.append(CFrequencyUnit::MHz());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unitName);
            }
        };

        /*!
         * Specialized class for mass units (kg, lbs).
         */
        class CMassUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CMassUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CMassUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            typedef Milli<One> GramsToKilograms;
            struct PoundsToKilograms { static double factor() { return 0.45359237; } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CMassUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CMassUnit &defaultUnit() { return kg(); }

            /*!
             * Null unit
             */
            static const CMassUnit &nullUnit()
            {
                static CMassUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Kilogram, SI base unit
             */
            static const CMassUnit &kg()
            {
                static CMassUnit kg(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram"), "kg", IdentityConverter(), 1);
                return kg;
            }

            /*!
             * Gram, SI unit
             */
            static const CMassUnit &g()
            {
                static CMassUnit g(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram"), "g", LinearConverter<GramsToKilograms>(), 0);
                return g;
            }

            /*!
             * Tonne, aka metric ton (1000kg)
             */
            static const CMassUnit &tonne()
            {
                static CMassUnit t(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne"), "t", LinearConverter<Mega<GramsToKilograms> >(), 3);
                return t;
            }

            /*!
             * Short ton (2000lb) used in the United States
             */
            static const CMassUnit &shortTon()
            {
                static CMassUnit ton(QT_TRANSLATE_NOOP("CMeasurementUnit", "short ton"), "ton", LinearConverter<Two<Kilo<PoundsToKilograms> > >(), 3);
                return ton;
            }

            /*!
             * Pound, aka mass pound
             */
            static const CMassUnit &lb()
            {
                static CMassUnit lbs(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound"), "lb", LinearConverter<PoundsToKilograms>(), 1);
                return lbs;
            }

            /*!
             * All units
             */
            static const QList<CMassUnit> &allUnits()
            {
                static QList<CMassUnit> u;
                if (u.isEmpty())
                {
                    u.append(CMassUnit::nullUnit());
                    u.append(CMassUnit::g());
                    u.append(CMassUnit::kg());
                    u.append(CMassUnit::lb());
                    u.append(CMassUnit::tonne());
                    u.append(CMassUnit::shortTon());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CMassUnit>(unitName);
            }
        };

        /*!
         * Specialized class for pressure (psi, hPa, bar).
         */
        class CPressureUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CPressureUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CPressureUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            typedef Centi<One> PascalsToHectopascals;
            struct PsiToHectopascals            { static double factor() { return  68.948;      } };
            struct InchesToHectopascals         { static double factor() { return  33.86389;    } };
            struct MillimetersToHectopascals    { static double factor() { return 860.142806;   } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CPressureUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CPressureUnit &defaultUnit() { return hPa(); }

            /*!
             * Null unit
             */
            static const CPressureUnit &nullUnit()
            {
                static CPressureUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Pascal
             */
            static const CPressureUnit &Pa()
            {
                static CPressureUnit Pa(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal"), "Pa", LinearConverter<PascalsToHectopascals>());
                return Pa;
            }

            /*!
             * Hectopascal
             */
            static const CPressureUnit &hPa()
            {
                static CPressureUnit hPa(QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal"), "hPa", IdentityConverter());
                return hPa;
            }

            /*!
             * Pounds per square inch
             */
            static const CPressureUnit &psi()
            {
                static CPressureUnit psi(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound per square inch"), "psi", LinearConverter<PsiToHectopascals>(), 2);
                return psi;
            }

            /*!
             * Bar
             */
            static const CPressureUnit &bar()
            {
                static CPressureUnit bar(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar"), "bar", LinearConverter<Kilo<One> >(), 1);
                return bar;
            }

            /*!
             * Millibar, actually the same as hPa
             */
            static const CPressureUnit &mbar()
            {
                static CPressureUnit mbar(QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar"), "mbar", IdentityConverter(), 1);
                return mbar;
            }

            /*!
             * Inch of mercury at 0°C
             */
            static const CPressureUnit &inHg()
            {
                static CPressureUnit inhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury"), "inHg", LinearConverter<InchesToHectopascals>());
                return inhg;
            }

            /*!
             * Millimeter of mercury
             */
            static const CPressureUnit &mmHg()
            {
                static CPressureUnit mmhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury"), "mmHg", LinearConverter<MillimetersToHectopascals>());
                return mmhg;
            }

            /*!
             * All units
             */
            static const QList<CPressureUnit> &allUnits()
            {
                static QList<CPressureUnit> u;
                if (u.isEmpty())
                {
                    u.append(CPressureUnit::nullUnit());
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
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CPressureUnit>(unitName);
            }
        };

        /*!
         * Specialized class for temperatur units (kelvin, centidegree).
         */
        class CTemperatureUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CTemperatureUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CTemperatureUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            struct KelvinToCentigrade
            {
                static double factor() { return 1.0; }
                static double offset() { return 273.15; }
            };
            struct FahrenheitToCentigrade
            {
                static double factor() { return 5.0 / 9.0; }
                static double offset() { return 32.0; }
            };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CTemperatureUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CTemperatureUnit &defaultUnit() { return C(); }

            /*!
             * Null unit
             */
            static const CTemperatureUnit &nullUnit()
            {
                static CTemperatureUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Kelvin
             */
            static const CTemperatureUnit &K()
            {
                static CTemperatureUnit K(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin"), "K", AffineConverter<KelvinToCentigrade>());
                return K;
            }

            /*!
             * Centigrade C
             */
            static const CTemperatureUnit &C()
            {
                static CTemperatureUnit C(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade"), QT_TRANSLATE_NOOP("CMeasurementUnit", "C"), IdentityConverter());
                return C;
            }

            /*!
             * Fahrenheit F
             */
            static const CTemperatureUnit &F()
            {
                static CTemperatureUnit F(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit"), QT_TRANSLATE_NOOP("CMeasurementUnit", "F"), AffineConverter<FahrenheitToCentigrade>());
                return F;
            }

            /*!
             * All units
             */
            static const QList<CTemperatureUnit> &allUnits()
            {
                static QList<CTemperatureUnit> u;
                if (u.isEmpty())
                {
                    u.append(CTemperatureUnit::nullUnit());
                    u.append(CTemperatureUnit::C());
                    u.append(CTemperatureUnit::F());
                    u.append(CTemperatureUnit::K());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unitName);
            }
        };

        /*!
         * Specialized class for speed units (m/s, ft/s, NM/h).
         */
        class CSpeedUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CSpeedUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CSpeedUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            struct KnotsToMps       { static double factor() { return 1852.0    / 3600.0; } };
            struct KphToMps         { static double factor() { return    1.0    /    3.6; } };
            struct FtPerSecToMps    { static double factor() { return    0.3048         ; } };
            struct FtPerMinToMps    { static double factor() { return    0.3048 /   60.0; } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CSpeedUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CSpeedUnit &defaultUnit() { return m_s(); }

            /*!
             * Null unit
             */
            static const CSpeedUnit &nullUnit()
            {
                static CSpeedUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Meter/second m/s
             */
            static const CSpeedUnit &m_s()
            {
                static CSpeedUnit ms(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second"), "m/s", IdentityConverter());
                return ms;
            }

            /*!
             * Knots
             */
            static const CSpeedUnit &kts()
            {
                static CSpeedUnit kts(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot"), "kts", LinearConverter<KnotsToMps>(), 1);
                return kts;
            }

            /*!
             * Nautical miles per hour NM/h (same as kts)
             */
            static const CSpeedUnit &NM_h()
            {
                static CSpeedUnit NMh(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile per hour"), "NM/h", LinearConverter<KnotsToMps>(), 1);
                return NMh;
            }

            /*!
             * Feet/second ft/s
             */
            static const CSpeedUnit &ft_s()
            {
                static CSpeedUnit fts(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second"), "ft/s", LinearConverter<FtPerSecToMps>(), 0);
                return fts;
            }

            /*!
             * Feet/min ft/min
             */
            static const CSpeedUnit &ft_min()
            {
                static CSpeedUnit ftmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per minute"), "ft/min", LinearConverter<FtPerMinToMps>(), 0);
                return ftmin;
            }

            /*!
             * Kilometer/hour km/h
             */
            static const CSpeedUnit &km_h()
            {
                static CSpeedUnit kmh(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer per hour"), "km/h", LinearConverter<KphToMps>(), 1);
                return kmh;
            }

            /*!
             * All units
             */
            static const QList<CSpeedUnit> &allUnits()
            {
                static QList<CSpeedUnit> u;
                if (u.isEmpty())
                {
                    u.append(CSpeedUnit::nullUnit());
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
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unitName);
            }
        };

        /*!
         * Specialized class for time units (ms, hour, min).
         */
        class CTimeUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CTimeUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CTimeUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            typedef One SecondsToSeconds;
            struct DaysToSeconds    { static double factor() { return 60.0 * 60.0 * 24.0;   } };
            struct HoursToSeconds   { static double factor() { return 60.0 * 60.0;          } };
            struct MinutesToSeconds { static double factor() { return 60.0;                 } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CTimeUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CTimeUnit &defaultUnit() { return s(); }

            /*!
             * Null unit
             */
            static const CTimeUnit &nullUnit()
            {
                static CTimeUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
             */
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            /*!
             * Second s
             */
            static const CTimeUnit &s()
            {
                static CTimeUnit s(QT_TRANSLATE_NOOP("CMeasurementUnit", "second"), "s", IdentityConverter(), 1);
                return s;
            }

            /*!
             * Millisecond ms
             */
            static const CTimeUnit &ms()
            {
                static CTimeUnit ms(QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond"), "ms", LinearConverter<Milli<SecondsToSeconds> >(), 0);
                return ms;
            }

            /*!
             * Hour
             */
            static const CTimeUnit &h()
            {
                static CTimeUnit h(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour"), "h", LinearConverter<HoursToSeconds>(), 1);
                return h;
            }

            /*!
             * Minute
             */
            static const CTimeUnit &min()
            {
                static CTimeUnit minute(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute"), "min", LinearConverter<MinutesToSeconds>(), 2);
                return minute;
            }

            /*!
             * Day
             */
            static const CTimeUnit &d()
            {
                static CTimeUnit day(QT_TRANSLATE_NOOP("CMeasurementUnit", "day"), "d", LinearConverter<DaysToSeconds>(), 1);
                return day;
            }

            /*!
             * Hours, minutes, seconds
             */
            static const CTimeUnit &hms()
            {
                static CTimeUnit hms(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute, second"), "hms", SubdivisionConverter2<HoursToSeconds, InEachHundred<60> >(), 4);
                return hms;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
            }

            /*!
             * Hours, minutes
             */
            static const CTimeUnit &hrmin()
            {
                static CTimeUnit hrmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute"), "hm", SubdivisionConverter<HoursToSeconds, InEachHundred<60> >(), 3);
                return hrmin;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
            }

            /*!
             * Minutes, seconds
             */
            static const CTimeUnit &minsec()
            {
                static CTimeUnit minsec(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute, second"), "minsec", SubdivisionConverter<MinutesToSeconds, InEachHundred<60> >(), 2);
                return minsec;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1m%L2s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1m%L2s");
            }

            /*!
             * All units
             */
            static const QList<CTimeUnit> &allUnits()
            {
                static QList<CTimeUnit> u;
                if (u.isEmpty())
                {
                    u.append(CTimeUnit::nullUnit());
                    u.append(CTimeUnit::d());
                    u.append(CTimeUnit::h());
                    u.append(CTimeUnit::hms());
                    u.append(CTimeUnit::hrmin());
                    u.append(CTimeUnit::min());
                    u.append(CTimeUnit::ms());
                    u.append(CTimeUnit::s());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CTimeUnit>(unitName);
            }
        };

        /*!
         * Specialized class for acceleration units (m/s2, ft/s2).
         */
        class CAccelerationUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CAccelerationUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            CAccelerationUnit(const QString &name, const QString &symbol, std::nullptr_t) :
                CMeasurementUnit(name, symbol, nullptr)
            {}

            struct FeetToMeters { static double factor() { return 0.3048; } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * Default unit
             */
            static const CAccelerationUnit &defaultUnit() { return m_s2(); }

            /*!
             * Null unit
             */
            static const CAccelerationUnit &nullUnit()
            {
                static CAccelerationUnit nu("null", "null", nullptr);
                return nu;
            }

            /*!
             * Meter/second^2 (m/s^2)
             */
            static const CAccelerationUnit &m_s2()
            {
                static CAccelerationUnit ms2(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second per second"), QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2"), IdentityConverter(), 1);
                return ms2;
            }

            /*!
             * Feet/second^2
             */
            static const CAccelerationUnit &ft_s2()
            {
                static CAccelerationUnit fts2(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second per second"), QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2"), LinearConverter<FeetToMeters>(), 0);
                return fts2;
            }

            /*!
             * All units
             */
            static const QList<CAccelerationUnit> &allUnits()
            {
                static QList<CAccelerationUnit> u;
                if (u.isEmpty())
                {
                    u.append(CAccelerationUnit::nullUnit());
                    u.append(CAccelerationUnit::ft_s2());
                    u.append(CAccelerationUnit::m_s2());
                }
                return u;
            }

        protected:
            /*!
             * \copydoc CValueObject::unmarshallFromDbus
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
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

#pragma pop_macro("min")
#endif // guard
