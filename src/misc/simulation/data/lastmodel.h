// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_DATA_LASTMODEL_H
#define SWIFT_MISC_SIMULATION_DATA_LASTMODEL_H

#include "misc/simulation/aircraftmodel.h"
#include "misc/datacache.h"

namespace swift::misc::simulation::data
{
    //! Last model used
    struct TLastModel : public swift::misc::TDataTrait<CAircraftModel>
    {
        //! \copydoc swift::misc::TSettingTrait::key
        static const char *key() { return "lastaircraftmodel"; }

        //! \copydoc swift::misc::TSettingTrait::humanReadable
        static const QString &humanReadable()
        {
            static const QString name("Own aircraft model");
            return name;
        }
    };
} // ns

#endif // guard
