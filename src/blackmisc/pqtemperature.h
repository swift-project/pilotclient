#ifndef CTEMPERATURE_H
#define CTEMPERATURE_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for temperatur units (kelvin, centidegree).
 * \author KWB
 */
class CTemperatureUnit : public CMeasurementUnit {
public:
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
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CTemperatureUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter m
     * \return
     */
    static CTemperatureUnit& K() { static CTemperatureUnit K("Kelvin", "K", true, true); return K;}
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static CTemperatureUnit& C() { static CTemperatureUnit C("centigrade", "°C", false, false);return C;}
};

/*!
 * \brief Physical unit temperature
 * \author KWB
 */
class CTemperature : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CTemperature();
    /**
     *\brief downcast copy constructor
     */
    CTemperature(const CPhysicalQuantity &temperature);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CTemperature(qint32 value, const CTemperatureUnit &unit = CTemperatureUnit::K());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CTemperature(double value, const CTemperatureUnit &unit = CTemperatureUnit::K());
    /*!
     * \brief Unit of the temperature
     * \return
     */
    CTemperatureUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CTemperatureUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
};
} // namespace

#endif // CTEMPERATURE_H
