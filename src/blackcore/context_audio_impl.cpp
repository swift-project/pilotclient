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
#include "blackmisc/logmessage.h"
#include "blackmisc/simplecommandparser.h"

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
        IContextAudio(mode, runtime),
        m_voice(new CVoiceVatlib())
    {
        // 1. Init by "voice driver"

        // 2. Register PTT hotkey function
        m_inputManager = CInputManager::getInstance();
        m_handlePtt = m_inputManager->registerHotkeyFunc(CHotkeyFunction::Ptt(), voice, &CVoiceVatlib::handlePushToTalk);

        m_channelCom1 = m_voice->getVoiceChannel(0);
        m_channelCom1->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channelCom1.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_com1ConnectionStatusChanged);
        m_channelCom2 = m_voice->getVoiceChannel(1);
        m_channelCom2->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channelCom2.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_com2ConnectionStatusChanged);

        // 4. load sounds (init), not possible in own thread
        QTimer::singleShot(10 * 1000, this, SLOT(ps_initNotificationSounds()));
    }

    /*
     * Cleanup
     */
    CContextAudio::~CContextAudio()
    {
        this->leaveAllVoiceRooms();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoomList CContextAudio::getComVoiceRoomsWithAudioStatus() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return getComVoiceRooms();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom1VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << withAudioStatus;
        // We always have the audio status due to shared status
        return m_channelCom1->getVoiceRoom();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom2VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << withAudioStatus;
        // We always have the audio status due to shared status
        return m_channelCom2->getVoiceRoom();
    }

    /*
     * Voice rooms for COM (const)
     */
    CVoiceRoomList CContextAudio::getComVoiceRooms() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
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
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        m_channelCom1->leaveVoiceRoom();
        m_channelCom2->leaveVoiceRoom();
    }

    /*
     * Audio devices
     */
    CAudioDeviceInfoList CContextAudio::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_voice->audioDevices();
    }

    /*
     * Audio default devices
     */
    CAudioDeviceInfoList CContextAudio::getCurrentAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CAudioDeviceInfoList devices;
        devices.push_back(this->m_voice->getCurrentInputDevice());
        devices.push_back(this->m_voice->getCurrentOutputDevice());
        return devices;
    }

    /*
     * Set current device
     */
    void CContextAudio::setCurrentAudioDevice(const CAudioDeviceInfo &audioDevice)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(audioDevice.getType() != CAudioDeviceInfo::Unknown);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << audioDevice;
        bool changed = false;
        if (audioDevice.getType() == CAudioDeviceInfo::InputDevice)
        {
            if (this->m_voice->getCurrentInputDevice() != audioDevice)
            {
                this->m_voice->setInputDevice(audioDevice);
                changed = true;
            }
        }
        else
        {
            if (this->m_voice->getCurrentOutputDevice() != audioDevice)
            {
                this->m_voice->setOutputDevice(audioDevice);
                changed = true;
            }
        }

        if (changed)
        {
            emit changedSelectedAudioDevices(this->getCurrentAudioDevices());
        }
    }

    /*
     * Set volumes
     */
    void CContextAudio::setVolumes(const CComSystem &com1, const CComSystem &com2)
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << com1 << com2;

        // volumes
        qint32 vol1 = com1.getVolumeOutput();
        qint32 vol2 = com2.getVolumeOutput();
        this->setVolumes(vol1, vol2);
    }

    void CContextAudio::setVolumes(int com1Volume, int com2Volume)
    {
        //! \todo Fix when VATLIB 2.0 is available
        int channelV1 = static_cast<int>(m_channelCom1->getVolume());
        int channelV2 = static_cast<int>(m_channelCom2->getVolume());
        if (channelV1 == com1Volume && channelV2 == com2Volume) { return; }

        bool enable1 = com1Volume > 0;
        bool enable2 = com2Volume > 0;
        bool muted = !enable1 && !enable2;

        //! \todo m_channelCom1->setVolume here crashed, also what is correct setRoomOutputVolume or setVolume
        m_channelCom1->setVolume(com1Volume);
        m_channelCom2->setVolume(com2Volume);
        this->setMute(muted);

        emit changedAudioVolumes(com1Volume, com2Volume);
    }

    void CContextAudio::setMute(bool muted)
    {
        if (this->isMuted() == muted) { return; } // avoid roundtrips / unnecessary signals

        m_channelCom1->switchAudioOutput(!muted);
        m_channelCom2->switchAudioOutput(!muted);
        m_channelCom1->setRoomOutputVolume(muted ? 0 : VoiceRoomEnabledVolume);
        m_channelCom2->setRoomOutputVolume(muted ? 0 : VoiceRoomEnabledVolume);

        // adjust volume when unmuted
        if (!muted)
        {
            bool adjusted = false;
            qint32 v1 = this->m_channelCom1->getVolume();
            qint32 v2 = this->m_channelCom2->getVolume();
            //! \todo rectify int/qint/quint mess
            int channelV1 = static_cast<int>(this->m_channelCom1->getVolume());
            int channelV2 = static_cast<int>(this->m_channelCom2->getVolume());
            if (channelV1 < MinUnmuteVolume) { v1 = MinUnmuteVolume; adjusted = true; }
            if (channelV2 < MinUnmuteVolume) { v2 = MinUnmuteVolume; adjusted = true; }
            if (adjusted) { this->setVolumes(v1, v2); }
        }

        // signal
        emit changedMute(muted);
    }

    /*
     * Muted?
     */
    bool CContextAudio::isMuted() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return m_channelCom1->isMuted() && m_channelCom2->isMuted();
    }

    /*
     * Set voice rooms
     */
    void CContextAudio::setComVoiceRooms(const CVoiceRoomList &newRooms)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(newRooms.size() == 2);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << newRooms;

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
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return m_channelCom1->getVoiceRoomCallsigns();
    }

    /*
     * Room 2 callsigns
     */
    CCallsignList CContextAudio::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
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
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
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
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->getIContextNetwork()->getUsersForCallsigns(this->getCom2RoomCallsigns());
    }

    /*
     * SELCAL tone
     */
    void CContextAudio::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << selcal;
        CAudioDeviceInfo outputDevice = m_voice->getCurrentOutputDevice();
        BlackSound::CSoundGenerator::playSelcal(90, selcal, outputDevice);
    }

    /*
     * Notification
     */
    void CContextAudio::playNotification(uint notification, bool considerSettings) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << notification;

        auto notificationSound = static_cast<BlackSound::CNotificationSounds::Notification>(notification);
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
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        // Deprecated
    }

    /*
     * Squelch test.
     */
    void CContextAudio::runSquelchTest()
    {
        CLogMessage(this).warning("This method is deprecated and will be removed soon");
    }

    /*
     * Microphone test
     */
    QString CContextAudio::getMicrophoneTestResult() const
    {
        CLogMessage(this).warning("This method is deprecated and will be removed soon");
        return QString();
    }

    /*
     * Squelch value
     */
    double CContextAudio::getSquelchValue() const
    {
        CLogMessage(this).warning("This method is deprecated and will be removed soon");
        return 0.0;
    }

    /*
     * Audio loopback
     */
    void CContextAudio::enableAudioLoopback(bool enable)
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        m_voice->enableAudioLoopback(enable);
    }

    bool CContextAudio::parseCommandLine(const QString &commandLine)
    {
        static CSimpleCommandParser parser(
        {
            ".vol", ".volume",    // all volumes
            ".vol1", ".volume1",  // COM1 volume
            ".vol2", ".volume2",  // COM2 volume
            ".mute",              // mute
            ".unmute"             // unmute
        });
        if (commandLine.isEmpty()) return false;
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) return false;

        if (parser.matchesCommand(".mute"))
        {
            this->setMute(true);
            return true;
        }
        else if (parser.matchesCommand(".unmute"))
        {
            this->setMute(false);
            return true;
        }
        else if (parser.commandStartsWith("vol") && parser.countParts() > 1)
        {
            qint32 v = parser.toInt(1);
            if (v >= 0 && v <= 100)
            {
                qint32 v1 = this->m_channelCom1->getVolume();
                qint32 v2 = this->m_channelCom2->getVolume();
                if (parser.commandEndsWith("1"))
                {
                    this->setVolumes(v, v2);
                }
                else if (parser.commandEndsWith("2"))
                {
                    this->setVolumes(v1, v);
                }
                else
                {
                    this->setVolumes(v, v);
                }
            }
        }
        return false;
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
            qWarning() << "Voice room COM1 error";
        // intentional fall-through
        case IVoiceChannel::Disconnected:
            if (this->getIContextOwnAircraft())
            {
                // good chance to update aircraft
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
            qWarning() << "Voice room COM2 error";
        // intentional fall-through
        case IVoiceChannel::Disconnected:
            if (this->getIContextOwnAircraft())
            {
                // good chance to update aircraft
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
