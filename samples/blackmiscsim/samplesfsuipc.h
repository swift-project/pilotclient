/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleblackmiscsim

#ifndef BLACKSAMPLE_SAMPLESFSUIPC_H
#define BLACKSAMPLE_SAMPLESFSUIPC_H

class QTextStream;

namespace BlackSample
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
