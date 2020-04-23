/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "volumesampleprovider.h"
#include "blackmisc/metadatautils.h"

using namespace BlackMisc;

namespace BlackSound
{
    namespace SampleProvider
    {
        CVolumeSampleProvider::CVolumeSampleProvider(ISampleProvider *sourceProvider, QObject *parent) :
            ISampleProvider(parent),
            m_sourceProvider(sourceProvider)
        {
            Q_ASSERT_X(sourceProvider, Q_FUNC_INFO, "Need source provider");
            const QString on = QStringLiteral("%1 with source: %2").arg(classNameShort(this), sourceProvider->objectName());
            this->setObjectName(on);
        }

        int CVolumeSampleProvider::readSamples(QVector<float> &samples, qint64 count)
        {
            const int samplesRead = m_sourceProvider->readSamples(samples, count);
            if (!qFuzzyCompare(m_volume, 1.0))
            {
                for (int n = 0; n < samplesRead; n++)
                {
                    samples[n] = static_cast<float>(m_volume * samples[n]);
                }
            }
            return samplesRead;
        }

        bool CVolumeSampleProvider::setVolume(double volume)
        {
            const bool changed = !qFuzzyCompare(m_volume, volume);
            if (changed) { m_volume = volume; }
            return changed;
        }
    } // ns
} // ns
