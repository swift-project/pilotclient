/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_application_proxy.h"
#include "blackcore/input_manager.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/loghandler.h"
#include "blackmisc/originatorlist.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;

namespace BlackCore
{
    CContextApplicationProxy::CContextApplicationProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextApplication(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new CGenericDBusInterface(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(), connection, this);

        // this->m_dBusInterface = new CGenericDBusInterface(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);

        // Enable event forwarding from GUI process to core
        CInputManager *inputManager = CInputManager::getInstance();
        connect(inputManager, &CInputManager::hotkeyFuncEvent, this, &CContextApplicationProxy::processHotkeyFuncEvent);
        inputManager->setEventForwarding(true);

        connect(this, &IContextApplication::messageLogged, this, [](const CStatusMessage & message, const COriginator & origin)
        {
            if (!origin.isFromSameProcess())
            {
                CLogHandler::instance()->logRemoteMessage(message);
            }
        });
    }

    void CContextApplicationProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        // signals originating from impl side
        bool s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                    "messageLogged", this, SIGNAL(messageLogged(BlackMisc::CStatusMessage, BlackMisc::COriginator)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "registrationChanged", this, SIGNAL(registrationChanged()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "fakedSetComVoiceRoom", this, SIGNAL(fakedSetComVoiceRoom(BlackMisc::Audio::CVoiceRoomList)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    void CContextApplicationProxy::logMessage(const CStatusMessage &message, const COriginator &origin)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("logMessage"), message, origin);
    }

    BlackMisc::COriginator CContextApplicationProxy::registerApplication(const COriginator &application)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::COriginator>(QLatin1Literal("registerApplication"), application);
    }

    void CContextApplicationProxy::unregisterApplication(const COriginator &application)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("unregisterApplication"), application);
    }

    BlackMisc::COriginatorList CContextApplicationProxy::getRegisteredApplications() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::COriginatorList>(QLatin1Literal("getRegisteredApplications"));
    }

    bool CContextApplicationProxy::writeToFile(const QString &fileName, const QString &content)
    {
        if (fileName.isEmpty()) { return false; }
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("writeToFile"), fileName, content);
    }

    QString CContextApplicationProxy::readFromFile(const QString &fileName) const
    {
        if (fileName.isEmpty()) { return ""; }
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("readFromFile"), fileName);
    }

    bool CContextApplicationProxy::removeFile(const QString &fileName)
    {
        if (fileName.isEmpty()) { return false; }
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("removeFile"), fileName);
    }

    bool CContextApplicationProxy::existsFile(const QString &fileName) const
    {
        if (fileName.isEmpty()) { return false; }
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("existsFile"), fileName);
    }

    void CContextApplicationProxy::processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("processHotkeyFuncEvent"), event);
    }

} // namespace
