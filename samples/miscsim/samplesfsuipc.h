// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#ifndef SWIFT_SAMPLE_SAMPLESFSUIPC_H
#define SWIFT_SAMPLE_SAMPLESFSUIPC_H

class QTextStream;

namespace swift::sample
{
    //! Samples for FSUIPC
    class CSamplesFsuipc
    {
    public:
        //! Run the misc. samples
        static void samplesFsuipc(QTextStream &streamOut);
    };
} // namespace

#endif
