/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
