#include "avverticalposition.h"

namespace BlackMisc {

/**
 * @brief Default constructor
 */
CAviationVerticalPosition::CAviationVerticalPosition() :m_elevation(CAviationVerticalPosition::valueNotSet()), m_height(CAviationVerticalPosition::valueNotSet()), m_altitude(CAviationVerticalPosition::valueNotSet())
{
    // void
}

/**
 * Constructor
 */
CAviationVerticalPosition::CAviationVerticalPosition(const CLength &height, const CLength &elevation, const CLength &altitude) : m_height(height), m_elevation(elevation), m_altitude(altitude)
{
    // void
}
}
