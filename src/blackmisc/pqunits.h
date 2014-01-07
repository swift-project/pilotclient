/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
         * \brief Specialized class for distance units (meter, foot, nautical miles).
         */
        class CLengthUnit : public CMeasurementUnit
        {
        private:
            template <class Converter>
            CLengthUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
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
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CLengthUnit m(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter"), "m", IdentityConverter());
                return m;
            }

            /*!
             * \brief Nautical miles NM
             * \return
             */
            static const CLengthUnit &NM()
            {
                static CLengthUnit NM(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile"), "NM", LinearConverter<NauticalMilesToMeters>(), 3);
                return NM;
            }

            /*!
             * \brief Foot ft
             * \return
             */
            static const CLengthUnit &ft()
            {
                static CLengthUnit ft(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot"), "ft", LinearConverter<FeetToMeters>(), 0);
                return ft;
            }

            /*!
             * \brief Kilometer km
             * \return
             */
            static const CLengthUnit &km()
            {
                static CLengthUnit km(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer"), "km", LinearConverter<Kilo<MetersToMeters> >(), 3);
                return km;
            }

            /*!
             * \brief Centimeter cm
             * \return
             */
            static const CLengthUnit &cm()
            {
                static CLengthUnit cm(QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter"), "cm", LinearConverter<Centi<MetersToMeters> >(), 1);
                return cm;
            }

            /*!
             * \brief International mile
             * \return
             */
            static const CLengthUnit &mi()
            {
                static CLengthUnit mi(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile"), "mi", LinearConverter<MilesToMeters>(), 3);
                return mi;
            }

            /*!
             * \brief Statute mile
             * \return
             */
            static const CLengthUnit &SM()
            {
                static CLengthUnit sm(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute mile"), "SM", LinearConverter<StatuteMilesToMeters>(), 3);
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
            template <class Converter>
            CAngleUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            struct RadiansToDegrees { static double factor() { return 180.0 / M_PI; } };
            typedef One DegreesToDegrees;

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CAngleUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const;

            /*!
             * \brief Radians
             * \return
             */
            static const CAngleUnit &rad()
            {
                static CAngleUnit rad(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian"), "rad", LinearConverter<RadiansToDegrees>());
                return rad;
            }

            /*!
             * \brief Degrees
             * \return
             */
            static const CAngleUnit &deg()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree"), QT_TRANSLATE_NOOP("CMeasurementUnit", "deg"), IdentityConverter());
                return deg;
            }

            /*!
             * \brief Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
             * \return
             */
            static const CAngleUnit &sexagesimalDeg()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute, second"), "DMS", SubdivisionConverter2<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2 %L3");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2 %L3");
            }

            /*!
             * \brief Sexagesimal degree (degrees, minutes, decimal minutes)
             * \return
             */
            static const CAngleUnit &sexagesimalDegMin()
            {
                static CAngleUnit deg(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute"), "MinDec", SubdivisionConverter<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2");
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
            template <class Converter>
            CFrequencyUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            typedef One HertzToHertz;

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CFrequencyUnit Hz(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz"), "Hz", IdentityConverter());
                return Hz;
            }

            /*!
             * \brief Kilohertz
             * \return
             */
            static const CFrequencyUnit &kHz()
            {
                static CFrequencyUnit kHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilohertz"), "kHz", LinearConverter<Kilo<HertzToHertz> >(), 1);
                return kHz;
            }

            /*!
             * \brief Megahertz
             * \return
             */
            static const CFrequencyUnit &MHz()
            {
                static CFrequencyUnit MHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "megahertz"), "MHz", LinearConverter<Mega<HertzToHertz> >(), 2);
                return MHz;
            }

            /*!
             * \brief Gigahertz
             * \return
             */
            static const CFrequencyUnit &GHz()
            {
                static CFrequencyUnit GHz(QT_TRANSLATE_NOOP("CMeasurementUnit", "gigahertz"), "GHz", LinearConverter<Giga<HertzToHertz> >(), 2);
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
            template <class Converter>
            CMassUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            typedef Milli<One> GramsToKilograms;
            struct PoundsToKilograms { static double factor() { return 0.45359237; } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CMassUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CMassUnit kg(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram"), "kg", IdentityConverter(), 1);
                return kg;
            }

            /*!
             * \brief Gram, SI unit
             * \return
             */
            static const CMassUnit &g()
            {
                static CMassUnit g(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram"), "g", LinearConverter<GramsToKilograms>(), 0);
                return g;
            }

            /*!
             * \brief Tonne, aka metric ton (1000kg)
             * \return
             */
            static const CMassUnit &tonne()
            {
                static CMassUnit t(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne"), "t", LinearConverter<Mega<GramsToKilograms> >(), 3);
                return t;
            }

            /*!
             * \brief Short ton (2000lb) used in the United States
             * \return
             */
            static const CMassUnit &shortTon()
            {
                static CMassUnit ton(QT_TRANSLATE_NOOP("CMeasurementUnit", "short ton"), "ton", LinearConverter<Two<Kilo<PoundsToKilograms> > >(), 3);
                return ton;
            }

            /*!
             * \brief Pound, aka mass pound
             * \return
             */
            static const CMassUnit &lb()
            {
                static CMassUnit lbs(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound"), "lb", LinearConverter<PoundsToKilograms>(), 1);
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
                    u.append(CMassUnit::tonne());
                    u.append(CMassUnit::shortTon());
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
            template <class Converter>
            CPressureUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
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
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CPressureUnit Pa(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal"), "Pa", LinearConverter<PascalsToHectopascals>());
                return Pa;
            }

            /*!
             * \brief Hectopascal
             * \return
             */
            static const CPressureUnit &hPa()
            {
                static CPressureUnit hPa(QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal"), "hPa", IdentityConverter());
                return hPa;
            }

            /*!
             * \brief Pounds per square inch
             * \return
             */
            static const CPressureUnit &psi()
            {
                static CPressureUnit psi(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound per square inch"), "psi", LinearConverter<PsiToHectopascals>(), 2);
                return psi;
            }

            /*!
             * \brief Bar
             * \return
             */
            static const CPressureUnit &bar()
            {
                static CPressureUnit bar(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar"), "bar", LinearConverter<Kilo<One> >(), 1);
                return bar;
            }

            /*!
             * \brief Millibar, actually the same as hPa
             * \return
             */
            static const CPressureUnit &mbar()
            {
                static CPressureUnit mbar(QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar"), "mbar", IdentityConverter(), 1);
                return mbar;
            }

            /*!
             * \brief Inch of mercury at 0°C
             * \return
             */
            static const CPressureUnit &inHg()
            {
                static CPressureUnit inhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury"), "inHg", LinearConverter<InchesToHectopascals>());
                return inhg;
            }

            /*!
             * \brief Millimeter of mercury
             * \return
             */
            static const CPressureUnit &mmHg()
            {
                static CPressureUnit mmhg(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury"), "mmHg", LinearConverter<MillimetersToHectopascals>());
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
            template <class Converter>
            CTemperatureUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
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
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CTemperatureUnit K(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin"), "K", AffineConverter<KelvinToCentigrade>());
                return K;
            }

            /*!
             * \brief Centigrade C
             * \return
             */
            static const CTemperatureUnit &C()
            {
                static CTemperatureUnit C(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade"), QT_TRANSLATE_NOOP("CMeasurementUnit", "C"), IdentityConverter());
                return C;
            }

            /*!
             * \brief Fahrenheit F
             * \return
             */
            static const CTemperatureUnit &F()
            {
                static CTemperatureUnit F(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit"), QT_TRANSLATE_NOOP("CMeasurementUnit", "F"), AffineConverter<FahrenheitToCentigrade>());
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
            template <class Converter>
            CSpeedUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
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
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CSpeedUnit ms(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second"), "m/s", IdentityConverter());
                return ms;
            }

            /*!
             * \brief Knots
             * \return
             */
            static const CSpeedUnit &kts()
            {
                static CSpeedUnit kts(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot"), "kts", LinearConverter<KnotsToMps>(), 1);
                return kts;
            }

            /*!
             * \brief Nautical miles per hour NM/h (same as kts)
             * \return
             */
            static const CSpeedUnit &NM_h()
            {
                static CSpeedUnit NMh(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile per hour"), "NM/h", LinearConverter<KnotsToMps>(), 1);
                return NMh;
            }

            /*!
             * \brief Feet/second ft/s
             * \return
             */
            static const CSpeedUnit &ft_s()
            {
                static CSpeedUnit fts(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second"), "ft/s", LinearConverter<FtPerSecToMps>(), 0);
                return fts;
            }

            /*!
             * \brief Feet/min ft/min
             * \return
             */
            static const CSpeedUnit &ft_min()
            {
                static CSpeedUnit ftmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per minute"), "ft/min", LinearConverter<FtPerMinToMps>(), 0);
                return ftmin;
            }

            /*!
             * \brief Kilometer/hour km/h
             * \return
             */
            static const CSpeedUnit &km_h()
            {
                static CSpeedUnit kmh(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer per hour"), "km/h", LinearConverter<KphToMps>(), 1);
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
            template <class Converter>
            CTimeUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
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
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const;

            /*!
             * \brief Second s
             * \return
             */
            static const CTimeUnit &s()
            {
                static CTimeUnit s(QT_TRANSLATE_NOOP("CMeasurementUnit", "second"), "s", IdentityConverter(), 1);
                return s;
            }

            /*!
             * \brief Millisecond ms
             * \return
             */
            static const CTimeUnit &ms()
            {
                static CTimeUnit ms(QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond"), "ms", LinearConverter<Milli<SecondsToSeconds> >(), 0);
                return ms;
            }

            /*!
             * \brief Hour
             * \return
             */
            static const CTimeUnit &h()
            {
                static CTimeUnit h(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour"), "h", LinearConverter<HoursToSeconds>(), 1);
                return h;
            }


            /*!
             * \brief Minute
             * \return
             */
            static const CTimeUnit &min()
            {
                static CTimeUnit minute(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute"), "min", LinearConverter<MinutesToSeconds>(), 2);
                return minute;
            }

            /*!
             * \brief Day
             * \return
             */
            static const CTimeUnit &d()
            {
                static CTimeUnit day(QT_TRANSLATE_NOOP("CMeasurementUnit", "day"), "d", LinearConverter<DaysToSeconds>(), 1);
                return day;
            }

            /*!
             * \brief Hours, minutes, seconds
             */
            static const CTimeUnit &hms()
            {
                static CTimeUnit hms(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute, second"), "hms", SubdivisionConverter2<HoursToSeconds, InEachHundred<60> >(), 4);
                return hms;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
            }

            /*!
             * \brief Hours, minutes
             */
            static const CTimeUnit &hrmin()
            {
                static CTimeUnit hrmin(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute"), "hm", SubdivisionConverter<HoursToSeconds, InEachHundred<60> >(), 3);
                return hrmin;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
            }

            /*!
             * \brief Minutes, seconds
             */
            static const CTimeUnit &minsec()
            {
                static CTimeUnit minsec(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute, second"), "minsec", SubdivisionConverter<MinutesToSeconds, InEachHundred<60> >(), 2);
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
            template <class Converter>
            CAccelerationUnit(const QString &name, const QString &symbol, const Converter &converter, int displayDigits = 2, double epsilon = 1E-9) :
                CMeasurementUnit(name, symbol, converter, displayDigits, epsilon)
            {}

            struct FeetToMeters { static double factor() { return 0.3048; } };

        public:
            /*!
             * Default constructor, required for Qt Metasystem
             */
            CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

            /*!
             * \brief Virtual method to return QVariant, used with DBUS QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

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
                static CAccelerationUnit ms2(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second per second"), QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2"), IdentityConverter(), 1);
                return ms2;
            }

            /*!
             * \brief Feet/second^2
             * \return
             */
            static const CAccelerationUnit &ft_s2()
            {
                static CAccelerationUnit fts2(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second per second"), QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2"), LinearConverter<FeetToMeters>(), 0);
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

#pragma pop_macro("min")
#endif // guard
