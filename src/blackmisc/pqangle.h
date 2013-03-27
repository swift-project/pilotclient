#ifndef PQANGLE_H
#define PQANGLE_H
#include "blackmisc/pqphysicalquantity.h"

namespace BlackMisc {

/*!
 * \brief Physical unit degree
 * \author KWB
 */
class CAngle : public CPhysicalQuantity<CAngleUnit, CAngle>
{

public:
    /*!
     * \brief Default constructor
     */
    CAngle() : CPhysicalQuantity(0, CAngleUnit::rad(), CAngleUnit::rad()) {}
    /**
       * \brief Copy constructor
       */
    CAngle(const CAngle &angle) : CPhysicalQuantity(angle) {}
    /*!
     * \brief Init by int value
     * \param value
     * \param unit
     */
    CAngle(qint32 value, const CAngleUnit &unit): CPhysicalQuantity(value, unit, CAngleUnit::rad()) {}
    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CAngle(double value, const CAngleUnit &unit): CPhysicalQuantity(value, unit, CAngleUnit::rad()) {}
    /*!
     * \brief Virtual destructor
     */
    virtual ~CAngle() {}
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

} // namespace

#endif // PQANGLE_H
