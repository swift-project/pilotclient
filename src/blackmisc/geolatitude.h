#ifndef BLACKMISC_GEOLATITUDE_H
#define BLACKMISC_GEOLATITUDE_H
#include "blackmisc/geoearthangle.h"

namespace BlackMisc
{
namespace Geo
{

/*!
 * \brief Latitude
 */
class CGeoLatitude : public CGeoEarthAngle<CGeoLatitude>
{
protected:
    /*!
     * \brief Specific string representation
     */
    virtual QString stringForConverter() const
    {
        QString s = "latitude ";
        return s.append(CGeoEarthAngle::stringForConverter());
    }

public:
    /*!
     * \brief Default constructor
     */
    CGeoLatitude() : CGeoEarthAngle() {}

    /*!
     * \brief Copy constructor
     * \param latitude
     */
    CGeoLatitude(const CGeoLatitude &latitude) : CGeoEarthAngle(latitude) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CGeoLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit): CGeoEarthAngle(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CGeoLatitude() {}
};

} // namespace
} // namespace


#endif // guard
