#ifndef PQCONSTANTS_H
#define PQCONSTANTS_H

#include "blackmisc/blackmisc.h"
#include "blackmisc/pqdistance.h"
#include "blackmisc/pqfrequency.h"
#include "blackmisc/pqspeed.h"
#include "blackmisc/pqangle.h"
#include "blackmisc/pqmass.h"
#include "blackmisc/pqpressure.h"
#include "blackmisc/pqtemperature.h"

namespace BlackMisc{

class CPhysicalQuantitiesConstants
{
public:
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static CTemperature& TemperatureAbsoluteZero() { static CTemperature t(-273.15, CTemperatureUnit::C()); return t;}
    /*!
     * \brief Tripe point of purified water, 0.01°C
     * \return
     */
    static CTemperature& TemperatureTriplePointOfVSMOW() { static CTemperature t(-273.16, CTemperatureUnit::K()); return t;}
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static CTemperature& TemperatureAbsoluteZeroC() { static CTemperature t(-273.15, CTemperatureUnit::C()); return t;}
    /*!
     * \brief Standard pressure 1013,25mbar / 29.92inHg
     * \return
     */
    static CPressure& InternationalStandardSeaLevelPressure() { static CPressure p(1013.25, CPressureUnit::hPa()); return p;}
};
} // namespace
#endif // PQCONSTANTS_H
