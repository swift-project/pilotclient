/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "pinknoisegenerator.h"

int PinkNoiseGenerator::readSamples(QVector<qint16> &samples, qint64 count)
{
    samples.clear();
    samples.fill(0, count);

    for (int sampleCount = 0; sampleCount < count; sampleCount++)
    {
        double white = 2 * random.generateDouble() - 1;

        pinkNoiseBuffer[0] = 0.99886*pinkNoiseBuffer[0] + white*0.0555179;
        pinkNoiseBuffer[1] = 0.99332*pinkNoiseBuffer[1] + white*0.0750759;
        pinkNoiseBuffer[2] = 0.96900*pinkNoiseBuffer[2] + white*0.1538520;
        pinkNoiseBuffer[3] = 0.86650*pinkNoiseBuffer[3] + white*0.3104856;
        pinkNoiseBuffer[4] = 0.55000*pinkNoiseBuffer[4] + white*0.5329522;
        pinkNoiseBuffer[5] = -0.7616*pinkNoiseBuffer[5] - white*0.0168980;
        double pink = pinkNoiseBuffer[0] + pinkNoiseBuffer[1] + pinkNoiseBuffer[2] + pinkNoiseBuffer[3] + pinkNoiseBuffer[4] + pinkNoiseBuffer[5] + pinkNoiseBuffer[6] + white*0.5362;
        pinkNoiseBuffer[6] = white*0.115926;
        double sampleValue = (m_gain*(pink/5));
        samples[sampleCount] = sampleValue * 32768;
    }
    return count;
}
