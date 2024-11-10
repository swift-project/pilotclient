// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemisc

#ifndef SWIFT_SAMPLE_SAMPLESFILE_H
#define SWIFT_SAMPLE_SAMPLESFILE_H

#include <QTextStream>

namespace swift::sample
{
    //! Samples for file/directory
    class CSamplesFile
    {
    public:
        //! Run the samples
        static int samples(QTextStream &out);
    };
} // namespace

#endif
