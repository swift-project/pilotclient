/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_DSP_BIQUADFILTER_H
#define BLACKSOUND_DSP_BIQUADFILTER_H

#include "blacksound/blacksoundexport.h"

namespace BlackSound::Dsp
{
    //! Digital biquad filter
    class BiQuadFilter
    {
    public:
        //! Ctor
        BiQuadFilter() = default;

        //! Transform
        float transform(float inSample);

        //! @{
        //! Set filter parameters
        void setCoefficients(double aa0, double aa1, double aa2, double b0, double b1, double b2);
        void setLowPassFilter(float sampleRate, float cutoffFrequency, float q);
        void setPeakingEq(float sampleRate, float centreFrequency, float q, float dbGain);
        void setHighPassFilter(float sampleRate, float cutoffFrequency, float q);
        //! @}

        //! @{
        //! Get filters
        static BiQuadFilter lowPassFilter(float sampleRate, float cutoffFrequency, float q);
        static BiQuadFilter highPassFilter(float sampleRate, float cutoffFrequency, float q);
        static BiQuadFilter peakingEQ(float sampleRate, float centreFrequency, float q, float dbGain);
        //! @}

    private:
        double m_a0 = 0.0;
        double m_a1 = 0.0;
        double m_a2 = 0.0;
        double m_a3 = 0.0;
        double m_a4 = 0.0;

        // state
        float m_x1 = 0.0;
        float m_x2 = 0.0;
        float m_y1 = 0.0;
        float m_y2 = 0.0;
    };
} // ns

#endif // guard
