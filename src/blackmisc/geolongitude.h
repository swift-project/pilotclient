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
class CLongitude : public CEarthAngle<CLongitude>
{
protected:
    /*!
     * \brief Specific string representation
     */
    virtual QString convertToQString() const
    {
        QString s = "longitude ";
        return s.append(CEarthAngle::convertToQString());
    }

public:
    /*!
     * \brief Default constructor
     */
    CLongitude() : CEarthAngle() {}

    /*!
     * \brief Constructor
     * \param angle
     */
    CLongitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CLongitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CLongitude() {}
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLongitude)

#endif // guard
