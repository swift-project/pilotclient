// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file
//! \ingroup samplemiscsim

#include "samplesfsx.h"

#include <QTextStream>

#include "misc/registermetadata.h"
#include "misc/simulation/fsx/simconnectutilities.h"

using namespace swift::misc::simulation::fsx;

namespace swift::sample
{
    void CSamplesFsx::samplesMisc(QTextStream &streamOut)
    {
        swift::misc::registerMetadata();
        streamOut << CSimConnectUtilities::simConnectExceptionToString(
                         CSimConnectUtilities::SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED)
                  << Qt::endl;
        streamOut << CSimConnectUtilities::simConnectExceptionToString(
                         CSimConnectUtilities::SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION)
                  << Qt::endl;
        streamOut << CSimConnectUtilities::simConnectSurfaceTypeToString(CSimConnectUtilities::Bituminus) << Qt::endl;
    }
} // namespace swift::sample
