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
     * \brief Constructor
     * \param height
     * \param elevation
     * \param altitude
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
    /*!
     * \brief Factory method for convenience if only one component is available
     * \param initValue
     * \return
     */
    static CAviationVerticalPosition getElevation(const CLength &initValue) { return CAviationVerticalPosition(CAviationVerticalPosition::valueNotSet(), initValue, CAviationVerticalPosition::valueNotSet());}
    /*!
     * \brief Factory method for convenience if only one component is available
     * \param initValue
     * \return
     */
    static CAviationVerticalPosition getAltitude(const CLength &initValue) { return CAviationVerticalPosition(CAviationVerticalPosition::valueNotSet(), CAviationVerticalPosition::valueNotSet(), initValue);}

};
} // namespace

#endif // AVLATERALPOSITION_H
