// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SOUND_SAMPLEPROVIDER_H
#define SWIFT_SOUND_SAMPLEPROVIDER_H

#include <QObject>
#include <QVector>

#include "config/buildconfig.h"
#include "sound/swiftsoundexport.h"

namespace swift::sound::sample_provider
{
    //! Sample provider interface
    class SWIFT_SOUND_EXPORT ISampleProvider : public QObject
    {
        Q_OBJECT

    public:
        //! Ctor
        ISampleProvider(QObject *parent = nullptr) : QObject(parent) {}

        //! Dtor
        virtual ~ISampleProvider() override {}

        //! Read samples
        virtual int readSamples(QVector<float> &samples, qint64 count) = 0;

        //! Finished?
        virtual bool isFinished() const { return false; }

    protected:
        //! Verbose logs?
        bool static verbose() { return swift::config::CBuildConfig::isLocalDeveloperDebugBuild(); }
    };

} // namespace swift::sound::sample_provider

#endif // SWIFT_SOUND_SAMPLEPROVIDER_H
