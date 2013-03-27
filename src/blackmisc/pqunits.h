#ifndef PQUNITS_H
#define PQUNITS_H
#include "blackmisc/pqbase.h"

namespace BlackMisc {

/*!
 * Specialized class for distance units (meter, foot, nautical miles).
 * \author KWB
 */
class CLengthUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief Distance unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CLengthUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "distance", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon)
    {
        // void
    }
public:
    CLengthUnit(const CLengthUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Meter m
     * \return
     */
    static const CLengthUnit& m() { static CLengthUnit m("meter", "m", true, true); return m;}
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static const CLengthUnit& NM() { static CLengthUnit NM("nautical miles", "NM", false, false, 1000.0*1.85200, CMeasurementPrefix::One(), 3);return NM;}
    /*!
     * \brief Foot ft
     * \return
     */
    static const CLengthUnit& ft() { static CLengthUnit ft("foot", "ft", false, false, 0.3048, CMeasurementPrefix::One(), 0); return ft;}
    /*!
     * \brief Kilometer km
     * \return
     */
    static const CLengthUnit& km() { static CLengthUnit km("kilometer", "km", true, false, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 3);return km;}
    /*!
     * \brief Centimeter cm
     * \return
     */
    static const CLengthUnit& cm() { static CLengthUnit cm("centimeter", "cm", true, false, CMeasurementPrefix::c().getFactor(), CMeasurementPrefix::c(), 1);return cm;}
};

/*!
 * Specialized class for angles (degrees, radian).
 * \author KWB
 */
class CAngleUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief Angle units: Radian, degree
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CAngleUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "angle", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon)
    {
        // void
    }
public:
    CAngleUnit(const CAngleUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Meter m
     * \return
     */
    static const CAngleUnit& rad() { static CAngleUnit rad("radian", "rad", true); return rad;}
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static const CAngleUnit& deg() { static CAngleUnit deg("degree", "°", false, M_PI/180); return deg;}
};

/*!
 * Specialized class for frequency (hertz, mega hertz, kilo hertz).
 * \author KWB
 */
class CFrequencyUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief CFrequencyUnit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CFrequencyUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "frequency", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    CFrequencyUnit(const CFrequencyUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Hertz
     * \return
     */
    static const CFrequencyUnit& Hz() { static CFrequencyUnit Hz("hertz", "Hz", true); return Hz;}
    /*!
     * \brief Kilohertz
     * \return
     */
    static const CFrequencyUnit& kHz() { static CFrequencyUnit kHz("kilohertz", "kHz", true, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 0);return kHz;}
    /*!
     * \brief Megahertz
     * \return
     */
    static const CFrequencyUnit& MHz() { static CFrequencyUnit MHz("megahertz", "MHz", false, CMeasurementPrefix::M().getFactor(), CMeasurementPrefix::M(), 0); return MHz;}
    /*!
     * \brief Gigahertz
     * \return
     */
    static const CFrequencyUnit& GHz() { static CFrequencyUnit GHz("gigahertz", "GHz", true, CMeasurementPrefix::G().getFactor(), CMeasurementPrefix::G(), 0);return GHz;}
};

/*!
 * Specialized class for mass units (kg, lbs).
 * \author KWB
 */
class CMassUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief Mass units
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CMassUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "mass", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    CMassUnit(const CMassUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Kilogram, SI base unit
     * \return
     */
    static const CMassUnit& kg() { static CMassUnit kg("kilogram", "kg", true, true, 1.0, CMeasurementPrefix::k(), 1); return kg;}
    /*!
     * \brief Gram, SI unit
     * \return
     */
    static const CMassUnit& g() { static CMassUnit g("gram", "g", true, false, 1.0/1000.0, CMeasurementPrefix::One(), 0); return g;}
    /*!
     * \brief Tonne, aka metric tonne (1000kg)
     * \return
     */
    static const CMassUnit& t() { static CMassUnit t("tonne", "t", true, false, 1000.0, CMeasurementPrefix::One(), 3); return t;}
    /*!
     * \brief Pound, aka mass pound
     * \return
     */
    static const CMassUnit& lb() { static CMassUnit lbs("pound", "lb", false, false, 0.45359237, CMeasurementPrefix::One(), 1); return lbs;}
};

/*!
 * Specialized class for pressure (psi, hPa, bar).
 * \author KWB
 */
class CPressureUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief Pressure unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CPressureUnit(const QString &name, const QString &unitName, bool isSIUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "frequency", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    CPressureUnit(const CPressureUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Pascal
     * \return
     */
    static const CPressureUnit& Pa() { static CPressureUnit Pa("pascal", "Pa", true); return Pa;}
    /*!
     * \brief Hectopascal
     * \return
     */
    static const CPressureUnit& hPa() { static CPressureUnit hPa("hectopascal", "hPa", true, CMeasurementPrefix::h().getFactor(), CMeasurementPrefix::h()); return hPa;}
    /*!
     * \brief Pounds per square inch
     * \return
     */
    static const CPressureUnit& psi() { static CPressureUnit psi("pounds per square inch", "psi", false, 6894.8, CMeasurementPrefix::One(), 2); return psi;}
    /*!
     * \brief Bar
     * \return
     */
    static const CPressureUnit& bar() { static CPressureUnit bar("bar", "bar", false, 1E5);return bar;}
    /*!
     * \brief Millibar, actually the same as hPa
     * \return
     */
    static const CPressureUnit& mbar() { static CPressureUnit bar("bar", "bar", false, 1E2);return bar;}
    /*!
     * \brief Inch of mercury at 0°C
     * \return
     */
    static const CPressureUnit& inHg() { static CPressureUnit inhg("Inch of mercury 0°C", "inHg", false, 3386.389);return inhg;}
    /*!
     * \brief Inch of mercury for flight level 29,92inHg = 1013,25mbar = 1013,25hPa
     * \return
     */
    static const CPressureUnit& inHgFL() { static CPressureUnit inhg("Inch of mercury ", "inHg", false, 3386.5307486631);return inhg;}
};

/*!
 * Specialized class for temperatur units (kelvin, centidegree).
 * \author KWB
 */
class CTemperatureUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief Temperature unit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CTemperatureUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "temperature", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * \brief Copy constructor
     * \param otherUnit
     */
    CTemperatureUnit(const CTemperatureUnit &otherUnit) : CMeasurementUnit(otherUnit)
    {
        // void
    }
    /*!
     * \brief Kelvin
     * \return
     */
    static const CTemperatureUnit& K() { static CTemperatureUnit K("Kelvin", "K", true, true); return K;}
    /*!
     * \brief Centigrade C
     * \return
     */
    static const CTemperatureUnit& C() { static CTemperatureUnit C("centigrade", "°C", false, false);return C;}
    /*!
     * \brief Fahrenheit F
     * \return
     */
    static const CTemperatureUnit& F() { static CTemperatureUnit F("Fahrenheit", "°F", false, false, 5.0/9.0);return F;}
};

/*!
 * Specialized class for speed units (m/s, ft/s, NM/h).
 * \author KWB
 */
class CSpeedUnit : public CMeasurementUnit {
private:
    /*!
     * Constructor
     * \brief CSpeedUnit
     * \param name
     * \param unitName
     * \param isSIUnit
     * \param isSIBaseUnit
     * \param conversionFactorToSI
     * \param mulitplier
     * \param displayDigits
     * \param epsilon
     */
    CSpeedUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "speed", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
public:
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CSpeedUnit(const CSpeedUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter/second m/s
     * \return
     */
    static const CSpeedUnit& m_s() { static CSpeedUnit ms("meter/second", "m/s", true, false); return ms;}
    /*!
     * \brief Nautical miles per hour NM/h
     * \return
     */
    static const CSpeedUnit& NM_h() { static CSpeedUnit NMh("nautical miles/hour", "NM/h", false, false, 1852.0/3600.0, CMeasurementPrefix::One(), 1);return NMh;}
    /*!
     * \brief Feet/second ft/s
     * \return
     */
    static const CSpeedUnit& ft_s() { static CSpeedUnit fts("feet/seconds", "ft/s", false, false, 0.3048, CMeasurementPrefix::One(), 0); return fts;}
    /*!
     * \brief Feet/min ft/min
     * \return
     */
    static const CSpeedUnit& ft_min() { static CSpeedUnit ftmin("feet/minute", "ft/min", false, false, 0.3048 / 60.0, CMeasurementPrefix::One(), 0); return ftmin;}
    /*!
     * \brief Kilometer/hour km/h
     * \return
     */
    static const CSpeedUnit& km_h() { static CSpeedUnit kmh("kilometer/hour", "km/h", false, false, 1.0/3.6, CMeasurementPrefix::One(), 1);return kmh;}
};


} // namespace
#endif // PQUNITS_H
