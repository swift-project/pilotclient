/* Copyright (C) 2017
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
