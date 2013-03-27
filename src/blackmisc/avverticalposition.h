#ifndef AVLATERALPOSITION_H
#define AVLATERALPOSITION_H
#include "blackmisc/pqlength.h"

namespace BlackMisc {

/*!
 * \brief Vertical (Z) position of an aircraft
 */
class CAviationVerticalPosition
{
private:
    CLength m_height;  //!< height
    CLength m_elevation;  //!< elevation
    CLength m_altitude;  //!< altitude

public:
    /*!
     * \brief Default constructor
     */
    CAviationVerticalPosition();
    /*!
     * \brief Default constructor
     */
    CAviationVerticalPosition(const CLength &height, const CLength &elevation, const CLength &altitude);
    /*!
     * \brief Value isnot set
     * \return
     */
    static const CLength& valueNotSet() { static CLength notSet(-1, CLengthUnit::m()); return notSet;}
    /*!
     * \brief Factory method for convenience if only one component is available
     * \param initValue
     * \return
     */
    static CAviationVerticalPosition getHeight(const CLength &initValue) { return CAviationVerticalPosition(initValue, CAviationVerticalPosition::valueNotSet(), CAviationVerticalPosition::valueNotSet());}

};
} // namespace

#endif // AVLATERALPOSITION_H
