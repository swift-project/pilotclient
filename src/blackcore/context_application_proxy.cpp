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
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
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

    /*
     * Connect for signals
     */
    void CContextApplicationProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        // signals originating from impl side
        bool s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                                    "messageLogged", this, SIGNAL(messageLogged(BlackMisc::CStatusMessage, BlackMisc::COriginator)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "componentChanged", this, SIGNAL(componentChanged(uint, uint)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextApplication::ObjectPath(), IContextApplication::InterfaceName(),
                               "fakedSetComVoiceRoom", this, SIGNAL(fakedSetComVoiceRoom(BlackMisc::Audio::CVoiceRoomList)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    /*
     * Log a message
     */
    void CContextApplicationProxy::logMessage(const CStatusMessage &message, const COriginator &origin)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("logMessage"), message, origin);
    }

    /*
     * Ping, is DBus alive?
     */
    qint64 CContextApplicationProxy::ping(qint64 token) const
    {
        qint64 t = this->m_dBusInterface->callDBusRet<qint64>(QLatin1Literal("ping"), token);
        return t;
    }

    /*
     * Component has changed
     */
    void CContextApplicationProxy::notifyAboutComponentChange(uint component, uint action)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("notifyAboutComponentChange"), component, action);
    }

    /*
     * To file
     */
    bool CContextApplicationProxy::writeToFile(const QString &fileName, const QString &content)
    {
        if (fileName.isEmpty()) return false;
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("writeToFile"), fileName, content);
    }

    /*
     * From file
     */
    QString CContextApplicationProxy::readFromFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return "";
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("readFromFile"), fileName);
    }

    /*
     *  Delete file
     */
    bool CContextApplicationProxy::removeFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("removeFile"), fileName);
    }

    /*
     * Check file
     */
    bool CContextApplicationProxy::existsFile(const QString &fileName)
    {
        if (fileName.isEmpty()) return false;
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("existsFile"), fileName);
    }

    void CContextApplicationProxy::processHotkeyFuncEvent(const BlackMisc::Event::CEventHotkeyFunction &event)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("processHotkeyFuncEvent"), event);
    }

} // namespace
