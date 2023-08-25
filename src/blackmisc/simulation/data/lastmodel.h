// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_DATA_LASTMODEL_H
#define BLACKMISC_SIMULATION_DATA_LASTMODEL_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/datacache.h"

namespace BlackMisc::Simulation::Data
{
    //! Last model used
    struct TLastModel : public BlackMisc::TDataTrait<CAircraftModel>
    {
        //! \copydoc BlackMisc::TSettingTrait::key
        static const char *key() { return "lastaircraftmodel"; }

        //! \copydoc BlackMisc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Own aircraft model");
            return name;
        }
    };
} // ns

#endif // guard
