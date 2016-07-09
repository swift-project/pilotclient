/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextaudio.h"

#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextaudioempty.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextaudioproxy.h"
#include "blackmisc/dbusserver.h"

namespace BlackCore
{
    namespace Context
    {
        IContextAudio *IContextAudio::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
        {
            switch (mode)
            {
            case CCoreFacadeConfig::Local:
            case CCoreFacadeConfig::LocalInDbusServer:
                return (new CContextAudio(mode, runtime))->registerWithDBus(server);
            case CCoreFacadeConfig::Remote:
                return new CContextAudioProxy(BlackMisc::CDBusServer::coreServiceName(), conn, mode, runtime);
            default:
                return new CContextAudioEmpty(runtime); // audio not mandatory
            }
        }
    } // ns
} // ns
