/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio_impl.h"
#include "context_network.h"
#include "context_ownaircraft.h"
#include "context_application.h"
#include "voice_channel.h"

#include "blacksound/soundgenerator.h"
#include "blackmisc/notificationsounds.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/hotkeyfunction.h"

#include <QTimer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Hardware;
using namespace BlackSound;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextAudio::CContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextAudio(mode, runtime), m_voice(nullptr), m_inputManager(nullptr)
    {
        // 1. Init by "voice driver"
        this->m_voice = new CVoiceVatlib();
        m_voice->moveToThread(&m_threadVoice);
        m_threadVoice.start();

        // 2. Register PTT hotkey function
        m_inputManager = CInputManager::getInstance();
        m_handlePtt = m_inputManager->registerHotkeyFunc(CHotkeyFunction::Ptt(), m_voice, &CVoiceVatlib::handlePushToTalk);

        // 3. own aircraft, if possible
        //if (this->getIContextOwnAircraft()) m_voice->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());

        // 4. Signal / slots
        connect(this->m_voice, &CVoiceVatlib::micTestFinished, this, &CContextAudio::audioTestCompleted);
        connect(this->m_voice, &CVoiceVatlib::squelchTestFinished, this, &CContextAudio::audioTestCompleted);
        //connect(this->m_voice, &CVoiceVatlib::connectionStatusChanged, this, &CContextAudio::ps_connectionStatusChanged);
        if (this->getIContextApplication()) this->connect(this->m_voice, &IVoice::statusMessage, this->getIContextApplication(), &IContextApplication::sendStatusMessage);

        m_channelCom1 = m_voice->getVoiceChannel(0);
        m_channelCom1->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channelCom1.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_com1ConnectionStatusChanged);
        m_channelCom2 = m_voice->getVoiceChannel(1);
        m_channelCom2->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channelCom2.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_com2ConnectionStatusChanged);

        // 5. load sounds (init), not possible in own thread
        QTimer::singleShot(10 * 1000, this, SLOT(ps_initNotificationSounds()));
    }

    /*
     * Cleanup
     */
    CContextAudio::~CContextAudio()
    {
        this->leaveAllVoiceRooms();
        m_threadVoice.quit();
        m_threadVoice.wait(1000);
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoomList CContextAudio::getComVoiceRoomsWithAudioStatus() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return getComVoiceRooms();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom1VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, withAudioStatus);
        // We always have the audio status due to shared status
        return m_channelCom1->getVoiceRoom();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom2VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, withAudioStatus);
        // We always have the audio status due to shared status
        return m_channelCom2->getVoiceRoom();
    }

    /*
     * Voice rooms for COM (const)
     */
    CVoiceRoomList CContextAudio::getComVoiceRooms() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        CVoiceRoomList voiceRoomList;
        voiceRoomList.push_back(m_channelCom1->getVoiceRoom());
        voiceRoomList.push_back(m_channelCom2->getVoiceRoom());
        return voiceRoomList;
    }

    /*
     * Leave all voice rooms
     */
    void CContextAudio::leaveAllVoiceRooms()
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        m_channelCom1->leaveVoiceRoom();
        m_channelCom2->leaveVoiceRoom();
    }

    /*
     * Audio devices
     */
    CAudioDeviceList CContextAudio::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->m_voice->audioDevices();
    }

    /*
     * Audio default devices
     */
    CAudioDeviceList CContextAudio::getCurrentAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        CAudioDeviceList devices;
        devices.push_back(this->m_voice->getCurrentInputDevice());
        devices.push_back(this->m_voice->getCurrentOutputDevice());
        return devices;
    }

    /*
     * Set current device
     */
    void CContextAudio::setCurrentAudioDevice(const CAudioDevice &audioDevice)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(audioDevice.getType() != CAudioDevice::Unknown);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, audioDevice.toQString());
        if (audioDevice.getType() == CAudioDevice::InputDevice)
        {
            this->m_voice->setInputDevice(audioDevice);
        }
        else
        {
            this->m_voice->setOutputDevice(audioDevice);
        }
    }

    /*
     * Set volumes
     */
    void CContextAudio::setVolumes(const CComSystem &com1, const CComSystem &com2)
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, { com1.toQString(), com2.toQString() });

        // volumes
        qint32 vol1 = com1.getVolumeOutput();
        qint32 vol2 = com2.getVolumeOutput();
        m_channelCom1->setRoomOutputVolume(vol1);
        m_channelCom2->setRoomOutputVolume(vol2);

        // enable / disable in the same step
        m_channelCom1->switchAudioOutput(com1.isEnabled());
        m_channelCom2->switchAudioOutput(com2.isEnabled());
    }

    /*
     * Muted?
     */
    bool CContextAudio::isMuted() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return m_channelCom1->isMuted() && m_channelCom2->isMuted();
    }

    /*
     * Set voice rooms
     */
    void CContextAudio::setComVoiceRooms(const CVoiceRoomList &newRooms)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(newRooms.size() == 2);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, newRooms.toQString());

        CVoiceRoomList currentRooms = getComVoiceRooms();
        CVoiceRoom currentRoom1 = currentRooms[0];
        CVoiceRoom currentRoom2 = currentRooms[1];
        CVoiceRoom newRoom1 = newRooms[0];
        CVoiceRoom newRoom2 = newRooms[1];

        bool changed = false;

        // changed rooms?  But only compare on "URL",  not status as connected etc.
        if (currentRoom1.getVoiceRoomUrl() != newRoom1.getVoiceRoomUrl())
        {
            m_channelCom1->leaveVoiceRoom();
            if (newRoom1.isValid())
            {
                m_channelCom1->joinVoiceRoom(newRoom1);
            }
            changed = true;
        }
        if (currentRoom2.getVoiceRoomUrl() != newRoom2.getVoiceRoomUrl())
        {
            m_channelCom2->leaveVoiceRoom();
            if (newRoom2.isValid())
            {
                m_channelCom2->joinVoiceRoom(newRoom2);
            }
            changed = true;
        }

        // changed not yet used, but I keep it for debugging
        // changedVoiceRooms called by connectionStatusChanged;
        Q_UNUSED(changed);
    }

    /*
     * Room 1 callsigns
     */
    CCallsignList CContextAudio::getCom1RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return m_channelCom1->getVoiceRoomCallsigns();
    }

    /*
     * Room 2 callsigns
     */
    CCallsignList CContextAudio::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return m_channelCom2->getVoiceRoomCallsigns();
    }

    /*
     * Room 1 users
     */
    Network::CUserList CContextAudio::getCom1RoomUsers() const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        if (!this->getRuntime()->getIContextNetwork()) return Network::CUserList();
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->getIContextNetwork()->getUsersForCallsigns(this->getCom1RoomCallsigns());
    }

    /*
     * Room 2 users
     */
    Network::CUserList CContextAudio::getCom2RoomUsers() const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        if (!this->getRuntime()->getIContextNetwork()) return Network::CUserList();
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->getIContextNetwork()->getUsersForCallsigns(this->getCom2RoomCallsigns());
    }

    /*
     * SELCAL tone
     */
    void CContextAudio::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, selcal.toQString());
        CAudioDevice outputDevice = m_voice->getCurrentOutputDevice();
        BlackSound::CSoundGenerator::playSelcal(90, selcal, outputDevice);
    }

    /*
     * Notification
     */
    void CContextAudio::playNotification(uint notification, bool considerSettings) const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, QString::number(notification));

        BlackSound::CNotificationSounds::Notification notificationSound = static_cast<BlackSound::CNotificationSounds::Notification>(notification);
        if (considerSettings)
        {
            Q_ASSERT(this->getIContextSettings());
            bool play = this->getIContextSettings()->getAudioSettings().getNotificationFlag(notificationSound);
            if (!play) return;
        }
        BlackSound::CSoundGenerator::playNotificationSound(90, notificationSound);
    }

    /*
     * Notification
     */
    void CContextAudio::ps_initNotificationSounds()
    {
        // not possible in own thread
        CSoundGenerator::playNotificationSound(0, CNotificationSounds::NotificationsLoadSounds);
    }

    /*
     * Mic test.
     */
    void CContextAudio::runMicrophoneTest()
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        this->m_voice->runMicrophoneTest();
    }

    /*
     * Squelch test.
     */
    void CContextAudio::runSquelchTest()
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        this->m_voice->runSquelchTest();
    }

    /*
     * Microphone test
     */
    QString CContextAudio::getMicrophoneTestResult() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->m_voice->micTestResultAsString();
    }

    /*
     * Squelch value
     */
    double CContextAudio::getSquelchValue() const
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return static_cast<double>(this->m_voice->inputSquelch());
    }

    /*
     * Audio loopback
     */
    void CContextAudio::enableAudioLoopback(bool enable)
    {
        Q_ASSERT(this->m_voice);
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        m_voice->enableAudioLoopback(enable);
    }

    /*
     * Connection status changed
     */
    void CContextAudio::ps_com1ConnectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus);

        switch (newStatus)
        {
        case IVoiceChannel::Connected:
            emit this->changedVoiceRooms(getComVoiceRooms(), true);
            break;
        case IVoiceChannel::Disconnecting:
            break;
        case IVoiceChannel::Connecting:
            break;
        case IVoiceChannel::ConnectingFailed:
        case IVoiceChannel::DisconnectedError:
            {
                const QString e = QString("Voice room COM1 error");
                qWarning(e.toUtf8().constData());

                // no break here!
            }
        case IVoiceChannel::Disconnected:
            // good chance to update aircraft
            if (this->getIContextOwnAircraft())
            {
                m_channelCom1->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
                m_channelCom2->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
            }
            emit this->changedVoiceRooms(getComVoiceRooms(), false);
            break;
        default:
            break;
        }
    }

    void CContextAudio::ps_com2ConnectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus);

        switch (newStatus)
        {
        case IVoiceChannel::Connected:
            emit this->changedVoiceRooms(getComVoiceRooms(), true);
            break;
        case IVoiceChannel::Disconnecting:
            break;
        case IVoiceChannel::Connecting:
            break;
        case IVoiceChannel::ConnectingFailed:
        case IVoiceChannel::DisconnectedError:
            {
                const QString e = QString("Voice room COM2 error");
                qWarning(e.toUtf8().constData());

                // no break here!
            }
        case IVoiceChannel::Disconnected:
            // good chance to update aircraft
            if (this->getIContextOwnAircraft())
            {
                m_channelCom1->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
                m_channelCom2->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
            }
            emit this->changedVoiceRooms(getComVoiceRooms(), false);
            break;
        default:
            break;
        }
    }

} // namespace
