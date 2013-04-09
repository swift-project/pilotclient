/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PQCONSTANTS_H
#define PQCONSTANTS_H

#include "blackmisc/pqallquantities.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

class CPhysicalQuantitiesConstants
{

public:
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static const CTemperature& TemperatureAbsoluteZero() {
        static CTemperature t(-273.15, CTemperatureUnit::C());
        return t;
    }
    /*!
     * \brief Tripe point of purified water, 0.01°C
     * \return
     */
    static const CTemperature& TemperatureTriplePointOfVSMOW() {
        static CTemperature t(-273.16, CTemperatureUnit::K());
        return t;
    }
    /*!
     * \brief Temperature absolute Zero in °C
     * \return
     */
    static const CTemperature& TemperatureAbsoluteZeroC() {
        static CTemperature t(-273.15, CTemperatureUnit::C());
        return t;
    }
    /*!
     * \brief Standard pressure 1013,25mbar / 29.92inHg
     * \return
     */
    static const CPressure& InternationalStandardSeaLevelPressure() {
        static CPressure p(1013.25, CPressureUnit::hPa());
        return p;
    }
    /*!
     * \brief 0m
     * \return
     */
    static const CLength& Length0m() {
        static CLength l(0, CLengthUnit::m());
        return l;
    }
    /*!
     * \brief 0ft
     * \return
     */
    static const CLength& Length0ft() {
        static CLength l(0, CLengthUnit::ft());
        return l;
    }
    /*!
     * \brief Unicom frequency
     * \return
     */
    static const CFrequency& FrequencyUnicom() {
        static CFrequency f(122.8, CFrequencyUnit::MHz());
        return f;
    }
    /*!
     * \brief Civil aircraft emergency frequency
     * \return
     */
    static const CFrequency& FrequencyInternationalAirDistress() {
        static CFrequency f(121.5, CFrequencyUnit::MHz());
        return f;
    }
    /*!
     * \brief Military aircraft emergency frequency
     * \return
     */
    static const CFrequency& FrequencyMilitaryAirDistress() {
        static CFrequency f(243.0, CFrequencyUnit::MHz());
        return f;
    }
};
} //namespace
} // namespace
#endif // PQCONSTANTS_H
