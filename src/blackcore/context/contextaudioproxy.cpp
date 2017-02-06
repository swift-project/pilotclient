/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextaudioproxy.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1Literal>
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
                                   "changedAudioVolume", this, SIGNAL(changedAudioVolume(int)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedAudioDevices", this, SIGNAL(changedAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedSelectedAudioDevices", this, SIGNAL(changedSelectedAudioDevices(BlackMisc::Audio::CAudioDeviceInfoList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                   "changedMute", this, SIGNAL(changedMute(bool)));
            Q_ASSERT(s);
            Q_UNUSED(s);

            this->relayBaseClassSignals(serviceName, connection, IContextAudio::ObjectPath(), IContextAudio::InterfaceName());
        }

        void CContextAudioProxy::leaveAllVoiceRooms()
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("leaveAllVoiceRooms"));
        }

        BlackMisc::Aviation::CCallsignSet CContextAudioProxy::getRoomCallsigns(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignSet>(QLatin1Literal("getRoomCallsigns"), comUnitValue);
        }

        BlackMisc::Network::CUserList CContextAudioProxy::getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getRoomUsers"), comUnitValue);
        }

        CAudioDeviceInfoList CContextAudioProxy::getAudioDevices() const
        {
            return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1Literal("getAudioDevices"));
        }

        CAudioDeviceInfoList CContextAudioProxy::getCurrentAudioDevices() const
        {
            return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1Literal("getCurrentAudioDevices"));
        }

        void CContextAudioProxy::setCurrentAudioDevice(const CAudioDeviceInfo &audioDevice)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("setCurrentAudioDevice"), audioDevice);
        }

        CVoiceRoomList CContextAudioProxy::getComVoiceRoomsWithAudioStatus() const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRoomsWithAudioStatus"));
        }

        CVoiceRoomList CContextAudioProxy::getComVoiceRooms() const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRooms"));
        }

        CVoiceRoom CContextAudioProxy::getVoiceRoom(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue, bool withAudioStatus) const
        {
            return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getVoiceRoom"), comUnitValue, withAudioStatus);
        }

        void CContextAudioProxy::setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("setComVoiceRooms"), voiceRooms);
        }

        void CContextAudioProxy::setOwnCallsignForRooms(const CCallsign &callsign)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("setOwnCallsignForRooms"), callsign);
        }

        void CContextAudioProxy::playSelcalTone(const CSelcal &selcal) const
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("playSelcalTone"), selcal);
        }

        void CContextAudioProxy::playNotification(CNotificationSounds::Notification notification, bool considerSettings) const
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("playNotification"), notification, considerSettings);
        }

        void CContextAudioProxy::setVoiceOutputVolume(int volume)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("setVoiceOutputVolume"), volume);
        }

        int CContextAudioProxy::getVoiceOutputVolume() const
        {
            return this->m_dBusInterface->callDBusRet<int>(QLatin1Literal("getVoiceOutputVolume"));
        }

        void CContextAudioProxy::setMute(bool muted)
        {
            return this->m_dBusInterface->callDBus(QLatin1Literal("setMute"), muted);
        }

        bool CContextAudioProxy::isMuted() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isMuted"));
        }

        void CContextAudioProxy::enableAudioLoopback(bool enable)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("enableAudioLoopback"), enable);
        }

        bool CContextAudioProxy::isAudioLoopbackEnabled() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isAudioLoopbackEnabled"));
        }

        bool CContextAudioProxy::parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("parseCommandLine"), commandLine, originator);
        }
    } // namespace
} // namespace
