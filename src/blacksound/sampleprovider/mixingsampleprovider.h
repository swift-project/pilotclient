/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef MIXINGSAMPLEPROVIDER_H
#define MIXINGSAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include "blacksound/sampleprovider/sampleprovider.h"
#include <QSharedPointer>
#include <QVector>

namespace BlackSound
{
    namespace SampleProvider
    {
        class BLACKSOUND_EXPORT CMixingSampleProvider : public ISampleProvider
        {
        public:
            CMixingSampleProvider(QObject *parent = nullptr) : ISampleProvider(parent) {}

            void addMixerInput(ISampleProvider *provider) { m_sources.append(provider); }
            virtual int readSamples(QVector<qint16> &samples, qint64 count) override;

        private:
            QVector<ISampleProvider *> m_sources;
        };
    }
}

#endif // guard
