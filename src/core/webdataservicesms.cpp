// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/webdataservicesms.h"

#include "core/application.h"
#include "core/webdataservices.h"
#include "misc/aviation/aircrafticaocodelist.h"

using namespace swift::misc::aviation;

namespace swift::core
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

    bool MSWebServices::checkApp() { return (sApp && !sApp->isShuttingDown() && sApp->getWebDataServices()); }
} // namespace swift::core
