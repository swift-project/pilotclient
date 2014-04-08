/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio_proxy.h"
#include <QDBusConnection>

using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextAudioProxy::CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextAudio(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void CContextAudioProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                           "audioTestCompleted", this, SIGNAL(audioTestCompleted()));
    }

    /*
     * Own aircraft
     */
    void CContextAudioProxy::setOwnAircraft(const CAircraft &ownAircraft)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setOwnAircraft"), ownAircraft);
    }

    /*
     * Leave all voice rooms
     */
    void CContextAudioProxy::leaveAllVoiceRooms()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("leaveAllVoiceRooms"));
    }

    /*
     * COM1 callsigns
     */
    BlackMisc::Aviation::CCallsignList CContextAudioProxy::getCom1RoomCallsigns() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignList>(QLatin1Literal("getCom1RoomCallsigns"));
    }

    /*
     * COM2 callsigns
     */
    BlackMisc::Aviation::CCallsignList CContextAudioProxy::getCom2RoomCallsigns() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignList>(QLatin1Literal("getCom2RoomCallsigns"));
    }

    /*
     * COM1 users
     */
    BlackMisc::Network::CUserList CContextAudioProxy::getCom1RoomUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getCom1RoomUsers"));
    }

    /*
     * COM2 users
     */
    BlackMisc::Network::CUserList CContextAudioProxy::getCom2RoomUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getCom2RoomUsers"));
    }

    /*
     * Audio devices
     */
    CAudioDeviceList CContextAudioProxy::getAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceList>(QLatin1Literal("getAudioDevices"));
    }

    /*
     * Get current audio devices
     */
    BlackMisc::Audio::CAudioDeviceList CContextAudioProxy::getCurrentAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceList>(QLatin1Literal("getCurrentAudioDevices"));
    }

    /*
     * Set current audio device
     */
    void CContextAudioProxy::setCurrentAudioDevice(const CAudioDevice &audioDevice)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setCurrentAudioDevice"), audioDevice);
    }

    /*
     * Voice rooms, with audio status
     */
    CVoiceRoomList CContextAudioProxy::getComVoiceRoomsWithAudioStatus() const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRoomsWithAudioStatus"));
    }

    /*
     * Voice rooms, without audio status
     */
    CVoiceRoomList CContextAudioProxy::getComVoiceRooms() const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRooms"));
    }

    /*
     * Voice room
     */
    CVoiceRoom CContextAudioProxy::getCom1VoiceRoom(bool withAudioStatus) const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getCom1VoiceRoom"), withAudioStatus);
    }

    /*
     * Voice room
     */
    CVoiceRoom CContextAudioProxy::getCom2VoiceRoom(bool withAudioStatus) const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getCom2VoiceRoom"), withAudioStatus);
    }

    /*
     * Set voice rooms
     */
    void CContextAudioProxy::setComVoiceRooms(const BlackMisc::Audio::CVoiceRoom &voiceRoomCom1, const BlackMisc::Audio::CVoiceRoom &voiceRoomCom2)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setComVoiceRooms"), voiceRoomCom1, voiceRoomCom2);
    }

    /*
     * Play SELCAL tone
     */
    void CContextAudioProxy::playSelcalTone(const CSelcal &selcal) const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("playSelcalTone"), selcal);
    }

    /*
     * Notification sound
     */
    void CContextAudioProxy::playNotification(uint notification) const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("playNotification"), notification);
    }

    /*
     * MIC test
     */
    void CContextAudioProxy::runMicrophoneTest()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("runMicrophoneTest"));
    }

    /*
     * Mic test
     */
    void CContextAudioProxy::runSquelchTest()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("runSquelchTest"));
    }

    /*
     *  Test result
     */
    QString CContextAudioProxy::getMicrophoneTestResult() const
    {
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("getMicrophoneTestResult"));
    }

    /*
     * Squelch value
     */
    double CContextAudioProxy::getSquelchValue() const
    {
        return this->m_dBusInterface->callDBusRet<double>(QLatin1Literal("getSquelchValue"));
    }

    /*
     * Volumes, by COM systems
     */
    void CContextAudioProxy::setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setVolumes"), com1, com2);
    }

    /*
     * Muted?
     */
    bool CContextAudioProxy::isMuted() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isMuted"));
    }

} // namespace
