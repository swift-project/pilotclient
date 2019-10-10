/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextaudioproxy.h"
#include "blackmisc/audio/ptt.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackCore
{
    namespace Context
    {
        CContextAudioProxy::CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextAudio(mode, runtime), m_dBusInterface(nullptr)
        {
            this->m_dBusInterface = new CGenericDBusInterface(
                serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(), connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextAudioProxy::unitTestRelaySignals()
        {
            // connect signals, asserts when failures
            QDBusConnection con = QDBusConnection::sessionBus();
            CContextAudioProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
            Q_UNUSED(c)
        }

        void CContextAudioProxy::registerDevices(const CAudioDeviceInfoList &devices)
        {
            if (devices.isEmpty()) { return; }
            m_dBusInterface->callDBus(QLatin1String("registerDevices"));
        }

        void CContextAudioProxy::unRegisterDevices(const CAudioDeviceInfoList &devices)
        {
            if (devices.isEmpty()) { return; }
            m_dBusInterface->callDBus(QLatin1String("unRegisterDevices"));
        }

        CAudioDeviceInfoList CContextAudioProxy::getRegisteredDevices() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Audio::CAudioDeviceInfoList>(QLatin1String("getRegisteredDevices"));
        }

        void CContextAudioProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            /**
            bool s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                        "changedAudioVolume", this, SIGNAL(changedAudioVolume(int)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedMute", this, SIGNAL(changedMute(bool)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedAudioDevices", this, SIGNAL(changedAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
            **/

            this->relayBaseClassSignals(serviceName, connection, IContextAudio::ObjectPath(), IContextAudio::InterfaceName());
        }

    } // namespace
} // namespace
