// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#ifndef BLACKSAMPLE_SAMPLESVPILOTRULES_H
#define BLACKSAMPLE_SAMPLESVPILOTRULES_H

class QTextStream;

namespace BlackSample
{
    //! Samples for vPilot rules
    class CSamplesVPilotRules
    {
    public:
        //! Run the samples
        static void samples(QTextStream &streamOut, const QTextStream &streamIn);
    };
} // namespace

#endif
