/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PQ_UNITS_H
#define BLACKMISC_PQ_UNITS_H

#pragma push_macro("min")
#undef min

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/dbus.h"
#include "blackmisc/pq/measurementunit.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/variant.h"

#include <math.h>
#include <QDBusArgument>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <QtMath>
#include <cstddef>

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

        //! Specialized class for distance units (meter, foot, nautical miles).
        class BLACKMISC_EXPORT CLengthUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CLengthUnit>,
            public Mixin::DBusOperators<CLengthUnit>,
            public Mixin::Index<CLengthUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            struct NauticalMilesToMeters    { static double factor() { return 1852.0;    } };
            struct FeetToMeters             { static double factor() { return    0.3048; } };
            struct MilesToMeters            { static double factor() { return 1609.344;  } };
            struct StatuteMilesToMeters     { static double factor() { return 1609.3472; } };
            using MetersToMeters = One;

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CLengthUnit)

            //! Default constructor, required for Qt Metasystem
            CLengthUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CLengthUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CLengthUnit defaultUnit() { return m(); }

            //! Null unit
            static CLengthUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Meter m
            static CLengthUnit m()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data m(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter")), constQLatin1("m"), IdentityConverter());
                return m;
            }

            //! Nautical miles NM
            static CLengthUnit NM()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data NM(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile")), constQLatin1("NM"), LinearConverter<NauticalMilesToMeters>(), 3);
                return NM;
            }

            //! Foot ft
            static CLengthUnit ft()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ft(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot")), constQLatin1("ft"), LinearConverter<FeetToMeters>(), 1);
                return ft;
            }

            //! Kilometer km
            static CLengthUnit km()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data km(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer")), constQLatin1("km"), LinearConverter<Kilo<MetersToMeters> >(), 3);
                return km;
            }

            //! Centimeter cm
            static CLengthUnit cm()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data cm(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter")), constQLatin1("cm"), LinearConverter<Centi<MetersToMeters> >(), 1);
                return cm;
            }

            //! International mile
            static CLengthUnit mi()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data mi(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile")), constQLatin1("mi"), LinearConverter<MilesToMeters>(), 3);
                return mi;
            }

            //! Statute mile
            static CLengthUnit SM()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data sm(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute mile")), constQLatin1("SM"), LinearConverter<StatuteMilesToMeters>(), 3);
                return sm;
            }

            //! All units
            static const QList<CLengthUnit> &allUnits()
            {
                static const QList<CLengthUnit> u
                {
                    CLengthUnit::nullUnit(),
                    CLengthUnit::cm(),
                    CLengthUnit::ft(),
                    CLengthUnit::km(),
                    CLengthUnit::m(),
                    CLengthUnit::mi(),
                    CLengthUnit::SM(),
                    CLengthUnit::NM()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CLengthUnit>(unitName);
            }
        };

        //! Specialized class for angles (degrees, radian).
        class BLACKMISC_EXPORT CAngleUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CAngleUnit>,
            public Mixin::DBusOperators<CAngleUnit>,
            public Mixin::Index<CAngleUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            struct RadiansToDegrees { static double factor() { return 180.0 / M_PI; } };
            using DegreesToDegrees = One;

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAngleUnit)

            //!  Default constructor, required for Qt Metasystem
            CAngleUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CAngleUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CAngleUnit defaultUnit() { return deg(); }

            //! Null unit
            static CAngleUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            //! Radians
            static CAngleUnit rad()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data rad(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian")), constQLatin1("rad"), LinearConverter<RadiansToDegrees>());
                return rad;
            }

            //! Degrees
            static CAngleUnit deg()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data deg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "deg")), IdentityConverter());
                return deg;
            }

            //! Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
            static CAngleUnit sexagesimalDeg()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data deg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute, second")), constQLatin1("DMS"), SubdivisionConverter2<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2 %L3");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2 %L3");
            }

            //! Sexagesimal degree (degrees, minutes, decimal minutes)
            static CAngleUnit sexagesimalDegMin()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data deg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute")), constQLatin1("MinDec"), SubdivisionConverter<DegreesToDegrees, InEachHundred<60> >(), 4);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2");
            }

            //! All units
            static const QList<CAngleUnit> &allUnits()
            {
                static const QList<CAngleUnit> u
                {
                    CAngleUnit::nullUnit(),
                    CAngleUnit::deg(),
                    CAngleUnit::rad(),
                    CAngleUnit::sexagesimalDeg(),
                    CAngleUnit::sexagesimalDegMin()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CAngleUnit>(unitName);
            }
        };

        //! Specialized class for frequency (hertz, mega hertz, kilo hertz).
        class BLACKMISC_EXPORT CFrequencyUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CFrequencyUnit>,
            public Mixin::DBusOperators<CFrequencyUnit>,
            public Mixin::Index<CFrequencyUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            using HertzToHertz = One;

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CFrequencyUnit)

            //! Default constructor, required for Qt Metasystem
            CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CFrequencyUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CFrequencyUnit defaultUnit() { return Hz(); }

            //! Null unit
            static CFrequencyUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Hertz
            static CFrequencyUnit Hz()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data Hz(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz")), constQLatin1("Hz"), IdentityConverter());
                return Hz;
            }

            //! Kilohertz
            static CFrequencyUnit kHz()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data kHz(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilohertz")), constQLatin1("kHz"), LinearConverter<Kilo<HertzToHertz> >(), 1);
                return kHz;
            }

            //! Megahertz
            static CFrequencyUnit MHz()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data MHz(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "megahertz")), constQLatin1("MHz"), LinearConverter<Mega<HertzToHertz> >(), 2);
                return MHz;
            }

            //! Gigahertz
            static CFrequencyUnit GHz()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data GHz(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gigahertz")), constQLatin1("GHz"), LinearConverter<Giga<HertzToHertz> >(), 2);
                return GHz;
            }

            //! All units
            static const QList<CFrequencyUnit> &allUnits()
            {
                static const QList<CFrequencyUnit> u
                {
                    CFrequencyUnit::nullUnit(),
                    CFrequencyUnit::GHz(),
                    CFrequencyUnit::Hz(),
                    CFrequencyUnit::kHz(),
                    CFrequencyUnit::MHz()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unitName);
            }
        };

        //! Specialized class for mass units (kg, lbs).
        class BLACKMISC_EXPORT CMassUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CMassUnit>,
            public Mixin::DBusOperators<CMassUnit>,
            public Mixin::Index<CMassUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            using GramsToKilograms = Milli<One>;
            struct PoundsToKilograms { static double factor() { return 0.45359237; } };

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CMassUnit)

            //! Default constructor, required for Qt Metasystem
            CMassUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CMassUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CMassUnit defaultUnit() { return kg(); }

            //! Null unit
            static CMassUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Kilogram, SI base unit
            static CMassUnit kg()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data kg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram")), constQLatin1("kg"), IdentityConverter(), 1);
                return kg;
            }

            //! Gram, SI unit
            static CMassUnit g()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data g(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram")), constQLatin1("g"), LinearConverter<GramsToKilograms>(), 0);
                return g;
            }

            //! Tonne, aka metric ton (1000kg)
            static CMassUnit tonne()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data t(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne")), constQLatin1("t"), LinearConverter<Mega<GramsToKilograms> >(), 3);
                return t;
            }

            //! Short ton (2000lb) used in the United States
            static CMassUnit shortTon()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ton(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "short ton")), constQLatin1("ton"), LinearConverter<Two<Kilo<PoundsToKilograms> > >(), 3);
                return ton;
            }

            //! Pound, aka mass pound
            static CMassUnit lb()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data lbs(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound")), constQLatin1("lb"), LinearConverter<PoundsToKilograms>(), 1);
                return lbs;
            }

            //! All units
            static const QList<CMassUnit> &allUnits()
            {
                static const QList<CMassUnit> u
                {
                    CMassUnit::nullUnit(),
                    CMassUnit::g(),
                    CMassUnit::kg(),
                    CMassUnit::lb(),
                    CMassUnit::tonne(),
                    CMassUnit::shortTon()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CMassUnit>(unitName);
            }
        };

        //! Specialized class for pressure (psi, hPa, bar).
        class BLACKMISC_EXPORT CPressureUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CPressureUnit>,
            public Mixin::DBusOperators<CPressureUnit>,
            public Mixin::Index<CPressureUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            using PascalsToHectopascals = Centi<One>;
            struct PsiToHectopascals            { static double factor() { return  68.948;      } };
            struct InchesToHectopascals         { static double factor() { return  33.86389;    } };
            struct MillimetersToHectopascals    { static double factor() { return 860.142806;   } };

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CPressureUnit)

            //! Default constructor, required for Qt Metasystem
            CPressureUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CPressureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CPressureUnit defaultUnit() { return hPa(); }

            //! Null unit
            static CPressureUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Pascal
            static CPressureUnit Pa()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data Pa(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal")), constQLatin1("Pa"), LinearConverter<PascalsToHectopascals>());
                return Pa;
            }

            //! Hectopascal
            static CPressureUnit hPa()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data hPa(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal")), constQLatin1("hPa"), IdentityConverter());
                return hPa;
            }

            //! Pounds per square inch
            static CPressureUnit psi()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data psi(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound per square inch")), constQLatin1("psi"), LinearConverter<PsiToHectopascals>(), 2);
                return psi;
            }

            //! Bar
            static CPressureUnit bar()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data bar(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar")), constQLatin1("bar"), LinearConverter<Kilo<One> >(), 1);
                return bar;
            }

            //! Millibar, actually the same as hPa
            static CPressureUnit mbar()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data mbar(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar")), constQLatin1("mbar"), IdentityConverter(), 1);
                return mbar;
            }

            //! Inch of mercury at 0°C
            static CPressureUnit inHg()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data inhg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury")), constQLatin1("inHg"), LinearConverter<InchesToHectopascals>());
                return inhg;
            }

            //! Millimeter of mercury
            static CPressureUnit mmHg()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data mmhg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury")), constQLatin1("mmHg"), LinearConverter<MillimetersToHectopascals>());
                return mmhg;
            }

            //! All units
            static const QList<CPressureUnit> &allUnits()
            {
                static const QList<CPressureUnit> u
                {
                    CPressureUnit::nullUnit(),
                    CPressureUnit::bar(),
                    CPressureUnit::hPa(),
                    CPressureUnit::inHg(),
                    CPressureUnit::mmHg(),
                    CPressureUnit::mbar(),
                    CPressureUnit::psi()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CPressureUnit>(unitName);
            }
        };

        //! Specialized class for temperatur units (kelvin, centidegree).
        class BLACKMISC_EXPORT CTemperatureUnit final :
            public CMeasurementUnit,
            public Mixin::MetaType<CTemperatureUnit>,
            public Mixin::DBusOperators<CTemperatureUnit>,
            public Mixin::Index<CTemperatureUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

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
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CTemperatureUnit)

            //! Default constructor, required for Qt Metasystem
            CTemperatureUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CTemperatureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CTemperatureUnit defaultUnit() { return C(); }

            //! Null unit
            static CTemperatureUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Kelvin
            static CTemperatureUnit K()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data K(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin")), constQLatin1("K"), AffineConverter<KelvinToCentigrade>());
                return K;
            }

            //! Centigrade C
            static CTemperatureUnit C()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data C(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "C")), IdentityConverter());
                return C;
            }

            //! Fahrenheit F
            static CTemperatureUnit F()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data F(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "F")), AffineConverter<FahrenheitToCentigrade>());
                return F;
            }

            //! All units
            static const QList<CTemperatureUnit> &allUnits()
            {
                static const QList<CTemperatureUnit> u
                {
                    CTemperatureUnit::nullUnit(),
                    CTemperatureUnit::C(),
                    CTemperatureUnit::F(),
                    CTemperatureUnit::K()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unitName);
            }
        };

        //! Specialized class for speed units (m/s, ft/s, NM/h).
        class BLACKMISC_EXPORT CSpeedUnit final:
            public CMeasurementUnit,
            public Mixin::MetaType<CSpeedUnit>,
            public Mixin::DBusOperators<CSpeedUnit>,
            public Mixin::Index<CSpeedUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            struct KnotsToMps       { static double factor() { return 1852.0    / 3600.0; } };
            struct KphToMps         { static double factor() { return    1.0    /    3.6; } };
            struct FtPerSecToMps    { static double factor() { return    0.3048         ; } };
            struct FtPerMinToMps    { static double factor() { return    0.3048 /   60.0; } };

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSpeedUnit)

            //! Default constructor, required for Qt Metasystem
            CSpeedUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CSpeedUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CSpeedUnit defaultUnit() { return m_s(); }

            //! Null unit
            static CSpeedUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Meter/second m/s
            static CSpeedUnit m_s()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ms(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second")), constQLatin1("m/s"), IdentityConverter());
                return ms;
            }

            //! Knots
            static CSpeedUnit kts()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data kts(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot")), constQLatin1("kts"), LinearConverter<KnotsToMps>(), 1);
                return kts;
            }

            //! Nautical miles per hour NM/h (same as kts)
            static CSpeedUnit NM_h()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data NMh(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile per hour")), constQLatin1("NM/h"), LinearConverter<KnotsToMps>(), 1);
                return NMh;
            }

            //! Feet/second ft/s
            static CSpeedUnit ft_s()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data fts(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second")), constQLatin1("ft/s"), LinearConverter<FtPerSecToMps>(), 0);
                return fts;
            }

            //! Feet/min ft/min
            static CSpeedUnit ft_min()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ftmin(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per minute")), constQLatin1("ft/min"), LinearConverter<FtPerMinToMps>(), 0);
                return ftmin;
            }

            //! Kilometer/hour km/h
            static CSpeedUnit km_h()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data kmh(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer per hour")), constQLatin1("km/h"), LinearConverter<KphToMps>(), 1);
                return kmh;
            }

            //! All units
            static const QList<CSpeedUnit> &allUnits()
            {
                static const QList<CSpeedUnit> u
                {
                    CSpeedUnit::nullUnit(),
                    CSpeedUnit::ft_min(),
                    CSpeedUnit::ft_s(),
                    CSpeedUnit::km_h(),
                    CSpeedUnit::kts(),
                    CSpeedUnit::m_s(),
                    CSpeedUnit::NM_h()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unitName);
            }
        };

        //! Specialized class for time units (ms, hour, min).
        class BLACKMISC_EXPORT CTimeUnit final:
            public CMeasurementUnit,
            public Mixin::MetaType<CTimeUnit>,
            public Mixin::DBusOperators<CTimeUnit>,
            public Mixin::Index<CTimeUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            using SecondsToSeconds = One;
            struct DaysToSeconds    { static double factor() { return 60.0 * 60.0 * 24.0;   } };
            struct HoursToSeconds   { static double factor() { return 60.0 * 60.0;          } };
            struct MinutesToSeconds { static double factor() { return 60.0;                 } };

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CTimeUnit)

            //! Default constructor, required for Qt Metasystem
            CTimeUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CTimeUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CTimeUnit defaultUnit() { return s(); }

            //! Null unit
            static CTimeUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            //! Second s
            static CTimeUnit s()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data s(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "second")), constQLatin1("s"), IdentityConverter(), 1);
                return s;
            }

            //! Millisecond ms
            static CTimeUnit ms()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ms(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond")), constQLatin1("ms"), LinearConverter<Milli<SecondsToSeconds> >(), 0);
                return ms;
            }

            //! Hour
            static CTimeUnit h()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data h(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour")), constQLatin1("h"), LinearConverter<HoursToSeconds>(), 1);
                return h;
            }

            //! Minute
            static CTimeUnit min()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data minute(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute")), constQLatin1("min"), LinearConverter<MinutesToSeconds>(), 2);
                return minute;
            }

            //! Day
            static CTimeUnit d()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data day(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "day")), constQLatin1("d"), LinearConverter<DaysToSeconds>(), 1);
                return day;
            }

            //! Hours, minutes, seconds
            static CTimeUnit hms()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data hms(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute, second")), constQLatin1("hms"), SubdivisionConverter2<HoursToSeconds, InEachHundred<60> >(), 4);
                return hms;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
            }

            //! Hours, minutes
            static CTimeUnit hrmin()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data hrmin(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute")), constQLatin1("hm"), SubdivisionConverter<HoursToSeconds, InEachHundred<60> >(), 3);
                return hrmin;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
            }

            //! Minutes, seconds
            static CTimeUnit minsec()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data minsec(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute, second")), constQLatin1("minsec"), SubdivisionConverter<MinutesToSeconds, InEachHundred<60> >(), 2);
                return minsec;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1m%L2s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1m%L2s");
            }

            //! All units
            static const QList<CTimeUnit> &allUnits()
            {
                static const QList<CTimeUnit> u
                {
                    CTimeUnit::nullUnit(),
                    CTimeUnit::d(),
                    CTimeUnit::h(),
                    CTimeUnit::hms(),
                    CTimeUnit::hrmin(),
                    CTimeUnit::min(),
                    CTimeUnit::ms(),
                    CTimeUnit::s()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
            {
                QString unitName;
                argument >> unitName;
                (*this) = CMeasurementUnit::unitFromSymbol<CTimeUnit>(unitName);
            }
        };

        //! Specialized class for acceleration units (m/s2, ft/s2).
        class BLACKMISC_EXPORT CAccelerationUnit final:
            public CMeasurementUnit,
            public Mixin::MetaType<CAccelerationUnit>,
            public Mixin::DBusOperators<CAccelerationUnit>,
            public Mixin::Index<CAccelerationUnit>
        {
        private:
            using CMeasurementUnit::CMeasurementUnit;

            struct FeetToMeters { static double factor() { return 0.3048; } };

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAccelerationUnit)

            //! Default constructor, required for Qt Metasystem
            CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CAccelerationUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static CAccelerationUnit defaultUnit() { return m_s2(); }

            //! Null unit
            static CAccelerationUnit nullUnit()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
                return nu;
            }

            //! Meter/second^2 (m/s^2)
            static CAccelerationUnit m_s2()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data ms2(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second per second")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2")), IdentityConverter(), 1);
                return ms2;
            }

            //! Feet/second^2
            static CAccelerationUnit ft_s2()
            {
                static Q_CONSTEXPR CMeasurementUnit::Data fts2(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second per second")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2")), LinearConverter<FeetToMeters>(), 0);
                return fts2;
            }

            //! All units
            static const QList<CAccelerationUnit> &allUnits()
            {
                static const QList<CAccelerationUnit> u
                {
                    CAccelerationUnit::nullUnit(),
                    CAccelerationUnit::ft_s2(),
                    CAccelerationUnit::m_s2()
                };
                return u;
            }

            //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
            void unmarshallFromDbus(const QDBusArgument &argument)
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
