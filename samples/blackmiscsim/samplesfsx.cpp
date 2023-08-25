// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup sampleblackmiscsim

#include "samplesfsx.h"
#include "blackmisc/simulation/fsx/simconnectutilities.h"
#include "blackmisc/registermetadata.h"

#include <QTextStream>

using namespace BlackMisc::Simulation::Fsx;

namespace BlackSample
{
    void CSamplesFsx::samplesMisc(QTextStream &streamOut)
    {
        BlackMisc::registerMetadata();
        streamOut << CSimConnectUtilities::simConnectExceptionToString(CSimConnectUtilities::SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED) << Qt::endl;
        streamOut << CSimConnectUtilities::simConnectExceptionToString(CSimConnectUtilities::SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION) << Qt::endl;
        streamOut << CSimConnectUtilities::simConnectSurfaceTypeToString(CSimConnectUtilities::Bituminus) << Qt::endl;
    }
} // namespace
