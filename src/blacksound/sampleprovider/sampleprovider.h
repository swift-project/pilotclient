/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSOUND_SAMPLEPROVIDER_H
#define BLACKSOUND_SAMPLEPROVIDER_H

#include "blackconfig/buildconfig.h"
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
        bool static verbose() { return BlackConfig::CBuildConfig::isLocalDeveloperDebugBuild(); }
    };

} // ns

#endif // guard
