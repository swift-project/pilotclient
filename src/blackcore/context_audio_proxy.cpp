/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
            serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Relaying signals
     */
    void CContextAudioProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                                    "audioTestCompleted", this, SIGNAL(audioTestCompleted()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                               "changedVoiceRooms", this, SIGNAL(changedVoiceRooms(BlackMisc::Audio::CVoiceRoomList, bool)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextAudio::ObjectPath(), IContextAudio::InterfaceName(),
                               "changedAudioVolumes", this, SIGNAL(changedAudioVolumes(qint32, qint32)));
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
    BlackMisc::Aviation::CCallsignList CContextAudioProxy::getRoomCallsigns(int comUnitValue) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignList>(QLatin1Literal("getRoomCallsigns"), comUnitValue);
    }

    /*
     * COM1 users
     */
    BlackMisc::Network::CUserList CContextAudioProxy::getRoomUsers(int comUnitValue) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getRoomUsers"), comUnitValue);
    }

    /*
     * Audio devices
     */
    CAudioDeviceInfoList CContextAudioProxy::getAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1Literal("getAudioDevices"));
    }

    /*
     * Get current audio devices
     */
    CAudioDeviceInfoList CContextAudioProxy::getCurrentAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceInfoList>(QLatin1Literal("getCurrentAudioDevices"));
    }

    /*
     * Set current audio device
     */
    void CContextAudioProxy::setCurrentAudioDevice(const CAudioDeviceInfo &audioDevice)
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
    CVoiceRoom CContextAudioProxy::getVoiceRoom(int comUnitValue, bool withAudioStatus) const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getVoiceRoom"), comUnitValue, withAudioStatus);
    }

    /*
     * Set voice rooms
     */
    void CContextAudioProxy::setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setComVoiceRooms"), voiceRooms);
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
    void CContextAudioProxy::playNotification(uint notification, bool considerSettings) const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("playNotification"), notification, considerSettings);
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
     * Volumes
     */
    void CContextAudioProxy::setVolumes(int com1Volume, int com2Volume)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setVolumes"), com1Volume, com2Volume);
    }

    void CContextAudioProxy::setVoiceOutputVolume(int volume)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setVoiceOutputVolume"), volume);
    }

    /*
     * Toggle mute
     */
    void CContextAudioProxy::setMute(bool muted)
    {
        return this->m_dBusInterface->callDBus(QLatin1Literal("setMute"), muted);
    }

    /*
     * Muted?
     */
    bool CContextAudioProxy::isMuted() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isMuted"));
    }

    /*
     * Loopback
     */
    void CContextAudioProxy::enableAudioLoopback(bool enable)
    {
        return this->m_dBusInterface->callDBus(QLatin1Literal("enableAudioLoopback"), enable);
    }

    /*
     * Parse command line
     */
    bool CContextAudioProxy::parseCommandLine(const QString &commandLine)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("parseCommandLine"), commandLine);
    }

} // namespace
