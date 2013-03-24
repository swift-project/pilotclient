#ifndef PQMASS_H
#define PQMASS_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Specialized class for mass units (kg, lbs).
 * \author KWB
 */
class CMassUnit : public CMeasurementUnit {
    friend class CMass;
private:
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CMassUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
public:
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
 * \brief Mass
 * \author KWB
 */
class CMass : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CMass();
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CMass(qint32 value, const CMassUnit &unit = CMassUnit::kg());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CMass(double value, const CMassUnit &unit = CMassUnit::kg());
    /*!
     * \brief Copyconstructor
     * \param mass
     */
    CMass(const CPhysicalQuantity &mass);
    /*!
     * \brief Virtual destructor
     */
    virtual ~CMass();
    /*!
     * \brief Unit of the mass
     * \return
     */
    CMassUnit getUnit() const { return this->_pUnit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CMassUnit getConversionSiUnit() const { return this->_pConversionSiUnit; }
};
} // namespace blackCore

#endif // PQMASS_H
