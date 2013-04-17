#ifndef BLACKMISC_GEOLONGITUDE_H
#define BLACKMISC_GEOLONGITUDE_H
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
namespace Geo
{

/*!
 * \brief Longitude
 */
class CGeoLongitude : public CGeoEarthAngle<CGeoLongitude>
{
protected:
    /*!
     * \brief Specific string representation
     */
    virtual QString stringForConverter() const
    {
        QString s = "longitude ";
        return s.append(CGeoEarthAngle::stringForConverter());
    }

public:
    /*!
     * \brief Default constructor
     */
    CGeoLongitude() : CGeoEarthAngle() {}

    /*!
     * \brief Copy constructor
     * \param Longitude
     */
    CGeoLongitude(const CGeoLongitude &Longitude) : CGeoEarthAngle(Longitude) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CGeoLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit): CGeoEarthAngle(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CGeoLongitude() {}
};

} // namespace
} // namespace

#endif // guard
