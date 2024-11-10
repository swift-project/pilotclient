// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_DSP_BIQUADFILTER_H
#define BLACKSOUND_DSP_BIQUADFILTER_H

#include "sound/swiftsoundexport.h"

namespace swift::sound::dsp
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
