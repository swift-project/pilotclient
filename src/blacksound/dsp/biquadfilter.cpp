// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "biquadfilter.h"
#include "blackmisc/verify.h"
#include "config/buildconfig.h"

#include <QtMath>
#include <algorithm>

using namespace BlackMisc;
using namespace swift::config;

namespace BlackSound::Dsp
{
    float BiQuadFilter::transform(float inSample)
    {
        // compute result
        double result = m_a0 * inSample + m_a1 * m_x1 + m_a2 * m_x2 - m_a3 * m_y1 - m_a4 * m_y2;

        // shift x1 to x2, sample to x1
        m_x2 = m_x1;
        m_x1 = inSample;

        // shift y1 to y2, result to y1
        m_y2 = m_y1;
        m_y1 = static_cast<float>(result);

        return m_y1;
    }

    void BiQuadFilter::setCoefficients(double aa0, double aa1, double aa2, double b0, double b1, double b2)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild()) { SWIFT_VERIFY_X(qAbs(aa0) > 1E-06, Q_FUNC_INFO, "Div by zero?"); }

        // precompute the coefficients
        m_a0 = b0 / aa0;
        m_a1 = b1 / aa0;
        m_a2 = b2 / aa0;
        m_a3 = aa1 / aa0;
        m_a4 = aa2 / aa0;
    }

    void BiQuadFilter::setLowPassFilter(float sampleRate, float cutoffFrequency, float q)
    {
        // H(s) = 1 / (s^2 + s/Q + 1)
        auto w0 = 2 * M_PI * cutoffFrequency / sampleRate;
        auto cosw0 = qCos(w0);
        auto alpha = qSin(w0) / (2 * q);

        auto b0 = (1 - cosw0) / 2;
        auto b1 = 1 - cosw0;
        auto b2 = (1 - cosw0) / 2;
        auto aa0 = 1 + alpha;
        auto aa1 = -2 * cosw0;
        auto aa2 = 1 - alpha;
        setCoefficients(aa0, aa1, aa2, b0, b1, b2);
    }

    void BiQuadFilter::setPeakingEq(float sampleRate, float centreFrequency, float q, float dbGain)
    {
        // H(s) = (s^2 + s*(A/Q) + 1) / (s^2 + s/(A*Q) + 1)
        auto w0 = 2 * M_PI * centreFrequency / sampleRate;
        auto cosw0 = qCos(w0);
        auto sinw0 = qSin(w0);
        auto alpha = sinw0 / (2 * q);
        auto a = qPow(10, dbGain / 40); // TODO: should we square root this value?

        auto b0 = 1 + alpha * a;
        auto b1 = -2 * cosw0;
        auto b2 = 1 - alpha * a;
        auto aa0 = 1 + alpha / a;
        auto aa1 = -2 * cosw0;
        auto aa2 = 1 - alpha / a;
        setCoefficients(aa0, aa1, aa2, b0, b1, b2);
    }

    void BiQuadFilter::setHighPassFilter(float sampleRate, float cutoffFrequency, float q)
    {
        // H(s) = s^2 / (s^2 + s/Q + 1)
        auto w0 = 2 * M_PI * cutoffFrequency / sampleRate;
        auto cosw0 = qCos(w0);
        auto alpha = qSin(w0) / (2 * q);

        auto b0 = (1 + cosw0) / 2;
        auto b1 = -(1 + cosw0);
        auto b2 = (1 + cosw0) / 2;
        auto aa0 = 1 + alpha;
        auto aa1 = -2 * cosw0;
        auto aa2 = 1 - alpha;
        setCoefficients(aa0, aa1, aa2, b0, b1, b2);
    }

    BiQuadFilter BiQuadFilter::lowPassFilter(float sampleRate, float cutoffFrequency, float q)
    {
        BiQuadFilter filter;
        filter.setLowPassFilter(sampleRate, cutoffFrequency, q);
        return filter;
    }

    BiQuadFilter BiQuadFilter::highPassFilter(float sampleRate, float cutoffFrequency, float q)
    {
        BiQuadFilter filter;
        filter.setHighPassFilter(sampleRate, cutoffFrequency, q);
        return filter;
    }

    BiQuadFilter BiQuadFilter::peakingEQ(float sampleRate, float centreFrequency, float q, float dbGain)
    {
        BiQuadFilter filter;
        filter.setPeakingEq(sampleRate, centreFrequency, q, dbGain);
        return filter;
    }
} // ns
