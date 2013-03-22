#ifndef PQPRESSURE_H
#define PQPRESSURE_H

#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for pressure (psi, hPa, bar).
 * \author KWB
 */
class CPressureUnit : public CMeasurementUnit {
public:
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
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CPressureUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Pascal
     * \return
     */
    static CPressureUnit& Pa() { static CPressureUnit Pa("pascal", "Pa", true); return Pa;}
    /*!
     * \brief Hectopascal
     * \return
     */
    static CPressureUnit& hPa() { static CPressureUnit hPa("hectopascal", "hPa", true, CMeasurementPrefix::h().getFactor(), CMeasurementPrefix::h()); return hPa;}
    /*!
     * \brief Pounds per square inch
     * \return
     */
    static CPressureUnit& psi() { static CPressureUnit psi("pounds per square inch", "psi", false, 6894.8, CMeasurementPrefix::One(), 2); return psi;}
    /*!
     * \brief Bar
     * \return
     */
    static CPressureUnit& bar() { static CPressureUnit bar("bar", "bar", false, 1E5);return bar;}
    /*!
     * \brief Millibar, actually the same as hPa
     * \return
     */
    static CPressureUnit& mbar() { static CPressureUnit bar("bar", "bar", false, 1E2);return bar;}
    /*!
     * \brief Inch of mercury at 0°C
     * \return
     */
    static CPressureUnit& inHg() { static CPressureUnit inhg("Inch of mercury 0°C", "inHg", false, 3386.389);return inhg;}
    /*!
     * \brief Inch of mercury for flight level 29,92inHg = 1013,25mbar = 1013,25hPa
     * \return
     */
    static CPressureUnit& inHgFL() { static CPressureUnit inhg("Inch of mercury ", "inHg", false, 3386.5307486631);return inhg;}
};

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CPressure : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CPressure();
    /**
     *\brief downcast copy constructor
     */
    CPressure(const CPhysicalQuantity &frequency);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CPressure(qint32 value, const CPressureUnit &unit = CPressureUnit::Pa());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CPressure(double value, const CPressureUnit &unit = CPressureUnit::Pa());
    /*!
     * \brief Unit of the distance
     * \return
     */
    CPressureUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CPressureUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
};
} // namespace blackCore
#endif // PQPRESSURE_H
