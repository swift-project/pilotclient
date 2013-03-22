#ifndef CSPEED_H
#define CSPEED_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for speed units (m/s, ft/s, NM/h).
 * \author KWB
 */
class CSpeedUnit : public CMeasurementUnit {
public:
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
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CSpeedUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter/second m/s
     * \return
     */
    static CSpeedUnit& m_s() { static CSpeedUnit ms("meter/second", "m/s", true, false); return ms;}
    /*!
     * \brief Nautical miles per hour NM/h
     * \return
     */
    static CSpeedUnit& NM_h() { static CSpeedUnit NMh("nautical miles/hour", "NM/h", false, false, 1852.0/3600.0, CMeasurementPrefix::One(), 1);return NMh;}
    /*!
     * \brief Feet/second ft/s
     * \return
     */
    static CSpeedUnit& ft_s() { static CSpeedUnit fts("feet/seconds", "ft/s", false, false, 0.3048, CMeasurementPrefix::One(), 0); return fts;}
    /*!
     * \brief Feet/min ft/min
     * \return
     */
    static CSpeedUnit& ft_min() { static CSpeedUnit ftmin("feet/minute", "ft/min", false, false, 0.3048 / 60.0, CMeasurementPrefix::One(), 0); return ftmin;}
    /*!
     * \brief Kilometer/hour km/h
     * \return
     */
    static CSpeedUnit& km_h() { static CSpeedUnit kmh("kilometer/hour", "km/h", false, false, 1.0/3.6, CMeasurementPrefix::One(), 1);return kmh;}
};

/*!
 * Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
 * \author KWB
 */
class CSpeed : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CSpeed();
    /**
     *\brief downcast copy constructor
     */
    CSpeed(const CPhysicalQuantity &speed);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CSpeed(qint32 value, const CSpeedUnit &unit = CSpeedUnit::m_s());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CSpeed(double value, const CSpeedUnit &unit = CSpeedUnit::m_s());
    /*!
     * \brief Unit of the distance
     * \return
     */
    CSpeedUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CSpeedUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
};
} // namespace

#endif // CSPEED_H
