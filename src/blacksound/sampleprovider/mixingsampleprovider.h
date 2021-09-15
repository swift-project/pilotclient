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

namespace BlackSound::SampleProvider
{
    //! Mixer
    class BLACKSOUND_EXPORT CMixingSampleProvider : public ISampleProvider
    {
    public:
        //! Ctor mixing provider
        CMixingSampleProvider(QObject *parent = nullptr);

        //! Add a provider
        void addMixerInput(ISampleProvider *provider);

        //! \copydoc ISampleProvider::readSamples
        virtual int readSamples(QVector<float> &samples, qint64 count) override;

    private:
        QVector<ISampleProvider *> m_sources;
    };
} // ns

#endif // guard
