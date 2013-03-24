#ifndef CTEMPERATURE_H
#define CTEMPERATURE_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for temperatur units (kelvin, centidegree).
 * \author KWB
 */
class CTemperatureUnit : public CMeasurementUnit {
    friend class CTemperature;
private:
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CTemperatureUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
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
 * \brief Physical unit temperature
 * \author KWB
 */
class CTemperature : public CPhysicalQuantity
{
private:
    /*!
     * \brief Convert into another temperature unit
     * \param quantity
     * \param otherUnit
     * \return
     */
    static double temperaturUnitConverter(const CPhysicalQuantity *quantity, const CMeasurementUnit &otherUnit);
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
     * \brief Destructor
     */
    virtual ~CTemperature();
    /*!
     * \brief Unit of the temperature
     * \return
     */
    CTemperatureUnit getUnit() const { return this->_pUnit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CTemperatureUnit getConversionSiUnit() const { return this->_pConversionSiUnit; }
};
} // namespace

#endif // CTEMPERATURE_H
