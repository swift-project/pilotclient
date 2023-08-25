// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#include "volumesampleprovider.h"
#include "blackmisc/metadatautils.h"

using namespace BlackMisc;

namespace BlackSound::SampleProvider
{
    CVolumeSampleProvider::CVolumeSampleProvider(ISampleProvider *sourceProvider, QObject *parent) : ISampleProvider(parent),
                                                                                                     m_sourceProvider(sourceProvider)
    {
        Q_ASSERT_X(sourceProvider, Q_FUNC_INFO, "Need source provider");
        const QString on = QStringLiteral("%1 with source: %2").arg(classNameShort(this), sourceProvider->objectName());
        this->setObjectName(on);
    }

    int CVolumeSampleProvider::readSamples(QVector<float> &samples, qint64 count)
    {
        const int samplesRead = m_sourceProvider->readSamples(samples, count);
        if (!qFuzzyCompare(m_gainRatio, 1.0))
        {
            for (int n = 0; n < samplesRead; n++)
            {
                samples[n] = static_cast<float>(m_gainRatio * samples[n]);
            }
        }
        return samplesRead;
    }

    bool CVolumeSampleProvider::setGainRatio(double volume)
    {
        const bool changed = !qFuzzyCompare(m_gainRatio, volume);
        if (changed) { m_gainRatio = volume; }
        return changed;
    }
} // ns
