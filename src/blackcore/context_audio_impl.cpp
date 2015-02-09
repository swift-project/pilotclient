/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio_impl.h"
#include "context_network.h"
#include "context_ownaircraft.h"
#include "context_application.h"
#include "voice_channel.h"
#include "voice_vatlib.h"

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
        m_handlePtt = m_inputManager->registerHotkeyFunc(CHotkeyFunction::Ptt(), this, &CContextAudio::ps_setVoiceTransmission);

        m_channel1 = m_voice->createVoiceChannel();
        m_channel1->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channel1.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_connectionStatusChanged);
        m_channel2 = m_voice->createVoiceChannel();
        m_channel2->setMyAircraftCallsign(getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        connect(m_channel2.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_connectionStatusChanged);

        m_voiceInputDevice = m_voice->createInputDevice();
        m_voiceOutputDevice = m_voice->createOutputDevice();

        m_audioMixer = m_voice->createAudioMixer();

        m_voice->connectVoice(m_voiceInputDevice.get(), m_audioMixer.get(), IAudioMixer::InputMicrophone);
        m_voice->connectVoice(m_channel1.data(), m_audioMixer.get(), IAudioMixer::InputVoiceChannel1);
        m_voice->connectVoice(m_channel2.data(), m_audioMixer.get(), IAudioMixer::InputVoiceChannel2);
        m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputOutputDevice1, m_voiceOutputDevice.get());
        m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputVoiceChannel1, m_channel1.data());
        m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputVoiceChannel2, m_channel2.data());

        m_audioMixer->makeMixerConnection(IAudioMixer::InputVoiceChannel1, IAudioMixer::OutputOutputDevice1);
        m_audioMixer->makeMixerConnection(IAudioMixer::InputVoiceChannel2, IAudioMixer::OutputOutputDevice1);

        // 4. load sounds (init), not possible in own thread
        QTimer::singleShot(10 * 1000, this, SLOT(ps_initNotificationSounds()));

        m_unusedVoiceChannels.push_back(m_channel1);
        m_unusedVoiceChannels.push_back(m_channel2);
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
    CVoiceRoom CContextAudio::getVoiceRoom(int comUnitValue, bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << withAudioStatus;

        auto voiceChannel = m_voiceChannelMapping.value(static_cast<ComUnit>(comUnitValue));

        if (voiceChannel)
            return voiceChannel->getVoiceRoom();
        else
            return CVoiceRoom();
    }

    /*
     * Voice rooms for COM (const)
     */
    CVoiceRoomList CContextAudio::getComVoiceRooms() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CVoiceRoomList voiceRoomList;

        auto voiceChannelCom1 = m_voiceChannelMapping.value(Com1);
        if (voiceChannelCom1)
        {
            CVoiceRoom room = voiceChannelCom1->getVoiceRoom();
            voiceRoomList.push_back(room);
        }
        else
        {
            voiceRoomList.push_back(CVoiceRoom());
        }

        auto voiceChannelCom2 = m_voiceChannelMapping.value(Com2);
        if (voiceChannelCom2)
        {
            CVoiceRoom room = voiceChannelCom2->getVoiceRoom();
            voiceRoomList.push_back(room);
        }
        else
        {
            voiceRoomList.push_back(CVoiceRoom());
        }

        return voiceRoomList;
    }

    /*
     * Leave all voice rooms
     */
    void CContextAudio::leaveAllVoiceRooms()
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        m_voiceChannelMapping.clear();
        m_channel1->leaveVoiceRoom();
        m_channel2->leaveVoiceRoom();
        m_unusedVoiceChannels.push_back(m_channel1);
        m_unusedVoiceChannels.push_back(m_channel2);
    }

    /*
     * Audio devices
     */
    CAudioDeviceInfoList CContextAudio::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CAudioDeviceInfoList devices = this->m_voiceOutputDevice->getOutputDevices();
        devices = devices.join(this->m_voiceInputDevice->getInputDevices());
        return devices;
    }

    /*
     * Audio default devices
     */
    CAudioDeviceInfoList CContextAudio::getCurrentAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CAudioDeviceInfoList devices;
        devices.push_back(this->m_voiceInputDevice->getCurrentInputDevice());
        devices.push_back(this->m_voiceOutputDevice->getCurrentOutputDevice());
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
            if (this->m_voiceInputDevice->getCurrentInputDevice() != audioDevice)
            {
                this->m_voiceInputDevice->setInputDevice(audioDevice);
                changed = true;
            }
        }
        else
        {
            if (this->m_voiceOutputDevice->getCurrentOutputDevice() != audioDevice)
            {
                this->m_voiceOutputDevice->setOutputDevice(audioDevice);
                changed = true;
            }
        }

        if (changed)
        {
            emit changedSelectedAudioDevices(this->getCurrentAudioDevices());
        }
    }

    void CContextAudio::setVoiceOutputVolume(int volume)
    {
        Q_ASSERT(m_voiceOutputDevice);
        m_outDeviceVolume = volume;
        if (!isMuted()) { m_voiceOutputDevice->setOutputVolume(m_outDeviceVolume); }
        emit changedAudioVolume(volume);
    }

    int CContextAudio::getVoiceOutputVolume() const
    {
        Q_ASSERT(m_voiceOutputDevice);
        return m_voiceOutputDevice->getOutputVolume();
    }

    void CContextAudio::setMute(bool muted)
    {
        if (this->isMuted() == muted) { return; } // avoid roundtrips / unnecessary signals

        if (muted)
        {
            m_voiceOutputDevice->setOutputVolume(0);
        }
        else
        {
            m_voiceOutputDevice->setOutputVolume(m_outDeviceVolume);
        }

        // signal
        emit changedMute(muted);
    }

    /*
     * Muted?
     */
    bool CContextAudio::isMuted() const
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return m_voiceOutputDevice->getOutputVolume() == 0;
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
        CVoiceRoom currentRoomCom1 = currentRooms[0];
        CVoiceRoom currentRoomCom2 = currentRooms[1];
        CVoiceRoom newRoomCom1 = newRooms[0];
        CVoiceRoom newRoomCom2 = newRooms[1];

        bool changed = false;

        // changed rooms?  But only compare on "URL",  not status as connected etc.
        if (currentRoomCom1.getVoiceRoomUrl() != newRoomCom1.getVoiceRoomUrl())
        {
            auto oldVoiceChannel = m_voiceChannelMapping.value(Com1);
            if (oldVoiceChannel)
            {
                m_voiceChannelMapping.remove(Com1);

                // If the voice channel is not used by anybody else
                if (!m_voiceChannelMapping.values().contains(oldVoiceChannel))
                {
                    oldVoiceChannel->leaveVoiceRoom();
                    m_unusedVoiceChannels.push_back(oldVoiceChannel);
                }
                else
                {
                    emit this->changedVoiceRooms(getComVoiceRooms(), false);
                }
            }

            if (newRoomCom1.isValid())
            {
                auto newVoiceChannel = getVoiceChannelBy(newRoomCom1);
                bool inUse = m_voiceChannelMapping.values().contains(newVoiceChannel);
                m_voiceChannelMapping.insert(Com1, newVoiceChannel);

                // If the voice channel is not used by anybody else
                if (!inUse)
                {
                    newVoiceChannel->joinVoiceRoom(newRoomCom1);
                }
                else
                {
                    emit this->changedVoiceRooms(getComVoiceRooms(), true);
                }
            }
            changed = true;
        }
        // changed rooms?  But only compare on "URL",  not status as connected etc.
        if (currentRoomCom2.getVoiceRoomUrl() != newRoomCom2.getVoiceRoomUrl())
        {
            auto oldVoiceChannel = m_voiceChannelMapping.value(Com2);
            if (oldVoiceChannel)
            {
                m_voiceChannelMapping.remove(Com2);

                // If the voice channel is not used by anybody else
                if (!m_voiceChannelMapping.values().contains(oldVoiceChannel))
                {
                    oldVoiceChannel->leaveVoiceRoom();
                    m_unusedVoiceChannels.push_back(oldVoiceChannel);
                }
                else
                {
                    emit this->changedVoiceRooms(getComVoiceRooms(), false);
                }
            }

            if (newRoomCom2.isValid())
            {
                auto newVoiceChannel = getVoiceChannelBy(newRoomCom2);
                bool inUse = m_voiceChannelMapping.values().contains(newVoiceChannel);
                m_voiceChannelMapping.insert(Com2, newVoiceChannel);

                // If the voice channel is not used by anybody else
                if (!inUse)
                {
                    newVoiceChannel->joinVoiceRoom(newRoomCom2);
                }
                else
                {
                    emit this->changedVoiceRooms(getComVoiceRooms(), true);
                }
            }
            changed = true;
        }

        // changed not yet used, but I keep it for debugging
        // changedVoiceRooms called by connectionStatusChanged;
        Q_UNUSED(changed);
    }

    CCallsignList CContextAudio::getRoomCallsigns(int comUnitValue) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;

        auto voiceChannel = m_voiceChannelMapping.value(static_cast<ComUnit>(comUnitValue));
        if (voiceChannel)
            return voiceChannel->getVoiceRoomCallsigns();
        else
            return CCallsignList();
    }

    Network::CUserList CContextAudio::getRoomUsers(int comUnitValue) const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        if (!this->getRuntime()->getIContextNetwork()) return Network::CUserList();
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;

        auto comUnit = static_cast<ComUnit>(comUnitValue);
        return this->getIContextNetwork()->getUsersForCallsigns(this->getRoomCallsigns(comUnit));
    }

    /*
     * SELCAL tone
     */
    void CContextAudio::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << selcal;
        CAudioDeviceInfo outputDevice = m_voiceOutputDevice->getCurrentOutputDevice();
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
     * Audio loopback
     */
    void CContextAudio::enableAudioLoopback(bool enable)
    {
        Q_ASSERT(this->m_audioMixer);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        if (enable)
        {
            m_audioMixer->makeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
        }
        else
        {
            m_audioMixer->removeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
        }
    }

    bool CContextAudio::isAudioLoopbackEnabled() const
    {
        Q_ASSERT(this->m_audioMixer);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_audioMixer->hasMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
    }

    bool CContextAudio::parseCommandLine(const QString &commandLine)
    {
        static CSimpleCommandParser parser(
        {
            ".vol", ".volume",    // output volume
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
            int v = parser.toInt(1);
            if (v >= 0 && v <= 300)
            {
                setVoiceOutputVolume(v);
                return true;
            }
        }
        return false;
    }

    void CContextAudio::ps_setVoiceTransmission(bool enable)
    {
        // FIXME: Use the 'active' channel instead of hardcoded COM1
        if (enable) m_audioMixer->makeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputVoiceChannel1);
        else m_audioMixer->removeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputVoiceChannel1);
    }

    /*
     * Connection status changed
     */
    void CContextAudio::ps_connectionStatusChanged(IVoiceChannel::ConnectionStatus oldStatus, IVoiceChannel::ConnectionStatus newStatus)
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
            CLogMessage(this).warning("Voice channel disconnecting error");
        // intentional fall-through
        case IVoiceChannel::Disconnected:
            if (this->getIContextOwnAircraft())
            {
                // good chance to update aircraft
                m_channel1->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
                m_channel2->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
            }
            emit this->changedVoiceRooms(getComVoiceRooms(), false);
            break;
        default:
            break;
        }
    }

    QSharedPointer<IVoiceChannel> CContextAudio::getVoiceChannelBy(const CVoiceRoom &voiceRoom)
    {
        QSharedPointer<IVoiceChannel> voiceChannel;
        for (const auto &channel : m_voiceChannelMapping.values())
        {
            if (channel->getVoiceRoom().getVoiceRoomUrl() == voiceRoom.getVoiceRoomUrl()) voiceChannel = channel;
        }

        // If we haven't found a valid voice channel pointer, get an unused one
        if (!voiceChannel)
        {
            Q_ASSERT(!m_unusedVoiceChannels.isEmpty());
            voiceChannel = m_unusedVoiceChannels.takeFirst();
        }

        return voiceChannel;
    }

} // namespace
