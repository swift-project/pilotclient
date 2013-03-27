#ifndef PQDISTANCE_H
#define PQDISTANCE_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit distance
 * \author KWB
 */
class CDistance : public CPhysicalQuantity<CDistanceUnit,CDistance>
{
public:
    /*!
     * \brief Default constructor
     */
    CDistance() : CPhysicalQuantity (0, CDistanceUnit::m(), CDistanceUnit::m()) {}
    /**
     *\brief Copy constructor
     */
    CDistance(const CDistance &distance) : CPhysicalQuantity(distance) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CDistance(qint32 value, const CDistanceUnit &unit) : CPhysicalQuantity(value, unit, CDistanceUnit::m()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CDistance(double value, const CDistanceUnit &unit) : CPhysicalQuantity(value, unit, CDistanceUnit::m()) {}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CDistance() {}
};
} // namespace
#endif // PQDISTANCE_H
