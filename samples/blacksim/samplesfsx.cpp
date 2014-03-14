/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesfsx.h"
#include "blacksim/blacksimfreefunctions.h"
#include "blacksim/fsx/simconnectutilities.h"
#include <QDebug>

using namespace BlackSim::Fsx;

namespace BlackSimTest
{

    /*
     * Samples
     */
    int CSamplesFsx::samples()
    {
        BlackSim::registerMetadata();
        qDebug() << CSimConnectUtilities::simConnectExceptionToString(CSimConnectUtilities::SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED);
        qDebug() << CSimConnectUtilities::simConnectExceptionToString(CSimConnectUtilities::SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION);
        qDebug() << CSimConnectUtilities::simConnectSurfaceTypeToString(CSimConnectUtilities::Bituminus);
        return 0;
    }

} // namespace
