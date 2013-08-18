/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_PQCONSTANTS_H
#define BLACKMISC_PQCONSTANTS_H

#include "blackmisc/pqallquantities.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*!
 * \brief Physical quantities constants
 */
class CPhysicalQuantitiesConstants
{
public:
    /*!
     * \brief Tripe point of purified water, 0.01°C
     * \return
     */
    static const CTemperature& TemperatureTriplePointOfVSMOW()
    {
        static CTemperature t(-273.16, CTemperatureUnit::K());
        return t;
    }

    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static const CTemperature& TemperatureAbsoluteZeroC()
    {
        static CTemperature t(-273.15, CTemperatureUnit::C());
        return t;
    }

    /*!
     * \brief International Standard Atmosphere pressure at mean sea level, 1013.25hPa
     * \return
     */
    static const CPressure& ISASeaLevelPressure()
    {
        static CPressure p(1013.25, CPressureUnit::hPa());
        return p;
    }

    /*!
     * \brief ICAO standard pressure datum for flight levels, 1013.2hPa
     * \return
     */
    static const CPressure& ICAOFlightLevelPressure()
    {
        static CPressure p(1013.2, CPressureUnit::hPa());
        return p;
    }

    /*!
     * \brief Standard pressure datum for flight levels in USA, Canada, parts of Latin America, 29.92inHg
     * \return
     */
    static const CPressure& USFlightLevelPressure()
    {
        static CPressure p(29.92, CPressureUnit::inHg());
        return p;
    }

    /*!
     * \brief Unicom frequency
     * \return
     */
    static const CFrequency& FrequencyUnicom()
    {
        static CFrequency f(122.8, CFrequencyUnit::MHz());
        return f;
    }

    /*!
     * \brief Civil aircraft emergency frequency
     * \return
     */
    static const CFrequency& FrequencyInternationalAirDistress()
    {
        static CFrequency f(121.5, CFrequencyUnit::MHz());
        return f;
    }

    /*!
     * \brief Military aircraft emergency frequency
     * \return
     */

    static const CFrequency& FrequencyMilitaryAirDistress()
    {
        static CFrequency f(243.0, CFrequencyUnit::MHz());
        return f;
    }
};

} // namespace
} // namespace

#endif // guard
