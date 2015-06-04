/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_FREEFUNCTIONS_H
#define BLACKSIMPLUGIN_FREEFUNCTIONS_H

#include "multiplayer_packets.h"
#include "blackmisc/aviation/aircraftsituation.h"

namespace BlackSimPlugin
{
    namespace Fs9
    {
        //! Safely release a COM allocated object
        template <class T>
        void SafeRelease(T*& pT)
        {
            if (pT)
                pT->Release();
            pT = nullptr;
        }

        //! Safely delete an allocated pointer
        template <class T>
        void SafeDelete(T*& pT)
        {
            if( pT != nullptr )
                delete pT;
            pT = nullptr;
        }

        //! Safely delete an allocated array
        template <class T>
        void SafeDeleteArray(T*& pT)
        {
            if(pT)
                delete[] pT;
            pT = nullptr;
        }

        //! Register all relevant metadata in BlackMisc
        void registerMetadata();

        //! Convert FS9 struct to aircraft situation
        BlackMisc::Aviation::CAircraftSituation aircraftSituationfromFS9(const MPPositionVelocity &positionVelocity);

        //! Convert an aircraft situation to a FS9 struct
        MPPositionVelocity aircraftSituationToFS9(const BlackMisc::Aviation::CAircraftSituation &oldSituation,
                                 const BlackMisc::Aviation::CAircraftSituation &newSituation,
                                 double updateInterval);

        //! Convert an aircraft situation to a FS9 struct
        MPPositionSlewMode aircraftSituationToFS9(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Print the direct play error
        HRESULT logDirectPlayError(HRESULT error);

    }
}

#endif // guard
