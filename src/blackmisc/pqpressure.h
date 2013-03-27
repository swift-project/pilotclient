#ifndef PQPRESSURE_H
#define PQPRESSURE_H

#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CPressure : public CPhysicalQuantity<CPressureUnit,CPressure>
{
public:
    /*!
     * \brief Default constructor
     */
    CPressure() : CPhysicalQuantity(0, CPressureUnit::Pa(), CPressureUnit::Pa()){}
    /**
     *\brief Copy constructor
     */
    CPressure(const CPressure &pressure) : CPhysicalQuantity(pressure) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CPressure(qint32 value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa()){}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit, CPressureUnit::Pa()){}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CPressure() {}
};
} // namespace
#endif // PQPRESSURE_H
