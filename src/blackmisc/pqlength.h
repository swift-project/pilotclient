#ifndef PQlength_H
#define PQlength_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit length (length)
 * \author KWB
 */
class CLength : public CPhysicalQuantity<CLengthUnit,CLength>
{
public:
    /*!
     * \brief Default constructor
     */
    CLength() : CPhysicalQuantity (0, CLengthUnit::m(), CLengthUnit::m()) {}
    /**
     *\brief Copy constructor
     */
    CLength(const CLength &length) : CPhysicalQuantity(length) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CLength(qint32 value, const CLengthUnit &unit) : CPhysicalQuantity(value, unit, CLengthUnit::m()) {}
    /*!
     *\brief Init by double value
     * \param value
     * \param unit
     */
    CLength(double value, const CLengthUnit &unit) : CPhysicalQuantity(value, unit, CLengthUnit::m()) {}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CLength() {}
};
} // namespace
#endif // PQlength_H
