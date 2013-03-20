#ifndef PQFREQUENCY_H
#define PQFREQUENCY_H
#include "pqphysicalquantity.h"

namespace BlackCore {

/*!
 * Specialized class for frequency (hertz, mega hertz, kilo hertz).
 * \author KWB
 */
class CFrequencyUnit : public CMeasurementUnit {
public:
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
    /*!
     * Downcast copy constructor, allows to implement methods in base class
     * \param otherUnit
     */
    CFrequencyUnit(const CMeasurementUnit &otherUnit) : CMeasurementUnit(otherUnit) {}
    /*!
     * \brief Hertz
     * \return
     */
    static CFrequencyUnit& Hz() { static CFrequencyUnit Hz("hertz", "Hz", true); return Hz;}
    /*!
     * \brief Kilohertz
     * \return
     */
    static CFrequencyUnit& kHz() { static CFrequencyUnit kHz("kilohertz", "kHz", true, CMeasurementPrefix::k().getFactor(), CMeasurementPrefix::k(), 0);return kHz;}
    /*!
     * \brief Megahertz
     * \return
     */
    static CFrequencyUnit& MHz() { static CFrequencyUnit MHz("megahertz", "MHz", false, CMeasurementPrefix::M().getFactor(), CMeasurementPrefix::M(), 0); return MHz;}
    /*!
     * \brief Gigahertz
     * \return
     */
    static CFrequencyUnit& GHz() { static CFrequencyUnit GHz("gigahertz", "GHz", true, CMeasurementPrefix::G().getFactor(), CMeasurementPrefix::G(), 0);return GHz;}
};

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CFrequency : public CPhysicalQuantity
{
public:
    /*!
     * \brief Default constructor
     */
    CFrequency();
    /**
     *\brief downcast copy constructor
     */
    CFrequency(const CPhysicalQuantity &frequency);
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CFrequency(qint32 value, const CFrequencyUnit &unit = CFrequencyUnit::Hz());
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CFrequency(double value, const CFrequencyUnit &unit = CFrequencyUnit::Hz());
    /*!
     * \brief Unit of the distance
     * \return
     */
    CFrequencyUnit getUnit() const { return this->_unit; }
    /*!
     * \brief Conversion SI unit
     * \return
     */
    CFrequencyUnit getConversionSiUnit() const { return this->_conversionSiUnit; }
};
} // namespace blackCore
#endif // PQFREQUENCY_H
