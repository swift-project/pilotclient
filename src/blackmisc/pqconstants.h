#ifndef PQCONSTANTS_H
#define PQCONSTANTS_H

#include "blackmisc/pqallquantities.h"

namespace BlackMisc{

class CPhysicalQuantitiesConstants
{

public:
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static const CTemperature& TemperatureAbsoluteZero() { static CTemperature t(-273.15, CTemperatureUnit::C()); return t;}
    /*!
     * \brief Tripe point of purified water, 0.01°C
     * \return
     */
    static const CTemperature& TemperatureTriplePointOfVSMOW() { static CTemperature t(-273.16, CTemperatureUnit::K()); return t;}
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static const CTemperature& TemperatureAbsoluteZeroC() { static CTemperature t(-273.15, CTemperatureUnit::C()); return t;}
    /*!
     * \brief Standard pressure 1013,25mbar / 29.92inHg
     * \return
     */
    static const CPressure& InternationalStandardSeaLevelPressure() { static CPressure p(1013.25, CPressureUnit::hPa()); return p;}
    /*!
     * \brief 0m
     * \return
     */
    static const CLength& Length0m() { static CLength l(0, CLengthUnit::m()); return l;}
    /*!
     * \brief 0ft
     * \return
     */
    static const CLength& Length0ft() { static CLength l(0, CLengthUnit::ft()); return l;}
};
} // namespace
#endif // PQCONSTANTS_H
