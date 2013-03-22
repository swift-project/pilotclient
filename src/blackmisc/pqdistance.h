#ifndef PQDISTANCE_H
#define PQDISTANCE_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for distance units (meter, foot, nautical miles).
 * \author KWB
 */
class CDistanceUnit : public CMeasurementUnit {
public:
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
    CDistanceUnit(const QString &name, const QString &unitName, bool isSIUnit, bool isSIBaseUnit, double conversionFactorToSI = 1.0, const CMeasurementPrefix &mulitplier = CMeasurementPrefix::One(), qint32 displayDigits = 2, double epsilon = 1E-9) :
        CMeasurementUnit(name, unitName, "distance", isSIUnit, isSIBaseUnit, conversionFactorToSI, mulitplier, displayDigits, epsilon) {}
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CDistanceUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Meter m
     * \return
     */
    static CDistanceUnit& m() { static CDistanceUnit m("meter", "m", true, true); return m;}
    /*!
     * \brief Nautical miles NM
     * \return
     */
    static CDistanceUnit& NM() { static CDistanceUnit NM("nautical miles", "NM", false, false, 1000.0*1.85200, CMeasurementPrefix::One(), 3);return NM;}
    /*!
     * \brief Foot ft
     * \return
     */
    static CDistanceUnit& ft() { static CDistanceUnit ft("foot", "ft", false, false, 0.3048, CMeasurementPrefix::One(), 0); return ft;}
    /*!
     * \brief Kilometer km
     * \return
     */
    static CDistanceUnit& km() { static CDistanceUnit km("kilometer", "km", true, false, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 3);return km;}
    /*!
     * \brief Centimeter cm
     * \return
     */
    static CDistanceUnit& cm() { static CDistanceUnit cm("centimeter", "cm", true, false, CMeasurementPrefix::c().getFactor(), CMeasurementPrefix::c(), 1);return cm;}
};

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CDistance : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CDistance();
    /**
     *\brief downcast copy constructor
     */
    CDistance(const CPhysicalQuantity &distance);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CDistance(qint32 value, const CDistanceUnit &unit = CDistanceUnit::m());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CDistance(double value, const CDistanceUnit &unit = CDistanceUnit::m());
    /*!
     * \brief Unit of the distance
     * \return
     */
    CDistanceUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CDistanceUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
};
} // namespace blackCore
#endif // PQDISTANCE_H
