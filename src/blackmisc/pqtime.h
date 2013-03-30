#ifndef PQTIME_H
#define PQTIME_H

#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Time class, e.g. "ms", "hour", "s", "day"
 * \author KWB
 */
class CTime : public CPhysicalQuantity<CTimeUnit, CTime>
{
public:
    /*!
     * \brief Default constructor
     */
    CTime() : CPhysicalQuantity(0, CTimeUnit::s(), CTimeUnit::s()) {}
    /**
     *\brief Copy constructor
     */
    CTime(const CPhysicalQuantity &time): CPhysicalQuantity(time) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CTime(qint32 value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit, CTimeUnit::s()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CTime(double value, const CTimeUnit &unit) : CPhysicalQuantity(value, unit, CTimeUnit::s()) {}
    /*!
     * \brief Destructor
     */
    virtual ~CTime() {}
};
} // namespace


#endif // PQTIME_H
