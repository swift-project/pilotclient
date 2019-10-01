/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "audioutilities.h"

namespace BlackSound
{
    QVector<float> convertBytesTo32BitFloatPCM(const QByteArray input)
    {
        int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
        QVector<float> output;
        output.fill(0, inputSamples);

        for (int n = 0; n < inputSamples; n++)
        {
            output[n] = *reinterpret_cast<const qint16 *>(input.data() + n * 2);
            output[n] /= 32767.0;
        }
        return output;
    }

    QVector<qint16> convertBytesTo16BitPCM(const QByteArray input)
    {
        int inputSamples = input.size() / 2; // 16 bit input, so 2 bytes per sample
        QVector<qint16> output;
        output.fill(0, inputSamples);
        for (int n = 0; n < inputSamples; n++)
        {
            output[n] = *reinterpret_cast<const qint16 *>(input.data() + n * 2);
        }
        return output;
    }

    QVector<qint16> convertFloatBytesTo16BitPCM(const QByteArray input)
    {
        Q_UNUSED(input)
        // qFatal("Not implemented");
        return {};
    }

    QVector<float> convertFromMonoToStereo(const QVector<float> &mono)
    {
        QVector<float> stereo;
        stereo.reserve(mono.size() * 2);
        for (float sample : mono)
        {
            stereo << sample;
            stereo << sample;
        }
        return stereo;
    }

    QVector<qint16> convertFromStereoToMono(const QVector<qint16> &stereo)
    {
        QVector<qint16> mono;
        mono.reserve(stereo.size() / 2);
        for (int i = 0; i < stereo.size(); i = i + 2)
        {
            mono.append(stereo.at(i));
        }
        return mono;
    }

    QVector<float> convertFromShortToFloat(const QVector<qint16> &input)
    {
        QVector<float> output;
        for (auto sample : input)
        {
            output.push_back(sample / 32768.0);
        }
        return output;
    }

} // ns
