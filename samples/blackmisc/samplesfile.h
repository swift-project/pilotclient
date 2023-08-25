// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmisc

#ifndef BLACKSAMPLE_SAMPLESFILE_H
#define BLACKSAMPLE_SAMPLESFILE_H

#include <QTextStream>

namespace BlackSample
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
