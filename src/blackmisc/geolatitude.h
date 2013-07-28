#ifndef BLACKMISC_GEOLATITUDE_H
#define BLACKMISC_GEOLATITUDE_H

#include <QtCore/qmath.h>
#include "blackmisc/geoearthangle.h"


namespace BlackMisc
{
namespace Geo
{

/*!
 * \brief Latitude
 */
class CLatitude : public CEarthAngle<CLatitude>
{
protected:
    /*!
     * \brief Specific string representation
     */
    virtual QString convertToQString() const
    {
        QString s = "latitude ";
        return s.append(CEarthAngle::convertToQString());
    }

public:
    /*!
     * \brief Default constructor
     */
    CLatitude() : CEarthAngle() {}

    /*!
     * \brief Copy constructor
     * \param latitude
     */
    CLatitude(const CLatitude &latitude) : CEarthAngle(latitude) {}

    /*!
     * \brief Constructor
     * \param angle
     */
    CLatitude(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle(angle) {}

    /*!
     * \brief Init by double value
     * \param value
     * \param unit
     */
    CLatitude(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit): CEarthAngle(value, unit) {}

    /*!
     * \brief Virtual destructor
     */
    virtual ~CLatitude() {}

};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CLatitude)

#endif // guard
