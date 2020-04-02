/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
