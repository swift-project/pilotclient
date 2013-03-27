#ifndef CSPEED_H
#define CSPEED_H
#include "pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * Speed class, e.g. "m/s", "NM/h", "km/h", "ft/s"
 * \author KWB
 */
class CSpeed : public CPhysicalQuantity<CSpeedUnit, CSpeed>
{
public:
    /*!
     * \brief Default constructor
     */
    CSpeed() : CPhysicalQuantity(0, CSpeedUnit::m_s(), CSpeedUnit::m_s()) {}
    /**
     *\brief Copy constructor
     */
    CSpeed(const CPhysicalQuantity &speed): CPhysicalQuantity(speed) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CSpeed(qint32 value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit, CSpeedUnit::m_s()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CSpeed(double value, const CSpeedUnit &unit) : CPhysicalQuantity(value, unit, CSpeedUnit::m_s()) {}
    /*!
     * \brief Destructor
     */
    virtual ~CSpeed() {}
};
} // namespace

#endif // CSPEED_H
