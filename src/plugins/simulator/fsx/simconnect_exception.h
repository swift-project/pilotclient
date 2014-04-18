/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIMPLUGIN_FSX_SIMCONNECT_EXCEPTION_H
#define BLACKSIMPLUGIN_FSX_SIMCONNECT_EXCEPTION_H

#include "simconnect/SimConnect.h"

namespace BlackSimPlugin
{
    namespace Fsx
    {
        //! \brief Handles SimConnect exceptions
        class CSimConnectException
        {
        public:
            CSimConnectException();

            /*!
             * \brief Handle exception
             * \param exception
             */
            static void handleException(SIMCONNECT_EXCEPTION exception);
        };
    }
}

#endif // BLACKSIMPLUGIN_FSX_SIMCONNECT_EXCEPTION_H
