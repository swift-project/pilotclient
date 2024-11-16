// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_PQ_UNITS_H
#define SWIFT_MISC_PQ_UNITS_H

#pragma push_macro("min")
#undef min

#include <cmath>
#include <cstddef>

#include <QDBusArgument>
#include <QList>
#include <QMetaType>
#include <QString>
#include <QtGlobal>
#include <QtMath>

#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/pq/measurementunit.h"
#include "misc/swiftmiscexport.h"

//! \cond
#define SWIFT_TEMPLATE_UNIT_MIXINS(MU, Extern, Export)                                                                 \
    namespace swift::misc::physical_quantities                                                                         \
    {                                                                                                                  \
        class MU;                                                                                                      \
    }                                                                                                                  \
    namespace swift::misc::private_ns                                                                                  \
    {                                                                                                                  \
        Extern template struct Export CValueObjectMetaInfo<physical_quantities::MU>;                                   \
        Extern template struct Export MetaTypeHelper<physical_quantities::MU>;                                         \
    }                                                                                                                  \
    namespace swift::misc::mixin                                                                                       \
    {                                                                                                                  \
        Extern template class Export MetaType<physical_quantities::MU>;                                                \
        Extern template class Export DBusOperators<physical_quantities::MU>;                                           \
        Extern template class Export DataStreamOperators<physical_quantities::MU>;                                     \
        Extern template class Export Index<physical_quantities::MU>;                                                   \
    }
//! \endcond

/*!
 * \def SWIFT_DECLARE_UNIT_MIXINS
 * Explicit template declaration of mixins for a CMeasurementUnit subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def SWIFT_DEFINE_UNIT_MIXINS
 * Explicit template definition of mixins for a CMeasurementUnit subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_DECLARE_UNIT_MIXINS(MU)
#    define SWIFT_DEFINE_UNIT_MIXINS(MU)
#elif defined(Q_OS_WIN) && defined(Q_CC_CLANG)
#    define SWIFT_DECLARE_UNIT_MIXINS(MU) SWIFT_TEMPLATE_UNIT_MIXINS(MU, extern, )
#    define SWIFT_DEFINE_UNIT_MIXINS(MU) SWIFT_TEMPLATE_UNIT_MIXINS(MU, , SWIFT_MISC_EXPORT)
#else
#    define SWIFT_DECLARE_UNIT_MIXINS(MU) SWIFT_TEMPLATE_UNIT_MIXINS(MU, extern, )
#    define SWIFT_DEFINE_UNIT_MIXINS(MU) SWIFT_TEMPLATE_UNIT_MIXINS(MU, , )
#endif

SWIFT_DECLARE_UNIT_MIXINS(CAngleUnit)
SWIFT_DECLARE_UNIT_MIXINS(CLengthUnit)
SWIFT_DECLARE_UNIT_MIXINS(CPressureUnit)
SWIFT_DECLARE_UNIT_MIXINS(CFrequencyUnit)
SWIFT_DECLARE_UNIT_MIXINS(CMassUnit)
SWIFT_DECLARE_UNIT_MIXINS(CTemperatureUnit)
SWIFT_DECLARE_UNIT_MIXINS(CSpeedUnit)
SWIFT_DECLARE_UNIT_MIXINS(CTimeUnit)
SWIFT_DECLARE_UNIT_MIXINS(CAccelerationUnit)

//
// Used with the template for quantities. This is the reason for
// having all units in one file, since template requires concrete instantiations
//
// I18N:
// http://qt-project.org/doc/qt-4.8/linguist-programmers.html#translating-text-that-is-outside-of-a-qobject-subclass
//
namespace swift::misc::physical_quantities
{
    //! Specialized class for distance units (meter, foot, nautical miles).
    class SWIFT_MISC_EXPORT CLengthUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CLengthUnit>,
        public mixin::DBusOperators<CLengthUnit>,
        public mixin::DataStreamOperators<CLengthUnit>,
        public mixin::Index<CLengthUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        struct NauticalMilesToMeters
        {
            static double factor() { return 1852.0; }
        };
        struct FeetToMeters
        {
            static double factor() { return 0.3048; }
        };
        struct MilesToMeters
        {
            static double factor() { return 1609.344; }
        };
        struct StatuteMilesToMeters
        {
            static double factor() { return 1609.3472; }
        };
        using MetersToMeters = One;

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CLengthUnit)

        //! Default constructor, required for Qt Metasystem
        CLengthUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CLengthUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CLengthUnit defaultUnit() { return m(); }

        //! Null unit
        static CLengthUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Meter m
        static CLengthUnit m()
        {
            static constexpr CMeasurementUnit::Data m(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter")),
                                                      constQLatin1("m"), IdentityConverter());
            return m;
        }

        //! Nautical miles NM
        static CLengthUnit NM()
        {
            static constexpr CMeasurementUnit::Data NM(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile")), constQLatin1("NM"),
                LinearConverter<NauticalMilesToMeters>(), 3);
            return NM;
        }

        //! Foot ft
        static CLengthUnit ft()
        {
            static constexpr CMeasurementUnit::Data ft(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot")),
                                                       constQLatin1("ft"), LinearConverter<FeetToMeters>(), 1);
            return ft;
        }

        //! Kilometer km
        static CLengthUnit km()
        {
            static constexpr CMeasurementUnit::Data km(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer")),
                                                       constQLatin1("km"), LinearConverter<Kilo<MetersToMeters>>(), 3);
            return km;
        }

        //! Centimeter cm
        static CLengthUnit cm()
        {
            static constexpr CMeasurementUnit::Data cm(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centimeter")), constQLatin1("cm"),
                LinearConverter<Centi<MetersToMeters>>(), 1);
            return cm;
        }

        //! International mile
        static CLengthUnit mi()
        {
            static constexpr CMeasurementUnit::Data mi(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "mile")),
                                                       constQLatin1("mi"), LinearConverter<MilesToMeters>(), 3);
            return mi;
        }

        //! Statute mile
        static CLengthUnit SM()
        {
            static constexpr CMeasurementUnit::Data sm(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "statute mile")), constQLatin1("SM"),
                LinearConverter<StatuteMilesToMeters>(), 3);
            return sm;
        }

        //! All units
        static const QList<CLengthUnit> &allUnits()
        {
            static const QList<CLengthUnit> u { CLengthUnit::nullUnit(), CLengthUnit::cm(), CLengthUnit::ft(),
                                                CLengthUnit::km(),       CLengthUnit::m(),  CLengthUnit::mi(),
                                                CLengthUnit::SM(),       CLengthUnit::NM() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CLengthUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CLengthUnit>(unitName);
        }
    };

    //! Specialized class for angles (degrees, radian).
    class SWIFT_MISC_EXPORT CAngleUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CAngleUnit>,
        public mixin::DBusOperators<CAngleUnit>,
        public mixin::DataStreamOperators<CAngleUnit>,
        public mixin::Index<CAngleUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        struct RadiansToDegrees
        {
            static double factor() { return 180.0 / M_PI; }
        };
        using DegreesToDegrees = One;

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAngleUnit)

        //!  Default constructor, required for Qt Metasystem
        CAngleUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CAngleUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CAngleUnit defaultUnit() { return deg(); }

        //! Null unit
        static CAngleUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
        virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool withGroupSeparator = false,
                                                   bool i18n = false) const override;

        //! Radians
        static CAngleUnit rad()
        {
            static constexpr CMeasurementUnit::Data rad(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "radian")),
                                                        constQLatin1("rad"), LinearConverter<RadiansToDegrees>());
            return rad;
        }

        //! Degrees
        static CAngleUnit deg()
        {
            static constexpr CMeasurementUnit::Data deg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree")),
                                                        constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "deg")),
                                                        IdentityConverter());
            return deg;
        }

        //! Sexagesimal degree (degrees, minutes, seconds, decimal seconds)
        static CAngleUnit sexagesimalDeg()
        {
            static constexpr CMeasurementUnit::Data deg(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute, second")), constQLatin1("DMS"),
                SubdivisionConverter2<DegreesToDegrees, InEachHundred<60>>(), 4);
            return deg;
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2 %L3");
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2 %L3");
        }

        //! Sexagesimal degree (degrees, minutes, decimal minutes)
        static CAngleUnit sexagesimalDegMin()
        {
            static constexpr CMeasurementUnit::Data deg(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "degree, minute")), constQLatin1("MinDec"),
                SubdivisionConverter<DegreesToDegrees, InEachHundred<60>>(), 4);
            return deg;
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1 %L2");
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1 %L2");
        }

        //! All units
        static const QList<CAngleUnit> &allUnits()
        {
            static const QList<CAngleUnit> u { CAngleUnit::nullUnit(), CAngleUnit::deg(), CAngleUnit::rad(),
                                               CAngleUnit::sexagesimalDeg(), CAngleUnit::sexagesimalDegMin() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CAngleUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CAngleUnit>(unitName);
        }
    };

    //! Specialized class for frequency (hertz, mega hertz, kilo hertz).
    class SWIFT_MISC_EXPORT CFrequencyUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CFrequencyUnit>,
        public mixin::DBusOperators<CFrequencyUnit>,
        public mixin::DataStreamOperators<CFrequencyUnit>,
        public mixin::Index<CFrequencyUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        using HertzToHertz = One;

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CFrequencyUnit)

        //! Default constructor, required for Qt Metasystem
        CFrequencyUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CFrequencyUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CFrequencyUnit defaultUnit() { return Hz(); }

        //! Null unit
        static CFrequencyUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Hertz
        static CFrequencyUnit Hz()
        {
            static constexpr CMeasurementUnit::Data Hz(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hertz")),
                                                       constQLatin1("Hz"), IdentityConverter());
            return Hz;
        }

        //! Kilohertz
        static CFrequencyUnit kHz()
        {
            static constexpr CMeasurementUnit::Data kHz(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilohertz")), constQLatin1("kHz"),
                LinearConverter<Kilo<HertzToHertz>>(), 1);
            return kHz;
        }

        //! Megahertz
        static CFrequencyUnit MHz()
        {
            static constexpr CMeasurementUnit::Data MHz(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "megahertz")), constQLatin1("MHz"),
                LinearConverter<Mega<HertzToHertz>>(), 2);
            return MHz;
        }

        //! Gigahertz
        static CFrequencyUnit GHz()
        {
            static constexpr CMeasurementUnit::Data GHz(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gigahertz")), constQLatin1("GHz"),
                LinearConverter<Giga<HertzToHertz>>(), 2);
            return GHz;
        }

        //! All units
        static const QList<CFrequencyUnit> &allUnits()
        {
            static const QList<CFrequencyUnit> u { CFrequencyUnit::nullUnit(), CFrequencyUnit::GHz(),
                                                   CFrequencyUnit::Hz(), CFrequencyUnit::kHz(), CFrequencyUnit::MHz() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CFrequencyUnit>(unitName);
        }
    };

    //! Specialized class for mass units (kg, lbs).
    class SWIFT_MISC_EXPORT CMassUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CMassUnit>,
        public mixin::DBusOperators<CMassUnit>,
        public mixin::DataStreamOperators<CMassUnit>,
        public mixin::Index<CMassUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        using GramsToKilograms = Milli<One>;
        struct PoundsToKilograms
        {
            static double factor() { return 0.45359237; }
        };

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CMassUnit)

        //! Default constructor, required for Qt Metasystem
        CMassUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CMassUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CMassUnit defaultUnit() { return kg(); }

        //! Null unit
        static CMassUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Kilogram, SI base unit
        static CMassUnit kg()
        {
            static constexpr CMeasurementUnit::Data kg(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilogram")),
                                                       constQLatin1("kg"), IdentityConverter(), 1);
            return kg;
        }

        //! Gram, SI unit
        static CMassUnit g()
        {
            static constexpr CMeasurementUnit::Data g(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "gram")),
                                                      constQLatin1("g"), LinearConverter<GramsToKilograms>(), 0);
            return g;
        }

        //! Tonne, aka metric ton (1000kg)
        static CMassUnit tonne()
        {
            static constexpr CMeasurementUnit::Data t(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "tonne")),
                                                      constQLatin1("t"), LinearConverter<Mega<GramsToKilograms>>(), 3);
            return t;
        }

        //! Short ton (2000lb) used in the United States
        static CMassUnit shortTon()
        {
            static constexpr CMeasurementUnit::Data ton(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "short ton")), constQLatin1("ton"),
                LinearConverter<Two<Kilo<PoundsToKilograms>>>(), 3);
            return ton;
        }

        //! Pound, aka mass pound
        static CMassUnit lb()
        {
            static constexpr CMeasurementUnit::Data lbs(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound")),
                                                        constQLatin1("lb"), LinearConverter<PoundsToKilograms>(), 1);
            return lbs;
        }

        //! All units
        static const QList<CMassUnit> &allUnits()
        {
            static const QList<CMassUnit> u { CMassUnit::nullUnit(), CMassUnit::g(),     CMassUnit::kg(),
                                              CMassUnit::lb(),       CMassUnit::tonne(), CMassUnit::shortTon() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CMassUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CMassUnit>(unitName);
        }
    };

    //! Specialized class for pressure (psi, hPa, bar).
    class SWIFT_MISC_EXPORT CPressureUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CPressureUnit>,
        public mixin::DBusOperators<CPressureUnit>,
        public mixin::DataStreamOperators<CPressureUnit>,
        public mixin::Index<CPressureUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        using PascalsToHectopascals = Centi<One>;
        struct PsiToHectopascals
        {
            static double factor() { return 68.948; }
        };
        struct InchesToHectopascals
        {
            static double factor() { return 33.86389; }
        };
        struct MillimetersToHectopascals
        {
            static double factor() { return 860.142806; }
        };

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CPressureUnit)

        //! Default constructor, required for Qt Metasystem
        CPressureUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CPressureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CPressureUnit defaultUnit() { return hPa(); }

        //! Null unit
        static CPressureUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Pascal
        static CPressureUnit Pa()
        {
            static constexpr CMeasurementUnit::Data Pa(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pascal")),
                                                       constQLatin1("Pa"), LinearConverter<PascalsToHectopascals>());
            return Pa;
        }

        //! Hectopascal
        static CPressureUnit hPa()
        {
            static constexpr CMeasurementUnit::Data hPa(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hectopascal")), constQLatin1("hPa"),
                IdentityConverter());
            return hPa;
        }

        //! Pounds per square inch
        static CPressureUnit psi()
        {
            static constexpr CMeasurementUnit::Data psi(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "pound per square inch")), constQLatin1("psi"),
                LinearConverter<PsiToHectopascals>(), 2);
            return psi;
        }

        //! Bar
        static CPressureUnit bar()
        {
            static constexpr CMeasurementUnit::Data bar(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "bar")),
                                                        constQLatin1("bar"), LinearConverter<Kilo<One>>(), 1);
            return bar;
        }

        //! Millibar, actually the same as hPa
        static CPressureUnit mbar()
        {
            static constexpr CMeasurementUnit::Data mbar(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millibar")), constQLatin1("mbar"),
                IdentityConverter(), 1);
            return mbar;
        }

        //! Inch of mercury at 0°C
        static CPressureUnit inHg()
        {
            static constexpr CMeasurementUnit::Data inHg(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "inch of mercury")), constQLatin1("inHg"),
                LinearConverter<InchesToHectopascals>());
            return inHg;
        }

        //! Millimeter of mercury
        static CPressureUnit mmHg()
        {
            static constexpr CMeasurementUnit::Data mmhg(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millimeter of mercury")), constQLatin1("mmHg"),
                LinearConverter<MillimetersToHectopascals>());
            return mmhg;
        }

        //! All units
        static const QList<CPressureUnit> &allUnits()
        {
            static const QList<CPressureUnit> u { CPressureUnit::nullUnit(), CPressureUnit::bar(),
                                                  CPressureUnit::hPa(),      CPressureUnit::inHg(),
                                                  CPressureUnit::mmHg(),     CPressureUnit::mbar(),
                                                  CPressureUnit::psi() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CPressureUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CPressureUnit>(unitName);
        }
    };

    //! Specialized class for temperatur units (kelvin, centidegree).
    class SWIFT_MISC_EXPORT CTemperatureUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CTemperatureUnit>,
        public mixin::DBusOperators<CTemperatureUnit>,
        public mixin::DataStreamOperators<CTemperatureUnit>,
        public mixin::Index<CTemperatureUnit>
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

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CTemperatureUnit)

        //! Default constructor, required for Qt Metasystem
        CTemperatureUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CTemperatureUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CTemperatureUnit defaultUnit() { return C(); }

        //! Null unit
        static CTemperatureUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Kelvin
        static CTemperatureUnit K()
        {
            static constexpr CMeasurementUnit::Data K(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Kelvin")),
                                                      constQLatin1("K"), AffineConverter<KelvinToCentigrade>());
            return K;
        }

        //! Centigrade C
        static CTemperatureUnit C()
        {
            static constexpr CMeasurementUnit::Data C(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "centigrade")),
                                                      constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "C")),
                                                      IdentityConverter());
            return C;
        }

        //! Fahrenheit F
        static CTemperatureUnit F()
        {
            static constexpr CMeasurementUnit::Data F(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "Fahrenheit")),
                                                      constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "F")),
                                                      AffineConverter<FahrenheitToCentigrade>());
            return F;
        }

        //! All units
        static const QList<CTemperatureUnit> &allUnits()
        {
            static const QList<CTemperatureUnit> u { CTemperatureUnit::nullUnit(), CTemperatureUnit::C(),
                                                     CTemperatureUnit::F(), CTemperatureUnit::K() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CTemperatureUnit>(unitName);
        }
    };

    //! Specialized class for speed units (m/s, ft/s, NM/h).
    class SWIFT_MISC_EXPORT CSpeedUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CSpeedUnit>,
        public mixin::DBusOperators<CSpeedUnit>,
        public mixin::DataStreamOperators<CSpeedUnit>,
        public mixin::Index<CSpeedUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        struct KnotsToMps
        {
            static double factor() { return 1852.0 / 3600.0; }
        };
        struct KphToMps
        {
            static double factor() { return 1.0 / 3.6; }
        };
        struct FtPerSecToMps
        {
            static double factor() { return 0.3048; }
        };
        struct FtPerMinToMps
        {
            static double factor() { return 0.3048 / 60.0; }
        };

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CSpeedUnit)

        //! Default constructor, required for Qt Metasystem
        CSpeedUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CSpeedUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CSpeedUnit defaultUnit() { return m_s(); }

        //! Null unit
        static CSpeedUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Meter/second m/s
        static CSpeedUnit m_s()
        {
            static constexpr CMeasurementUnit::Data ms(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second")), constQLatin1("m/s"),
                IdentityConverter());
            return ms;
        }

        //! Knots
        static CSpeedUnit kts()
        {
            static constexpr CMeasurementUnit::Data kts(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "knot")),
                                                        constQLatin1("kts"), LinearConverter<KnotsToMps>(), 1);
            return kts;
        }

        //! Nautical miles per hour NM/h (same as kts)
        static CSpeedUnit NM_h()
        {
            static constexpr CMeasurementUnit::Data NMh(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "nautical mile per hour")), constQLatin1("NM/h"),
                LinearConverter<KnotsToMps>(), 1);
            return NMh;
        }

        //! Feet/second ft/s
        static CSpeedUnit ft_s()
        {
            static constexpr CMeasurementUnit::Data fts(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second")), constQLatin1("ft/s"),
                LinearConverter<FtPerSecToMps>(), 0);
            return fts;
        }

        //! Feet/min ft/min
        static CSpeedUnit ft_min()
        {
            static constexpr CMeasurementUnit::Data ftmin(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per minute")), constQLatin1("ft/min"),
                LinearConverter<FtPerMinToMps>(), 0);
            return ftmin;
        }

        //! Kilometer/hour km/h
        static CSpeedUnit km_h()
        {
            static constexpr CMeasurementUnit::Data kmh(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "kilometer per hour")), constQLatin1("km/h"),
                LinearConverter<KphToMps>(), 1);
            return kmh;
        }

        //! All units
        static const QList<CSpeedUnit> &allUnits()
        {
            static const QList<CSpeedUnit> u { CSpeedUnit::nullUnit(), CSpeedUnit::ft_min(), CSpeedUnit::ft_s(),
                                               CSpeedUnit::km_h(),     CSpeedUnit::kts(),    CSpeedUnit::m_s(),
                                               CSpeedUnit::NM_h() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CSpeedUnit>(unitName);
        }
    };

    //! Specialized class for time units (ms, hour, min).
    class SWIFT_MISC_EXPORT CTimeUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CTimeUnit>,
        public mixin::DBusOperators<CTimeUnit>,
        public mixin::DataStreamOperators<CTimeUnit>,
        public mixin::Index<CTimeUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        using SecondsToSeconds = One;
        struct DaysToSeconds
        {
            static double factor() { return 60.0 * 60.0 * 24.0; }
        };
        struct HoursToSeconds
        {
            static double factor() { return 60.0 * 60.0; }
        };
        struct MinutesToSeconds
        {
            static double factor() { return 60.0; }
        };

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CTimeUnit)

        //! Default constructor, required for Qt Metasystem
        CTimeUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CTimeUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CTimeUnit defaultUnit() { return s(); }

        //! Null unit
        static CTimeUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! \copydoc CMeasurementUnit::makeRoundedQStringWithUnit
        virtual QString makeRoundedQStringWithUnit(double value, int digits = -1, bool withGroupSeparator = false,
                                                   bool i18n = false) const override;

        //! Second s
        static CTimeUnit s()
        {
            static constexpr CMeasurementUnit::Data s(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "second")),
                                                      constQLatin1("s"), IdentityConverter(), 1);
            return s;
        }

        //! Millisecond ms
        static CTimeUnit ms()
        {
            static constexpr CMeasurementUnit::Data ms(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "millisecond")), constQLatin1("ms"),
                LinearConverter<Milli<SecondsToSeconds>>(), 0);
            return ms;
        }

        //! Hour
        static CTimeUnit h()
        {
            static constexpr CMeasurementUnit::Data h(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour")),
                                                      constQLatin1("h"), LinearConverter<HoursToSeconds>(), 1);
            return h;
        }

        //! Minute
        static CTimeUnit min()
        {
            static constexpr CMeasurementUnit::Data minute(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute")), constQLatin1("min"),
                LinearConverter<MinutesToSeconds>(), 2);
            return minute;
        }

        //! Day
        static CTimeUnit d()
        {
            static constexpr CMeasurementUnit::Data day(constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "day")),
                                                        constQLatin1("d"), LinearConverter<DaysToSeconds>(), 1);
            return day;
        }

        //! Hours, minutes, seconds
        static CTimeUnit hms()
        {
            static constexpr CMeasurementUnit::Data hms(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute, second")), constQLatin1("hms"),
                SubdivisionConverter2<HoursToSeconds, InEachHundred<60>>(), 4);
            return hms;
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m%L3s");
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%-L1h%L2m%L3s");
        }

        //! Hours, minutes
        static CTimeUnit hrmin()
        {
            static constexpr CMeasurementUnit::Data hrmin(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "hour, minute")), constQLatin1("hm"),
                SubdivisionConverter<HoursToSeconds, InEachHundred<60>>(), 3);
            return hrmin;
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1h%L2m");
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1h%L2m");
        }

        //! Minutes, seconds
        static CTimeUnit minsec()
        {
            static constexpr CMeasurementUnit::Data minsec(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "minute, second")), constQLatin1("minsec"),
                SubdivisionConverter<MinutesToSeconds, InEachHundred<60>>(), 2);
            return minsec;
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "%L1m%L2s");
            (void)QT_TRANSLATE_NOOP("CMeasurementUnit", "-%L1m%L2s");
        }

        //! All units
        static const QList<CTimeUnit> &allUnits()
        {
            static const QList<CTimeUnit> u { CTimeUnit::nullUnit(), CTimeUnit::d(),     CTimeUnit::h(),
                                              CTimeUnit::hms(),      CTimeUnit::hrmin(), CTimeUnit::min(),
                                              CTimeUnit::ms(),       CTimeUnit::s() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CTimeUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CTimeUnit>(unitName);
        }
    };

    //! Specialized class for acceleration units (m/s2, ft/s2).
    class SWIFT_MISC_EXPORT CAccelerationUnit final :
        public CMeasurementUnit,
        public mixin::MetaType<CAccelerationUnit>,
        public mixin::DBusOperators<CAccelerationUnit>,
        public mixin::DataStreamOperators<CAccelerationUnit>,
        public mixin::Index<CAccelerationUnit>
    {
    private:
        using CMeasurementUnit::CMeasurementUnit;

        struct FeetToMeters
        {
            static double factor() { return 0.3048; }
        };

        virtual void anchor();

    public:
        //! Base type
        using base_type = CMeasurementUnit;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAccelerationUnit)

        //! Default constructor, required for Qt Metasystem
        CAccelerationUnit() : CMeasurementUnit(defaultUnit()) {}

        //! Null constructor
        CAccelerationUnit(std::nullptr_t) : CMeasurementUnit(nullUnit()) {}

        //! Default unit
        static CAccelerationUnit defaultUnit() { return m_s2(); }

        //! Null unit
        static CAccelerationUnit nullUnit()
        {
            static constexpr CMeasurementUnit::Data nu(constQLatin1("null"), constQLatin1("null"));
            return nu;
        }

        //! Meter/second^2 (m/s^2)
        static CAccelerationUnit m_s2()
        {
            static constexpr CMeasurementUnit::Data ms2(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "meter per second per second")),
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "m/s^2")), IdentityConverter(), 1);
            return ms2;
        }

        //! Feet/second^2
        static CAccelerationUnit ft_s2()
        {
            static constexpr CMeasurementUnit::Data fts2(
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "foot per second per second")),
                constQLatin1(QT_TRANSLATE_NOOP("CMeasurementUnit", "ft/s^2")), LinearConverter<FeetToMeters>(), 0);
            return fts2;
        }

        //! All units
        static const QList<CAccelerationUnit> &allUnits()
        {
            static const QList<CAccelerationUnit> u { CAccelerationUnit::nullUnit(), CAccelerationUnit::ft_s2(),
                                                      CAccelerationUnit::m_s2() };
            return u;
        }

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        void unmarshallFromDbus(const QDBusArgument &argument)
        {
            QString unitName;
            argument >> unitName;
            (*this) = CMeasurementUnit::unitFromSymbol<CAccelerationUnit>(unitName);
        }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream)
        {
            QString unitName;
            stream >> unitName;
            *this = CMeasurementUnit::unitFromSymbol<CAccelerationUnit>(unitName);
        }
    };

} // namespace swift::misc::physical_quantities

Q_DECLARE_METATYPE(swift::misc::physical_quantities::CLengthUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CAngleUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CFrequencyUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CMassUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CPressureUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CTemperatureUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CSpeedUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CTimeUnit)
Q_DECLARE_METATYPE(swift::misc::physical_quantities::CAccelerationUnit)

#pragma pop_macro("min")

#endif // guard
