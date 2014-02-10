/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_voice_interface.h"
#include <QObject>
#include <QDBusConnection>

using namespace BlackMisc::Voice;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    IContextVoice::IContextVoice(const QString &serviceName, QDBusConnection &connection, QObject *parent) : QObject(parent), m_dBusInterface(0)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(serviceName , IContextVoice::ServicePath(), IContextVoice::InterfaceName(), connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void IContextVoice::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        connection.connect(serviceName, IContextVoice::ServicePath(), IContextVoice::InterfaceName(),
                           "audioTestCompleted", this, SIGNAL(audioTestCompleted()));
    }

    /*
     * Own aircraft
     */
    void IContextVoice::setOwnAircraft(const CAircraft &ownAircraft)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setOwnAircraft"), ownAircraft);
    }

    /*
     * Leave all voice rooms
     */
    void IContextVoice::leaveAllVoiceRooms()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("leaveAllVoiceRooms"));
    }

    /*
     * COM1 callsigns
     */
    BlackMisc::Aviation::CCallsignList IContextVoice::getCom1RoomCallsigns() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignList>(QLatin1Literal("getCom1RoomCallsigns"));
    }

    /*
     * COM2 callsigns
     */
    BlackMisc::Aviation::CCallsignList IContextVoice::getCom2RoomCallsigns() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignList>(QLatin1Literal("getCom2RoomCallsigns"));
    }

    /*
     * COM1 users
     */
    BlackMisc::Network::CUserList IContextVoice::getCom1RoomUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getCom1RoomUsers"));
    }

    /*
     * COM2 users
     */
    BlackMisc::Network::CUserList IContextVoice::getCom2RoomUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getCom2RoomUsers"));
    }

    /*
     * Audio devices
     */
    CAudioDeviceList IContextVoice::getAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceList>(QLatin1Literal("getAudioDevices"));
    }

    /*
     * Get current audio devices
     */
    BlackMisc::Voice::CAudioDeviceList IContextVoice::getCurrentAudioDevices() const
    {
        return this->m_dBusInterface->callDBusRet<CAudioDeviceList>(QLatin1Literal("getCurrentAudioDevices"));
    }

    /*
     * Set current audio device
     */
    void IContextVoice::setCurrentAudioDevice(const CAudioDevice &audioDevice)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setCurrentAudioDevice"), audioDevice);
    }

    /*
     * Voice rooms, with audio status
     */
    CVoiceRoomList IContextVoice::getComVoiceRoomsWithAudioStatus() const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRoomsWithAudioStatus"));
    }

    /*
     * Voice rooms, without audio status
     */
    CVoiceRoomList IContextVoice::getComVoiceRooms() const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoomList>(QLatin1Literal("getComVoiceRooms"));
    }

    /*
     * Voice room
     */
    CVoiceRoom IContextVoice::getCom1VoiceRoom(bool withAudioStatus) const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getCom1VoiceRoom"), withAudioStatus);
    }

    /*
     * Voice room
     */
    CVoiceRoom IContextVoice::getCom2VoiceRoom(bool withAudioStatus) const
    {
        return this->m_dBusInterface->callDBusRet<CVoiceRoom>(QLatin1Literal("getCom2VoiceRoom"), withAudioStatus);
    }

    /*
     * Set voice rooms
     */
    void IContextVoice::setComVoiceRooms(const BlackMisc::Voice::CVoiceRoom &voiceRoomCom1, const BlackMisc::Voice::CVoiceRoom &voiceRoomCom2)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setComVoiceRooms"), voiceRoomCom1, voiceRoomCom2);
    }

    /*
     * Play SELCAL tone
     */
    void IContextVoice::playSelcalTone(const CSelcal &selcal) const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("playSelcalTone"), selcal);
    }

    /*
     * Notification sound
     */
    void IContextVoice::playNotification(uint notification) const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("playNotification"), notification);
    }

    /*
     * MIC test
     */
    void IContextVoice::runMicrophoneTest()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("runMicrophoneTest"));
    }

    /*
     * Mic test
     */
    void IContextVoice::runSquelchTest()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("runSquelchTest"));
    }

    /*
     *  Test result
     */
    QString IContextVoice::getMicrophoneTestResult() const
    {
        return this->m_dBusInterface->callDBusRet<QString>(QLatin1Literal("getMicrophoneTestResult"));
    }

    /*
     * Squelch value
     */
    double IContextVoice::getSquelchValue() const
    {
        return this->m_dBusInterface->callDBusRet<double>(QLatin1Literal("getSquelchValue"));
    }

    /*
     * Volumes, by COM systems
     */
    void IContextVoice::setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setVolumes"), com1, com2);
    }

    /*
     * Muted?
     */
    bool IContextVoice::isMuted() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isMuted"));
    }

    /*
     * Logging
     */
    void IContextVoice::log(const QString &method, const QString &m1, const QString &m2, const QString &m3, const QString &m4) const
    {
        if (m1.isEmpty())
            qDebug() << "   LOG: " << method;
        else if (m2.isEmpty())
            qDebug() << "   LOG: " << method << m1;
        else if (m3.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2;
        else if (m4.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2 << m3;
        else
            qDebug() << "   LOG: " << method << m1 << m2 << m3 << m4;
    }

} // namespace
