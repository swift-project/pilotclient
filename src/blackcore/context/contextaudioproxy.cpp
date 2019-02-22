/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextaudioproxy.h"
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
            this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
                serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(), connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextAudioProxy::unitTestRelaySignals()
        {
            // connect signals, asserts when failures
            QDBusConnection con = QDBusConnection::sessionBus();
            CContextAudioProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
            Q_UNUSED(c);
        }

        void CContextAudioProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            bool s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                        "changedVoiceRooms", this, SIGNAL(changedVoiceRooms(BlackMisc::Audio::CVoiceRoomList, bool)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedVoiceRoomMembers", this, SIGNAL(changedVoiceRoomMembers()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedAudioVolume", this, SIGNAL(changedAudioVolume(int)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedMute", this, SIGNAL(changedMute(bool)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedAudioDevices", this, SIGNAL(changedAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedSelectedAudioDevices", this, SIGNAL(changedSelectedAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
            Q_ASSERT(s);

            Q_UNUSED(s);

            this->relayBaseClassSignals(serviceName, connection, IContextAudio::ObjectPath(), IContextAudio::InterfaceName());
        }

        void CContextAudioProxy::leaveAllVoiceRooms()
        {
            this->m_dBusInterface->callDBus(QLatin1String("leaveAllVoiceRooms"));
        }

        BlackMisc::Aviation::CCallsignSet CContextAudioProxy::getRoomCallsigns(CComSystem::ComUnit comUnitValue) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignSet>(QLatin1String("getRoomCallsigns"), comUnitValue);
        }

        BlackMisc::Network::CUserList CContextAudioProxy::getRoomUsers(CComSystem::ComUnit comUnitValue) const
        {
            return this->m_dBusInterface->callDBusRet<CUserList>(QLatin1String("getRoomUsers"), comUnitValue);
        }

        CAudioDeviceInfoList CContextAudioProxy::getAudioDevices() const
        {
            return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1String("getAudioDevices"));
        }

        CIdentifier CContextAudioProxy::audioRunsWhere() const
        {
            return this->m_dBusInterface->callDBusRet<CIdentifier>(QLatin1String("audioRunsWhere"));
        }

        CAudioDeviceInfoList CContextAudioProxy::getCurrentAudioDevices() const
        {
            return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1String("getCurrentAudioDevices"));
        }

        void CContextAudioProxy::setCurrentAudioDevice(const CAudioDeviceInfo &audioDevice)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setCurrentAudioDevice"), audioDevice);
        }

        CVoiceRoomList CContextAudioProxy::getComVoiceRoomsWithAudioStatus() const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1String("getComVoiceRoomsWithAudioStatus"));
        }

        CVoiceRoomList CContextAudioProxy::getComVoiceRooms() const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1String("getComVoiceRooms"));
        }

        CVoiceRoom CContextAudioProxy::getVoiceRoom(CComSystem::ComUnit comUnitValue, bool withAudioStatus) const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1String("getVoiceRoom"), comUnitValue, withAudioStatus);
        }

        void CContextAudioProxy::setComVoiceRooms(const CVoiceRoomList &voiceRooms)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setComVoiceRooms"), voiceRooms);
        }

        void CContextAudioProxy::playSelcalTone(const CSelcal &selcal) const
        {
            this->m_dBusInterface->callDBus(QLatin1String("playSelcalTone"), selcal);
        }

        void CContextAudioProxy::playNotification(CNotificationSounds::NotificationFlag notification, bool considerSettings) const
        {
            this->m_dBusInterface->callDBus(QLatin1String("playNotification"), notification, considerSettings);
        }

        void CContextAudioProxy::setVoiceOutputVolume(int volume)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setVoiceOutputVolume"), volume);
        }

        int CContextAudioProxy::getVoiceOutputVolume() const
        {
            return this->m_dBusInterface->callDBusRet<int>(QLatin1String("getVoiceOutputVolume"));
        }

        void CContextAudioProxy::setMute(bool muted)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setMute"), muted);
        }

        bool CContextAudioProxy::isMuted() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("isMuted"));
        }

        void CContextAudioProxy::enableAudioLoopback(bool enable)
        {
            this->m_dBusInterface->callDBus(QLatin1String("enableAudioLoopback"), enable);
        }

        bool CContextAudioProxy::isAudioLoopbackEnabled() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("isAudioLoopbackEnabled"));
        }

        bool CContextAudioProxy::parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1String("parseCommandLine"), commandLine, originator);
        }

        CVoiceSetup CContextAudioProxy::getVoiceSetup() const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceSetup>(QLatin1String("getVoiceSetup"));
        }

        void CContextAudioProxy::setVoiceSetup(const CVoiceSetup &setup)
        {
            this->m_dBusInterface->callDBus(QLatin1String("setVoiceSetup"), setup);
        }
    } // namespace
} // namespace
