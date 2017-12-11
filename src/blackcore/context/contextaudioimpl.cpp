/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */


#include "blackcore/audiodevice.h"
#include "blackcore/audiomixer.h"
#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/corefacade.h"
#include "blackcore/voice.h"
#include "blackcore/vatsim/voicevatlib.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "blacksound/soundgenerator.h"

#include <stdbool.h>
#include <QTimer>
#include <QtGlobal>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;
using namespace BlackMisc::Audio;
using namespace BlackSound;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    namespace Context
    {
        CContextAudio::CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextAudio(mode, runtime),
            m_voice(new CVoiceVatlib())
        {
            // own aircraft may or may not be available
            const CCallsign ownCallsign = (this->getIContextOwnAircraft()) ? getIContextOwnAircraft()->getOwnAircraft().getCallsign() : CCallsign();

            m_channel1 = m_voice->createVoiceChannel();
            m_channel1->setOwnAircraftCallsign(ownCallsign);
            connect(m_channel1.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_connectionStatusChanged);
            connect(m_channel1.data(), &IVoiceChannel::userJoinedRoom, this, &CContextAudio::ps_userJoinedRoom);
            connect(m_channel1.data(), &IVoiceChannel::userLeftRoom, this, &CContextAudio::ps_userLeftRoom);
            m_channel2 = m_voice->createVoiceChannel();
            m_channel2->setOwnAircraftCallsign(ownCallsign);
            connect(m_channel2.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::ps_connectionStatusChanged);
            connect(m_channel2.data(), &IVoiceChannel::userJoinedRoom, this, &CContextAudio::ps_userJoinedRoom);
            connect(m_channel2.data(), &IVoiceChannel::userLeftRoom, this, &CContextAudio::ps_userLeftRoom);

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
            this->setVoiceOutputVolume(90);

            // Load sounds (init), not possible in own thread
            QTimer::singleShot(10 * 1000, this, &CContextAudio::ps_initNotificationSounds);

            m_unusedVoiceChannels.push_back(m_channel1);
            m_unusedVoiceChannels.push_back(m_channel2);

            m_selcalPlayer = new CSelcalPlayer(QAudioDeviceInfo::defaultOutputDevice(), this);

            changeDeviceSettings();
        }

        CContextAudio *CContextAudio::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

        CContextAudio::~CContextAudio()
        {
            this->leaveAllVoiceRooms();
        }

        CVoiceRoomList CContextAudio::getComVoiceRoomsWithAudioStatus() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return getComVoiceRooms();
        }

        CVoiceRoom CContextAudio::getVoiceRoom(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue, bool withAudioStatus) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << withAudioStatus; }

            auto voiceChannel = m_voiceChannelMapping.value(comUnitValue);
            if (voiceChannel)
            {
                return voiceChannel->getVoiceRoom();
            }
            else
            {
                return CVoiceRoom();
            }
        }

        CVoiceRoomList CContextAudio::getComVoiceRooms() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CVoiceRoomList voiceRoomList;

            auto voiceChannelCom1 = m_voiceChannelMapping.value(BlackMisc::Aviation::CComSystem::Com1);
            if (voiceChannelCom1)
            {
                CVoiceRoom room = voiceChannelCom1->getVoiceRoom();
                voiceRoomList.push_back(room);
            }
            else
            {
                voiceRoomList.push_back(CVoiceRoom());
            }

            auto voiceChannelCom2 = m_voiceChannelMapping.value(BlackMisc::Aviation::CComSystem::Com2);
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

        void CContextAudio::leaveAllVoiceRooms()
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;}
            m_voiceChannelMapping.clear();
            m_channel1->leaveVoiceRoom();
            m_channel2->leaveVoiceRoom();
            m_unusedVoiceChannels.push_back(m_channel1);
            m_unusedVoiceChannels.push_back(m_channel2);
        }

        CIdentifier CContextAudio::audioRunsWhere() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            static const BlackMisc::CIdentifier i("CContextAudio");
            return i;
        }

        CAudioDeviceInfoList CContextAudio::getAudioDevices() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAudioDeviceInfoList devices = m_voiceOutputDevice->getOutputDevices();
            devices = devices.join(m_voiceInputDevice->getInputDevices());
            return devices;
        }

        CAudioDeviceInfoList CContextAudio::getCurrentAudioDevices() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAudioDeviceInfoList devices;
            devices.push_back(m_voiceInputDevice->getCurrentInputDevice());
            devices.push_back(m_voiceOutputDevice->getCurrentOutputDevice());
            return devices;
        }

        void CContextAudio::setCurrentAudioDevice(const CAudioDeviceInfo &audioDevice)
        {
            Q_ASSERT(m_voice);
            Q_ASSERT(audioDevice.getType() != CAudioDeviceInfo::Unknown);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << audioDevice; }
            bool changed = false;
            if (audioDevice.getType() == CAudioDeviceInfo::InputDevice)
            {
                if (m_voiceInputDevice->getCurrentInputDevice() != audioDevice)
                {
                    m_voiceInputDevice->setInputDevice(audioDevice);
                    changed = true;
                }
                if (m_inputDeviceSetting.get() != audioDevice.getName())
                {
                    m_inputDeviceSetting.set(audioDevice.getName());
                }
            }
            else
            {
                if (m_voiceOutputDevice->getCurrentOutputDevice() != audioDevice)
                {
                    m_voiceOutputDevice->setOutputDevice(audioDevice);
                    changed = true;
                }
                if (m_outputDeviceSetting.get() != audioDevice.getName())
                {
                    m_outputDeviceSetting.set(audioDevice.getName());
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
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << volume; }

            bool wasMuted = isMuted();
            bool changed = m_voiceOutputDevice->getOutputVolume() != volume;
            if (!changed) { return; }
            m_voiceOutputDevice->setOutputVolume(volume);
            m_outVolumeBeforeMute = m_voiceOutputDevice->getOutputVolume();

            emit changedAudioVolume(volume);
            if ((volume > 0 && wasMuted) || (volume < 1 && !wasMuted))
            {
                // inform about muted
                emit changedMute(volume < 1);
            }
        }

        int CContextAudio::getVoiceOutputVolume() const
        {
            Q_ASSERT(m_voiceOutputDevice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voiceOutputDevice->getOutputVolume();
        }

        void CContextAudio::setMute(bool muted)
        {
            if (this->isMuted() == muted) { return; } // avoid roundtrips / unnecessary signals
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << muted; }

            int newVolume;
            if (muted)
            {
                Q_ASSERT(m_voiceOutputDevice);
                m_outVolumeBeforeMute = m_voiceOutputDevice->getOutputVolume();
                newVolume = 0;
            }
            else
            {
                newVolume = m_outVolumeBeforeMute < MinUnmuteVolume ? MinUnmuteVolume : m_outVolumeBeforeMute;
                m_outVolumeBeforeMute = newVolume;
            }

            // do not call setVoiceOutputVolume -> infinite loop
            if (newVolume != m_voiceOutputDevice->getOutputVolume())
            {
                m_voiceOutputDevice->setOutputVolume(newVolume);
                emit changedAudioVolume(newVolume);
            }

            // signal
            emit changedMute(muted);
        }

        bool CContextAudio::isMuted() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voiceOutputDevice->getOutputVolume() < 1;
        }

        void CContextAudio::setComVoiceRooms(const CVoiceRoomList &newRooms)
        {
            Q_ASSERT(m_voice);
            Q_ASSERT(newRooms.size() == 2);
            Q_ASSERT(getIContextOwnAircraft());
            if (m_debugEnabled) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << newRooms; }

            CVoiceRoomList currentRooms = getComVoiceRooms();
            CVoiceRoom currentRoomCom1 = currentRooms[0];
            CVoiceRoom currentRoomCom2 = currentRooms[1];
            CVoiceRoom newRoomCom1 = newRooms[0];
            CVoiceRoom newRoomCom2 = newRooms[1];
            const CCallsign ownCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());

            bool changed = false;

            // changed rooms?  But only compare on "URL",  not status as connected etc.
            if (currentRoomCom1.getVoiceRoomUrl() != newRoomCom1.getVoiceRoomUrl())
            {
                auto oldVoiceChannel = m_voiceChannelMapping.value(BlackMisc::Aviation::CComSystem::Com1);
                if (oldVoiceChannel)
                {
                    m_voiceChannelMapping.remove(BlackMisc::Aviation::CComSystem::Com1);

                    // If the voice channel is not used by anybody else
                    if (!m_voiceChannelMapping.key(oldVoiceChannel))
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
                    newVoiceChannel->setOwnAircraftCallsign(ownCallsign);
                    bool inUse = m_voiceChannelMapping.key(newVoiceChannel);
                    m_voiceChannelMapping.insert(BlackMisc::Aviation::CComSystem::Com1, newVoiceChannel);

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
                auto oldVoiceChannel = m_voiceChannelMapping.value(BlackMisc::Aviation::CComSystem::Com2);
                if (oldVoiceChannel)
                {
                    m_voiceChannelMapping.remove(BlackMisc::Aviation::CComSystem::Com2);

                    // If the voice channel is not used by anybody else
                    if (!m_voiceChannelMapping.key(oldVoiceChannel))
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
                    newVoiceChannel->setOwnAircraftCallsign(ownCallsign);
                    bool inUse = m_voiceChannelMapping.key(newVoiceChannel);
                    m_voiceChannelMapping.insert(BlackMisc::Aviation::CComSystem::Com2, newVoiceChannel);

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

        void CContextAudio::setOwnCallsignForRooms(const CCallsign &callsign)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }

            if (m_channel1) { m_channel1->setOwnAircraftCallsign(callsign); }
            if (m_channel2) { m_channel2->setOwnAircraftCallsign(callsign); }
        }

        CCallsignSet CContextAudio::getRoomCallsigns(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

            auto voiceChannel = m_voiceChannelMapping.value(comUnitValue);
            if (voiceChannel)
            {
                return voiceChannel->getVoiceRoomCallsigns();
            }
            else
            {
                return CCallsignSet();
            }
        }

        Network::CUserList CContextAudio::getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const
        {
            Q_ASSERT(m_voice);
            Q_ASSERT(this->getRuntime());
            if (!this->getRuntime()->getIContextNetwork()) return Network::CUserList();
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

            return this->getIContextNetwork()->getUsersForCallsigns(this->getRoomCallsigns(comUnit));
        }

        void CContextAudio::playSelcalTone(const CSelcal &selcal) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << selcal; }
            m_selcalPlayer->play(90, selcal);
        }

        void CContextAudio::playNotification(CNotificationSounds::Notification notification, bool considerSettings) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << notification; }

            bool play = !considerSettings || m_audioSettings.getThreadLocal().getNotificationFlag(notification);
            if (play)
            {
                CSoundGenerator::playNotificationSound(90, notification);
            }
        }

        void CContextAudio::ps_initNotificationSounds()
        {
            // not possible in own thread
            CSoundGenerator::playNotificationSound(0, CNotificationSounds::NotificationsLoadSounds);
        }

        void CContextAudio::enableAudioLoopback(bool enable)
        {
            Q_ASSERT(m_audioMixer);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
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
            Q_ASSERT(m_audioMixer);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_audioMixer->hasMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputOutputDevice1);
        }

        bool CContextAudio::parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator)
        {
            Q_UNUSED(originator);
            if (commandLine.isEmpty()) { return false; }
            CSimpleCommandParser parser(
            {
                ".vol", ".volume",    // output volume
                ".mute",              // mute
                ".unmute"             // unmute
            });
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }

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

        void CContextAudio::ps_connectionStatusChanged(BlackCore::IVoiceChannel::ConnectionStatus oldStatus,
                BlackCore::IVoiceChannel::ConnectionStatus newStatus)
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
                    m_channel1->setOwnAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
                    m_channel2->setOwnAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
                }
                emit this->changedVoiceRooms(getComVoiceRooms(), false);
                break;
            default:
                break;
            }
        }

        void CContextAudio::ps_userJoinedRoom(const CCallsign & /**callsign**/)
        {
            emit this->changedVoiceRoomMembers();
        }

        void CContextAudio::ps_userLeftRoom(const CCallsign & /**callsign**/)
        {
            emit this->changedVoiceRoomMembers();
        }

        void CContextAudio::changeDeviceSettings()
        {
            const QString inputDeviceName = m_inputDeviceSetting.get();
            if (!inputDeviceName.isEmpty())
            {
                for (auto device : m_voiceInputDevice->getInputDevices())
                {
                    if (device.getName() == inputDeviceName)
                    {
                        setCurrentAudioDevice(device);
                        break;
                    }
                }
            }

            const QString outputDeviceName = m_outputDeviceSetting.get();
            if (!outputDeviceName.isEmpty())
            {
                for (auto device : m_voiceOutputDevice->getOutputDevices())
                {
                    if (device.getName() == outputDeviceName)
                    {
                        setCurrentAudioDevice(device);
                        break;
                    }
                }
            }
        }

        QSharedPointer<IVoiceChannel> CContextAudio::getVoiceChannelBy(const CVoiceRoom &voiceRoom)
        {
            QSharedPointer<IVoiceChannel> voiceChannel;
            for (const auto &channel : as_const(m_voiceChannelMapping))
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
} // namespace
