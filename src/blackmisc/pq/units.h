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

            virtual void anchor();

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
            static const CLengthUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CLengthUnit nu(nu_);
                return nu;
            }

            //! Meter m
            static const CLengthUnit &m()
            {
                static const CMeasurementUnit::Data m_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter")), constQLatin1("m"), IdentityConverter());
                static const CLengthUnit m(m_);
                return m;
            }

            //! Nautical miles NM
            static const CLengthUnit &NM()
            {
                static const CMeasurementUnit::Data NM_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile")), constQLatin1("NM"), LinearConverter<NauticalMilesToMeters>(), 3);
                static const CLengthUnit NM(NM_);
                return NM;
            }

            //! Foot ft
            static const CLengthUnit &ft()
            {
                static const CMeasurementUnit::Data ft_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot")), constQLatin1("ft"), LinearConverter<FeetToMeters>(), 1);
                static const CLengthUnit ft(ft_);
                return ft;
            }

            //! Kilometer km
            static const CLengthUnit &km()
            {
                static const CMeasurementUnit::Data km_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer")), constQLatin1("km"), LinearConverter<Kilo<MetersToMeters> >(), 3);
                static const CLengthUnit km(km_);
                return km;
            }

            //! Centimeter cm
            static const CLengthUnit &cm()
            {
                static const CMeasurementUnit::Data cm_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter")), constQLatin1("cm"), LinearConverter<Centi<MetersToMeters> >(), 1);
                static const CLengthUnit cm(cm_);
                return cm;
            }

            //! International mile
            static const CLengthUnit &mi()
            {
                static const CMeasurementUnit::Data mi_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile")), constQLatin1("mi"), LinearConverter<MilesToMeters>(), 3);
                static const CLengthUnit mi(mi_);
                return mi;
            }

            //! Statute mile
            static const CLengthUnit &SM()
            {
                static const CMeasurementUnit::Data sm_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute mile")), constQLatin1("SM"), LinearConverter<StatuteMilesToMeters>(), 3);
                static const CLengthUnit sm(sm_);
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
            static const CAngleUnit &defaultUnit() { return deg(); }

            //! Null unit
            static const CAngleUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CAngleUnit nu(nu_);
                return nu;
            }

            //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            //! Radians
            static const CAngleUnit &rad()
            {
                static constexpr CMeasurementUnit::Data rad_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian")), constQLatin1("rad"), LinearConverter<RadiansToDegrees>());
                static const CAngleUnit rad(rad_);
                return rad;
            }

            //! Degrees
            static const CAngleUnit &deg()
            {
                static const CMeasurementUnit::Data deg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "deg")), IdentityConverter());
                static const CAngleUnit deg(deg_);
                return deg;
            }

            //! Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
            static const CAngleUnit &sexagesimalDeg()
            {
                static const CMeasurementUnit::Data deg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute, second")), constQLatin1("DMS"), SubdivisionConverter2<DegreesToDegrees, InEachHundred<60> >(), 4);
                static const CAngleUnit deg(deg_);
                return deg;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2 %L3");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2 %L3");
            }

            //! Sexagesimal degree (degrees, minutes, decimal minutes)
            static const CAngleUnit &sexagesimalDegMin()
            {
                static const CMeasurementUnit::Data deg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute")), constQLatin1("MinDec"), SubdivisionConverter<DegreesToDegrees, InEachHundred<60> >(), 4);
                static const CAngleUnit deg(deg_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CFrequencyUnit)

            //! Default constructor, required for Qt Metasystem
            CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CFrequencyUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CFrequencyUnit &defaultUnit() { return Hz(); }

            //! Null unit
            static const CFrequencyUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CFrequencyUnit nu(nu_);
                return nu;
            }

            //! Hertz
            static const CFrequencyUnit &Hz()
            {
                static const CMeasurementUnit::Data Hz_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz")), constQLatin1("Hz"), IdentityConverter());
                static const CFrequencyUnit Hz(Hz_);
                return Hz;
            }

            //! Kilohertz
            static const CFrequencyUnit &kHz()
            {
                static const CMeasurementUnit::Data kHz_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilohertz")), constQLatin1("kHz"), LinearConverter<Kilo<HertzToHertz> >(), 1);
                static const CFrequencyUnit kHz(kHz_);
                return kHz;
            }

            //! Megahertz
            static const CFrequencyUnit &MHz()
            {
                static const CMeasurementUnit::Data MHz_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "megahertz")), constQLatin1("MHz"), LinearConverter<Mega<HertzToHertz> >(), 2);
                static const CFrequencyUnit MHz(MHz_);
                return MHz;
            }

            //! Gigahertz
            static const CFrequencyUnit &GHz()
            {
                static const CMeasurementUnit::Data GHz_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gigahertz")), constQLatin1("GHz"), LinearConverter<Giga<HertzToHertz> >(), 2);
                static const CFrequencyUnit GHz(GHz_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CMassUnit)

            //! Default constructor, required for Qt Metasystem
            CMassUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CMassUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CMassUnit &defaultUnit() { return kg(); }

            //! Null unit
            static const CMassUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CMassUnit nu(nu_);
                return nu;
            }

            //! Kilogram, SI base unit
            static const CMassUnit &kg()
            {
                static const CMeasurementUnit::Data kg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram")), constQLatin1("kg"), IdentityConverter(), 1);
                static const CMassUnit kg(kg_);
                return kg;
            }

            //! Gram, SI unit
            static const CMassUnit &g()
            {
                static const CMeasurementUnit::Data g_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram")), constQLatin1("g"), LinearConverter<GramsToKilograms>(), 0);
                static const CMassUnit g(g_);
                return g;
            }

            //! Tonne, aka metric ton (1000kg)
            static const CMassUnit &tonne()
            {
                static const CMeasurementUnit::Data t_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne")), constQLatin1("t"), LinearConverter<Mega<GramsToKilograms> >(), 3);
                static const CMassUnit t(t_);
                return t;
            }

            //! Short ton (2000lb) used in the United States
            static const CMassUnit &shortTon()
            {
                static const CMeasurementUnit::Data ton_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "short ton")), constQLatin1("ton"), LinearConverter<Two<Kilo<PoundsToKilograms> > >(), 3);
                static const CMassUnit ton(ton_);
                return ton;
            }

            //! Pound, aka mass pound
            static const CMassUnit &lb()
            {
                static const CMeasurementUnit::Data lbs_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound")), constQLatin1("lb"), LinearConverter<PoundsToKilograms>(), 1);
                static const CMassUnit lbs(lbs_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CPressureUnit)

            //! Default constructor, required for Qt Metasystem
            CPressureUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CPressureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CPressureUnit &defaultUnit() { return hPa(); }

            //! Null unit
            static const CPressureUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CPressureUnit nu(nu_);
                return nu;
            }

            //! Pascal
            static const CPressureUnit &Pa()
            {
                static const CMeasurementUnit::Data Pa_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal")), constQLatin1("Pa"), LinearConverter<PascalsToHectopascals>());
                static const CPressureUnit Pa(Pa_);
                return Pa;
            }

            //! Hectopascal
            static const CPressureUnit &hPa()
            {
                static const CMeasurementUnit::Data hPa_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal")), constQLatin1("hPa"), IdentityConverter());
                static const CPressureUnit hPa(hPa_);
                return hPa;
            }

            //! Pounds per square inch
            static const CPressureUnit &psi()
            {
                static const CMeasurementUnit::Data psi_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound per square inch")), constQLatin1("psi"), LinearConverter<PsiToHectopascals>(), 2);
                static const CPressureUnit psi(psi_);
                return psi;
            }

            //! Bar
            static const CPressureUnit &bar()
            {
                static const CMeasurementUnit::Data bar_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar")), constQLatin1("bar"), LinearConverter<Kilo<One> >(), 1);
                static const CPressureUnit bar(bar_);
                return bar;
            }

            //! Millibar, actually the same as hPa
            static const CPressureUnit &mbar()
            {
                static const CMeasurementUnit::Data mbar_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar")), constQLatin1("mbar"), IdentityConverter(), 1);
                static const CPressureUnit mbar(mbar_);
                return mbar;
            }

            //! Inch of mercury at 0°C
            static const CPressureUnit &inHg()
            {
                static const CMeasurementUnit::Data inHg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury")), constQLatin1("inHg"), LinearConverter<InchesToHectopascals>());
                static const CPressureUnit inHg(inHg_);
                return inHg;
            }

            //! Millimeter of mercury
            static const CPressureUnit &mmHg()
            {
                static const CMeasurementUnit::Data mmhg_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury")), constQLatin1("mmHg"), LinearConverter<MillimetersToHectopascals>());
                static const CPressureUnit mmhg(mmhg_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CTemperatureUnit)

            //! Default constructor, required for Qt Metasystem
            CTemperatureUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CTemperatureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CTemperatureUnit &defaultUnit() { return C(); }

            //! Null unit
            static const CTemperatureUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CTemperatureUnit nu(nu_);
                return nu;
            }

            //! Kelvin
            static const CTemperatureUnit &K()
            {
                static const CMeasurementUnit::Data K_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin")), constQLatin1("K"), AffineConverter<KelvinToCentigrade>());
                static const CTemperatureUnit K(K_);
                return K;
            }

            //! Centigrade C
            static const CTemperatureUnit &C()
            {
                static const CMeasurementUnit::Data C_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "C")), IdentityConverter());
                static const CTemperatureUnit C(C_);
                return C;
            }

            //! Fahrenheit F
            static const CTemperatureUnit &F()
            {
                static const CMeasurementUnit::Data F_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "F")), AffineConverter<FahrenheitToCentigrade>());
                static const CTemperatureUnit F(F_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CSpeedUnit)

            //! Default constructor, required for Qt Metasystem
            CSpeedUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CSpeedUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CSpeedUnit &defaultUnit() { return m_s(); }

            //! Null unit
            static const CSpeedUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CSpeedUnit nu(nu_);
                return nu;
            }

            //! Meter/second m/s
            static const CSpeedUnit &m_s()
            {
                static const CMeasurementUnit::Data ms_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second")), constQLatin1("m/s"), IdentityConverter());
                static const CSpeedUnit ms(ms_);
                return ms;
            }

            //! Knots
            static const CSpeedUnit &kts()
            {
                static const CMeasurementUnit::Data kts_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot")), constQLatin1("kts"), LinearConverter<KnotsToMps>(), 1);
                static const CSpeedUnit kts(kts_);
                return kts;
            }

            //! Nautical miles per hour NM/h (same as kts)
            static const CSpeedUnit &NM_h()
            {
                static const CMeasurementUnit::Data NMh_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile per hour")), constQLatin1("NM/h"), LinearConverter<KnotsToMps>(), 1);
                static const CSpeedUnit NMh(NMh_);
                return NMh;
            }

            //! Feet/second ft/s
            static const CSpeedUnit &ft_s()
            {
                static const CMeasurementUnit::Data fts_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second")), constQLatin1("ft/s"), LinearConverter<FtPerSecToMps>(), 0);
                static const CSpeedUnit fts(fts_);
                return fts;
            }

            //! Feet/min ft/min
            static const CSpeedUnit &ft_min()
            {
                static const CMeasurementUnit::Data ftmin_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per minute")), constQLatin1("ft/min"), LinearConverter<FtPerMinToMps>(), 0);
                static const CSpeedUnit ftmin(ftmin_);
                return ftmin;
            }

            //! Kilometer/hour km/h
            static const CSpeedUnit &km_h()
            {
                static const CMeasurementUnit::Data kmh_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer per hour")), constQLatin1("km/h"), LinearConverter<KphToMps>(), 1);
                static const CSpeedUnit kmh(kmh_);
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
            static const CTimeUnit &defaultUnit() { return s(); }

            //! Null unit
            static const CTimeUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CTimeUnit nu(nu_);
                return nu;
            }

            //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
            virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool i18n = false) const override;

            //! Second s
            static const CTimeUnit &s()
            {
                static const CMeasurementUnit::Data s_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "second")), constQLatin1("s"), IdentityConverter(), 1);
                static const CTimeUnit s(s_);
                return s;
            }

            //! Millisecond ms
            static const CTimeUnit &ms()
            {
                static const CMeasurementUnit::Data ms_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond")), constQLatin1("ms"), LinearConverter<Milli<SecondsToSeconds> >(), 0);
                static const CTimeUnit ms(ms_);
                return ms;
            }

            //! Hour
            static const CTimeUnit &h()
            {
                static const CMeasurementUnit::Data h_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour")), constQLatin1("h"), LinearConverter<HoursToSeconds>(), 1);
                static const CTimeUnit h(h_);
                return h;
            }

            //! Minute
            static const CTimeUnit &min()
            {
                static const CMeasurementUnit::Data minute_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute")), constQLatin1("min"), LinearConverter<MinutesToSeconds>(), 2);
                static const CTimeUnit minute(minute_);
                return minute;
            }

            //! Day
            static const CTimeUnit &d()
            {
                static const CMeasurementUnit::Data day_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "day")), constQLatin1("d"), LinearConverter<DaysToSeconds>(), 1);
                static const CTimeUnit day(day_);
                return day;
            }

            //! Hours, minutes, seconds
            static const CTimeUnit &hms()
            {
                static const CMeasurementUnit::Data hms_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute, second")), constQLatin1("hms"), SubdivisionConverter2<HoursToSeconds, InEachHundred<60> >(), 4);
                static const CTimeUnit hms(hms_);
                return hms;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
            }

            //! Hours, minutes
            static const CTimeUnit &hrmin()
            {
                static const CMeasurementUnit::Data hrmin_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute")), constQLatin1("hm"), SubdivisionConverter<HoursToSeconds, InEachHundred<60> >(), 3);
                static const CTimeUnit hrmin(hrmin_);
                return hrmin;
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
                (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
            }

            //! Minutes, seconds
            static const CTimeUnit &minsec()
            {
                static const CMeasurementUnit::Data minsec_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute, second")), constQLatin1("minsec"), SubdivisionConverter<MinutesToSeconds, InEachHundred<60> >(), 2);
                static const CTimeUnit minsec(minsec_);
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

            virtual void anchor();

        public:
            //! Base type
            using base_type = CMeasurementUnit;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAccelerationUnit)

            //! Default constructor, required for Qt Metasystem
            CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

            //! Null constructor
            CAccelerationUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

            //! Default unit
            static const CAccelerationUnit &defaultUnit() { return m_s2(); }

            //! Null unit
            static const CAccelerationUnit &nullUnit()
            {
                static const CMeasurementUnit::Data nu_(constQLatin1("null"), constQLatin1("null"));
                static const CAccelerationUnit nu(nu_);
                return nu;
            }

            //! Meter/second^2 (m/s^2)
            static const CAccelerationUnit &m_s2()
            {
                static const CMeasurementUnit::Data ms2_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second per second")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2")), IdentityConverter(), 1);
                static const CAccelerationUnit ms2(ms2_);
                return ms2;
            }

            //! Feet/second^2
            static const CAccelerationUnit &ft_s2()
            {
                static const CMeasurementUnit::Data fts2_(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second per second")), constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2")), LinearConverter<FeetToMeters>(), 0);
                static const CAccelerationUnit fts2(fts2_);
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
