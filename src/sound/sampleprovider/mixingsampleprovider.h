// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef MIXINGSAMPLEPROVIDER_H
#define MIXINGSAMPLEPROVIDER_H

#include <QSharedPointer>
#include <QVector>

#include "sound/sampleprovider/sampleprovider.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Mixer
    class SWIFT_SOUND_EXPORT CMixingSampleProvider : public ISampleProvider
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
} // namespace swift::sound::sample_provider

#endif // guard
