// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_MATCHINGSCRIPTMISC_H
#define BLACKMISC_SIMULATION_MATCHINGSCRIPTMISC_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    //! Matching script type
    enum MatchingScript
    {
        ReverseLookup,
        MatchingStage
    };

    //! Enum as string
    BLACKMISC_EXPORT const QString &msToString(MatchingScript ms);

    //! Return values
    struct MatchingScriptReturnValues
    {
        //! Ctor
        MatchingScriptReturnValues(const BlackMisc::Simulation::CAircraftModel &model) : model(model) {}

        BlackMisc::Simulation::CAircraftModel model; //!< the model
        bool modified = false; //!< modified?
        bool rerun = false; //!< rerun that matching part?
        bool runScript = false; //!< did we run the script

        //! Did run the script with modified result
        bool runScriptAndModified() const { return modified && runScript; }

        //! Did run the script and re-run requested
        bool runScriptAndRerun() const { return rerun && runScript; }

        //! Did run the script, modified value and re-run requested
        bool runScriptModifiedAndRerun() const { return modified && rerun && runScript; }
    };
} // namespace

#endif // guard
