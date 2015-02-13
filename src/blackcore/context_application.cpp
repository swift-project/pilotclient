/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_application.h"
#include "blackcore/context_application_impl.h"
#include "blackcore/context_application_proxy.h"
#include "blackcore/context_settings.h"
#include "blackcore/input_manager.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/loghandler.h"
#include <QCoreApplication>
#include <QThread>

using namespace BlackMisc;
using namespace BlackMisc::Settings;

namespace BlackCore
{

    IContextApplication *IContextApplication::create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CRuntimeConfig::Local:
        case CRuntimeConfig::LocalInDbusServer:
            return (new CContextApplication(mode, parent))->registerWithDBus(server);
        case CRuntimeConfig::Remote:
            return new BlackCore::CContextApplicationProxy(BlackCore::CDBusServer::ServiceName(), connection, mode, parent);
        default:
            qFatal("Always initialize an application context!");
            return nullptr;
        }
    }

    /*
     * Constructor
     */
    IContextApplication::IContextApplication(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        CContext(mode, runtime)
    {
        connect(CLogHandler::instance(), &CLogHandler::localMessageLogged, this, [this](const CStatusMessage &message)
        {
            this->logMessage(message, {});
        });

        changeSettings(IContextSettings::SettingsHotKeys);
    }

    void IContextApplication::changeSettings(uint typeValue)
    {
        auto type = static_cast<IContextSettings::SettingsType>(typeValue);
        switch (type)
        {
        case IContextSettings::SettingsHotKeys:
        {
            CSettingKeyboardHotkeyList hotkeys = getIContextSettings()->getHotkeys();
            CInputManager::getInstance()->changeHotkeySettings(hotkeys);
            break;
        }
        default:
            break;
        }
    }

} // namespace
