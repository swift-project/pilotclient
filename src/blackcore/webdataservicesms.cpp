// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/webdataservicesms.h"
#include "blackcore/webdataservices.h"
#include "blackcore/application.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"

using namespace BlackMisc::Aviation;

namespace BlackCore
{
    int MSWebServices::countAircraftIcaoCodesForDesignator(const QString &designator) const
    {
        if (!checkApp()) { return 0; }
        return sApp->getWebDataServices()->getAircraftIcaoCodesForDesignatorCount(designator);
    }

    int MSWebServices::countAirlineIcaoCodesForDesignator(const QString &designator) const
    {
        if (!checkApp()) { return 0; }
        return sApp->getWebDataServices()->getAirlineIcaoCodesForDesignatorCount(designator);
    }

    bool MSWebServices::checkApp()
    {
        return (sApp && !sApp->isShuttingDown() && sApp->getWebDataServices());
    }
} // ns
