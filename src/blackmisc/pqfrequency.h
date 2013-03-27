#ifndef PQFREQUENCY_H
#define PQFREQUENCY_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CFrequency : public CPhysicalQuantity<CFrequencyUnit,CFrequency>
{
public:
    /*!
     * \brief Default constructor
     */
    CFrequency() : CPhysicalQuantity(0, CFrequencyUnit::Hz(), CFrequencyUnit::Hz()) {}
    /**
     *\brief Copy constructor
     */
    CFrequency(const CFrequency &frequency) : CPhysicalQuantity(frequency) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CFrequency(qint32 value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit, CFrequencyUnit::Hz()){}
    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CFrequency(double value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit, CFrequencyUnit::Hz()){}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CFrequency() {}
};
} // namespace
#endif // PQFREQUENCY_H
