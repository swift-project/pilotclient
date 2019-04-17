/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "contextaudio.h"

#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextaudioempty.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextaudioproxy.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;

namespace BlackCore
{
    namespace Context
    {
        const QString &IContextAudio::InterfaceName()
        {
            static const QString s(BLACKCORE_CONTEXTAUDIO_INTERFACENAME);
            return s;
        }

        const QString &IContextAudio::ObjectPath()
        {
            static const QString s(BLACKCORE_CONTEXTAUDIO_OBJECTPATH);
            return s;
        }

        IContextAudio *IContextAudio::create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
        {
            switch (mode)
            {
            case CCoreFacadeConfig::Local:
            case CCoreFacadeConfig::LocalInDBusServer:
                return (new CContextAudio(mode, runtime))->registerWithDBus(server);
            case CCoreFacadeConfig::Remote:
                return new CContextAudioProxy(CDBusServer::coreServiceName(connection), connection, mode, runtime);
            default:
                return new CContextAudioEmpty(runtime); // audio not mandatory
            }
        }

        QString IContextAudio::audioRunsWhereInfo() const
        {
            if (this->isEmptyObject()) { return "no audio"; }
            const CIdentifier i = this->audioRunsWhere();
            return this->isUsingImplementingObject() ?
                QStringLiteral("Local audio on '%1', '%2'.").arg(i.getMachineName(), i.getProcessName()) :
                QStringLiteral("Remote audio on '%1', '%2'.").arg(i.getMachineName(), i.getProcessName());
        }
    } // ns
} // ns
