// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#ifndef SWIFT_SAMPLE_SAMPLESFSCOMMON_H
#define SWIFT_SAMPLE_SAMPLESFSCOMMON_H

class QTextStream;

namespace swift::sample
{
    //! Samples for FsCommon classes
    class CSamplesFsCommon
    {
    public:
        //! Run the samples
        static void samples(QTextStream &streamOut, QTextStream &streamIn);
    };
} // namespace

#endif
