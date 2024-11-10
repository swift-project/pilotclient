// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_H
#define BLACKSOUND_SAMPLEPROVIDER_H

#include "config/buildconfig.h"
#include "blacksound/blacksoundexport.h"
#include <QObject>
#include <QVector>

namespace BlackSound::SampleProvider
{
    //! Sample provider interface
    class BLACKSOUND_EXPORT ISampleProvider : public QObject
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

} // ns

#endif // guard
