/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CONSTANTS_H
#define BLACKCORE_AFV_CONSTANTS_H

namespace BlackCore
{
    namespace Afv
    {
        constexpr double MilesToMeters = 1609.34;
        constexpr double MetersToFeet = 3.28084;
        constexpr double FeetToMeters = 0.3048;
        constexpr double NauticalMilesToMeters = 1852;
        constexpr double MetersToNauticalMiles = 0.000539957;
        constexpr double RadToDeg = 57.295779513082320876798154814105;
        constexpr double DegToRad = 0.01745329251994329576923690768489;

        constexpr int c_channelCount = 1;
        constexpr int c_sampleRate = 48000;
    } // ns
} // ns

#endif // guard
