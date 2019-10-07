/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

#include "blacksound/blacksoundexport.h"

namespace BlackSound
{
    namespace Dsp
    {
        //! Digital biquad filter
        class BiQuadFilter
        {
        public:
            //! Ctor
            BiQuadFilter() = default;

            float transform(float inSample);
            void setCoefficients(double aa0, double aa1, double aa2, double b0, double b1, double b2);
            void setLowPassFilter(float sampleRate, float cutoffFrequency, float q);
            void setPeakingEq(float sampleRate, float centreFrequency, float q, float dbGain);
            void setHighPassFilter(float sampleRate, float cutoffFrequency, float q);

            static BiQuadFilter lowPassFilter(float sampleRate, float cutoffFrequency, float q);
            static BiQuadFilter highPassFilter(float sampleRate, float cutoffFrequency, float q);
            static BiQuadFilter peakingEQ(float sampleRate, float centreFrequency, float q, float dbGain);

        private:
            double a0;
            double a1;
            double a2;
            double a3;
            double a4;

            // state
            float x1 = 0.0;
            float x2 = 0.0;
            float y1 = 0.0;
            float y2 = 0.0;
        };
    } // ns
} // ns

#endif // guard
