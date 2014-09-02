/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIMPLUGIN_FREEFUNCTIONS_H
#define BLACKSIMPLUGIN_FREEFUNCTIONS_H

#include "multiplayer_packets.h"
#include "blackmisc/avaircraftsituation.h"

namespace BlackSimPlugin
{
    namespace Fs9
    {
        //! Register all relevant metadata in BlackMisc
        void registerMetadata();

        //! Convert FS9 struct to aircraft situation
        BlackMisc::Aviation::CAircraftSituation aircraftSituationfromFS9(const MPPositionVelocity &positionVelocity);

        //! Convert an aircraft situation to a FS9 struct
        MPPositionVelocity aircraftSituationtoFS9(const BlackMisc::Aviation::CAircraftSituation &oldSituation,
                                 const BlackMisc::Aviation::CAircraftSituation &newSituation,
                                 double updateInterval);

        HRESULT printDirectPlayError(HRESULT error);
    }
}

#endif //BLACKSIMPLUGIN_FREEFUNCTIONS_H
