/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "contextaudio.h"
#include "contextaudioimpl.h"
#include "contextaudioproxy.h"
#include "contextaudioempty.h"

namespace BlackCore
{

    IContextAudio *IContextAudio::create(CRuntime *runtime, CRuntimeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextAudio(mode, runtime))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new CContextAudioProxy(BlackMisc::CDBusServer::ServiceName(), conn, mode, runtime);
        default:
            return new CContextAudioEmpty(runtime); // audio not mandatory
        }
    }
} // namespace
