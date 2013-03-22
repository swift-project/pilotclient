#ifndef PQANGLE_H
#define PQANGLE_H
#include "pqphysicalquantity.h"
#include "math.h"

namespace BlackMisc {

/*!
 * Specialized class for angles (degrees, radian).
 * \author KWB
 */
class CAngleUnit : public CMeasurementUnit {
public:
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
        CMeasurementUnit(name, unitName, "angle", isSIUnit, false, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CAngleUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter m
     * \return
     */
    static CAngleUnit& rad() { static CAngleUnit rad("radian", "rad", true); return rad;}
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static CAngleUnit& deg() { static CAngleUnit deg("degree", "°", false, M_PI/180); return deg;}
};

/*!
 * \brief Physical unit degree
 * \author KWB
 */
class CAngle : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CAngle();
    /**
     *\brief downcast copy constructor
     */
    CAngle(const CPhysicalQuantity &angle);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CAngle(qint32 value, const CAngleUnit &unit = CAngleUnit::rad());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CAngle(double value, const CAngleUnit &unit = CAngleUnit::rad());
    /*!
     * \brief Unit of the distance
     * \return
     */
    CAngleUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CAngleUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
    /*!
     * \brief Convenience method PI
     * \return
     */
    const static double pi() { return M_PI;}
    /*!
     * \brief Value as factor of PI (e.g.0.5PI)
     * \return
     */
    double piFactor() const { return CPhysicalQuantity::round(this->convertedSiValueToDouble() / M_PI,6);}

};
} // namespace blackCore

#endif // PQANGLE_H
