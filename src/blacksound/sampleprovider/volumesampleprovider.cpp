/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "volumesampleprovider.h"

VolumeSampleProvider::VolumeSampleProvider(ISampleProvider *sourceProvider, QObject *parent) :
    ISampleProvider(parent),
    m_sourceProvider(sourceProvider)
{ }

int VolumeSampleProvider::readSamples(QVector<qint16> &samples, qint64 count)
{
    int samplesRead = m_sourceProvider->readSamples(samples, count);

    if (! qFuzzyCompare(m_volume, 1.0))
    {
        for (int n = 0; n < samplesRead; n++)
        {
            samples[n] *= m_volume;
        }
    }
    return samplesRead;
}

double VolumeSampleProvider::volume() const
{
    return m_volume;
}

void VolumeSampleProvider::setVolume(double volume)
{
    m_volume = volume;
}
