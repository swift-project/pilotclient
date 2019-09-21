/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef SAMPLEPROVIDER_H
#define SAMPLEPROVIDER_H

#include "blacksound/blacksoundexport.h"
#include <QObject>
#include <QVector>

namespace BlackSound
{
    namespace SampleProvider
    {
        //! Sample provider interface
        class BLACKSOUND_EXPORT ISampleProvider : public QObject
        {
            Q_OBJECT

        public:
            ISampleProvider(QObject *parent = nullptr) : QObject(parent) {}
            virtual ~ISampleProvider() override {}

            virtual int readSamples(QVector<qint16> &samples, qint64 count) = 0;

            virtual bool isFinished() { return false; }
        };
    }
}

#endif // guard
