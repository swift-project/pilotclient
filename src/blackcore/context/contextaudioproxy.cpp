// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/context/contextaudioproxy.h"
#include "blackcore/afv/clients/afvclient.h"
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
using namespace BlackCore::Afv::Clients;

namespace BlackCore::Context
{
    CContextAudioProxy::CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : CContextAudioBase(mode, runtime), m_dBusInterface(nullptr)
    {
        m_dBusInterface = new CGenericDBusInterface(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);

        connect(this, &CContextAudioProxy::changedLocalAudioDevices, this, &CContextAudioProxy::onChangedLocalDevices, Qt::QueuedConnection);
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
        m_dBusInterface->callDBus(QLatin1String("registerDevices"), devices);
    }

    void CContextAudioProxy::unRegisterDevices(const CAudioDeviceInfoList &devices)
    {
        if (devices.isEmpty()) { return; }
        m_dBusInterface->callDBus(QLatin1String("unRegisterDevices"), devices);
    }

    void CContextAudioProxy::unRegisterDevicesFor(const CIdentifier &identifier)
    {
        m_dBusInterface->callDBus(QLatin1String("unRegisterDevicesFor"), identifier);
    }

    void CContextAudioProxy::registerAudioCallsign(const CCallsign &callsign, const CIdentifier &identifier)
    {
        m_dBusInterface->callDBus(QLatin1String("registerAudioCallsign"), callsign, identifier);
    }

    void CContextAudioProxy::unRegisterAudioCallsign(const CCallsign &callsign, const CIdentifier &identifier)
    {
        m_dBusInterface->callDBus(QLatin1String("unRegisterAudioCallsign"), callsign, identifier);
    }

    bool CContextAudioProxy::hasRegisteredAudioCallsign(const CCallsign &callsign) const
    {
        return m_dBusInterface->callDBusRet<bool>(QLatin1String("hasRegisteredAudioCallsign"), callsign);
    }

    CAudioDeviceInfoList CContextAudioProxy::getRegisteredDevices() const
    {
        return m_dBusInterface->callDBusRet<BlackMisc::Audio::CAudioDeviceInfoList>(QLatin1String("getRegisteredDevices"));
    }

    void CContextAudioProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                    "voiceClientFailure", this, SIGNAL(voiceClientFailure(BlackMisc::CStatusMessage)));
        Q_ASSERT(s);

        /**
        s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                    "changedAudioVolume", this, SIGNAL(changedAudioVolume(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                "changedMute", this, SIGNAL(changedMute(bool)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                "changedLocalAudioDevices", this, SIGNAL(changedLocalAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
        **/

        this->relayBaseClassSignals(serviceName, connection, IContextAudio::ObjectPath(), IContextAudio::InterfaceName());
    }

} // namespace
