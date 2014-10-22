/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio.h"
#include "context_audio_impl.h"
#include "context_audio_proxy.h"

namespace BlackCore
{

    IContextAudio *IContextAudio::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextAudio(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextAudioProxy(BlackCore::CDBusServer::ServiceName, conn, mode, parent);
        default:
            return nullptr; // audio not mandatory
        }
    }
} // namespace
