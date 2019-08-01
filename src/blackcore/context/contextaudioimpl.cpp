/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */


#include "blackcore/context/contextaudioimpl.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/application.h"
#include "blackcore/audiodevice.h"
#include "blackcore/corefacade.h"
#include "blackcore/voice.h"
#include "blackcore/vatsim/voicevatlib.h"
#include "blackmisc/simulation/simulatedaircraft.h"
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
#include "blackmisc/statusmessage.h"
#include "blacksound/soundgenerator.h"

#include <QTimer>
#include <QtGlobal>
#include <QPointer>

#include <algorithm>
#include <stdbool.h>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Input;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
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
            initVoiceChannels();
            initInputDevice();
            initOutputDevice();
            initAudioMixer();

            this->setVoiceOutputVolume(m_audioSettings.getThreadLocal().getAudioVolume());
            m_selcalPlayer = new CSelcalPlayer(QAudioDeviceInfo::defaultOutputDevice(), this);

            this->changeDeviceSettings();
            QPointer<CContextAudio> myself(this);
            QTimer::singleShot(5000, this, [ = ]
            {
                if (!myself) { return; }
                if (!sApp || sApp->isShuttingDown()) { return; }
                myself->onChangedAudioSettings();
            });
        }

        CContextAudio *CContextAudio::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

        void CContextAudio::initVoiceChannels()
        {
            //! \todo KB 2018-11 those are supposed to be Qt::QueuedConnection, but not yet changed (risk to break something)
            m_channel1 = m_voice->createVoiceChannel();
            connect(m_channel1.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::onConnectionStatusChanged);
            connect(m_channel1.data(), &IVoiceChannel::userJoinedRoom,          this, &CContextAudio::onUserJoinedRoom);
            connect(m_channel1.data(), &IVoiceChannel::userLeftRoom,            this, &CContextAudio::onUserLeftRoom);
            m_channel2 = m_voice->createVoiceChannel();
            connect(m_channel2.data(), &IVoiceChannel::connectionStatusChanged, this, &CContextAudio::onConnectionStatusChanged);
            connect(m_channel2.data(), &IVoiceChannel::userJoinedRoom,          this, &CContextAudio::onUserJoinedRoom);
            connect(m_channel2.data(), &IVoiceChannel::userLeftRoom,            this, &CContextAudio::onUserLeftRoom);

            m_unusedVoiceChannels.push_back(m_channel1);
            m_unusedVoiceChannels.push_back(m_channel2);

            m_voiceChannelOutputPortMapping[m_channel1] = IAudioMixer::OutputVoiceChannel1;
            m_voiceChannelOutputPortMapping[m_channel2] = IAudioMixer::OutputVoiceChannel2;
        }

        void CContextAudio::initInputDevice()
        {
#ifdef Q_OS_MAC
            CMacOSMicrophoneAccess::AuthorizationStatus status = m_micAccess.getAuthorizationStatus();
            if (status == CMacOSMicrophoneAccess::Authorized)
            {
                m_voiceInputDevice = m_voice->createInputDevice();
                CLogMessage(this).info(u"MacOS specific input device init");
            }
            else if (status == CMacOSMicrophoneAccess::NotDetermined)
            {
                m_voiceInputDevice.reset(new CAudioInputDeviceDummy(this));
                connect(&m_micAccess, &CMacOSMicrophoneAccess::permissionRequestAnswered, this, &CContextAudio::delayedInitMicrophone);
                m_micAccess.requestAccess();
                CLogMessage(this).info(u"MacOS requested input device");
            }
            else
            {
                m_voiceInputDevice.reset(new CAudioInputDeviceDummy(this));
                CLogMessage(this).error(u"Microphone access not granted. Voice input will not work.");
            }
#else
            m_voiceInputDevice = m_voice->createInputDevice();
#endif
        }

        void CContextAudio::initOutputDevice()
        {
            m_voiceOutputDevice = m_voice->createOutputDevice();
        }

        void CContextAudio::initAudioMixer()
        {
            m_audioMixer = m_voice->createAudioMixer();

            if (! m_voiceInputDevice->isDummyDevice())
            {
                m_voice->connectVoice(m_voiceInputDevice.get(), m_audioMixer.get(), IAudioMixer::InputMicrophone);
            }
            m_voice->connectVoice(m_channel1.data(), m_audioMixer.get(), IAudioMixer::InputVoiceChannel1);
            m_voice->connectVoice(m_channel2.data(), m_audioMixer.get(), IAudioMixer::InputVoiceChannel2);
            m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputOutputDevice1, m_voiceOutputDevice.get());
            m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputVoiceChannel1, m_channel1.data());
            m_voice->connectVoice(m_audioMixer.get(), IAudioMixer::OutputVoiceChannel2, m_channel2.data());

            m_audioMixer->makeMixerConnection(IAudioMixer::InputVoiceChannel1, IAudioMixer::OutputOutputDevice1);
            m_audioMixer->makeMixerConnection(IAudioMixer::InputVoiceChannel2, IAudioMixer::OutputOutputDevice1);
        }

        CContextAudio::~CContextAudio()
        {
            this->leaveAllVoiceRooms();
        }

        CVoiceRoomList CContextAudio::getComVoiceRoomsWithAudioStatus() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return this->getComVoiceRooms();
        }

        CVoiceRoom CContextAudio::getVoiceRoom(CComSystem::ComUnit comUnitValue, bool withAudioStatus) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << withAudioStatus; }

            const auto voiceChannel = m_voiceChannelMapping.value(comUnitValue);
            return voiceChannel ? voiceChannel->getVoiceRoom() : CVoiceRoom();
        }

        CVoiceRoomList CContextAudio::getComVoiceRooms() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CVoiceRoomList voiceRoomList;

            QSharedPointer<IVoiceChannel> voiceChannelCom1 = m_voiceChannelMapping.value(CComSystem::Com1);
            if (voiceChannelCom1)
            {
                CVoiceRoom room = voiceChannelCom1->getVoiceRoom();
                voiceRoomList.push_back(room);
            }
            else
            {
                voiceRoomList.push_back(CVoiceRoom());
            }

            QSharedPointer<IVoiceChannel> voiceChannelCom2 = m_voiceChannelMapping.value(CComSystem::Com2);
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

        bool CContextAudio::canTalk() const
        {
            const CVoiceRoomList rooms = this->getComVoiceRoomsWithAudioStatus();
            return rooms.countCanTalkTo() > 0;
        }

        void CContextAudio::leaveAllVoiceRooms()
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;}
            m_voiceChannelMapping.clear();
            if (m_channel1)
            {
                m_channel1->leaveVoiceRoom();
                m_unusedVoiceChannels.push_back(m_channel1);
            }
            if (m_channel2)
            {
                m_channel2->leaveVoiceRoom();
                m_unusedVoiceChannels.push_back(m_channel2);
            }
        }

        CIdentifier CContextAudio::audioRunsWhere() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            static const CIdentifier i("CContextAudio");
            return i;
        }

        CAudioDeviceInfoList CContextAudio::getAudioDevices() const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAudioDeviceInfoList devices = m_voiceOutputDevice->getOutputDevices();
            devices.push_back(m_voiceInputDevice->getInputDevices());
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
                emit this->changedSelectedAudioDevices(this->getCurrentAudioDevices());
            }
        }

        void CContextAudio::setVoiceOutputVolume(int volume)
        {
            Q_ASSERT(m_voiceOutputDevice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << volume; }

            const bool wasMuted = isMuted();
            volume = qMin(CSettings::MaxAudioVolume, volume);

            bool changedVoiceOutput = m_voiceOutputDevice->getOutputVolume() != volume;
            if (changedVoiceOutput)
            {
                m_voiceOutputDevice->setOutputVolume(volume);
                m_outVolumeBeforeMute = m_voiceOutputDevice->getOutputVolume();

                emit this->changedAudioVolume(volume);
                if ((volume > 0 && wasMuted) || (volume < 1 && !wasMuted))
                {
                    // inform about muted
                    emit this->changedMute(volume < 1);
                }
            }

            CSettings as(m_audioSettings.getThreadLocal());
            if (as.getAudioVolume() != volume)
            {
                as.setAudioVolume(volume);
                m_audioSettings.set(as);
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
                emit this->changedAudioVolume(newVolume);
            }

            // signal
            emit this->changedMute(muted);
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
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << newRooms; }

            const CVoiceRoomList currentRooms = this->getComVoiceRooms();
            const CVoiceRoom currentRoomCom1  = currentRooms[0];
            const CVoiceRoom currentRoomCom2  = currentRooms[1];
            CVoiceRoom newRoomCom1 = newRooms[0];
            CVoiceRoom newRoomCom2 = newRooms[1];
            const CCallsign ownCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
            const QString id = this->getIContextOwnAircraft()->getOwnAircraft().getPilotId();

            bool changed = false;

            // changed rooms?  But only compare on "URL", not status as connected etc.
            if (currentRoomCom1.getVoiceRoomUrl() != newRoomCom1.getVoiceRoomUrl())
            {
                QSharedPointer<IVoiceChannel> oldVoiceChannel = m_voiceChannelMapping.value(CComSystem::Com1);
                if (oldVoiceChannel)
                {
                    m_voiceChannelMapping.remove(CComSystem::Com1);

                    // If the voice channel is not used by anybody else
                    if (!m_voiceChannelMapping.values().contains(oldVoiceChannel))
                    {
                        oldVoiceChannel->leaveVoiceRoom();
                        m_unusedVoiceChannels.push_back(oldVoiceChannel);
                    }
                    else
                    {
                        emit this->changedVoiceRooms(this->getComVoiceRooms(), false);
                    }
                }

                if (newRoomCom1.isValid())
                {
                    QSharedPointer<IVoiceChannel> newVoiceChannel = this->getVoiceChannelBy(newRoomCom1);
                    newVoiceChannel->setOwnAircraftCallsign(ownCallsign);
                    newVoiceChannel->setUserId(id);
                    const bool inUse = m_voiceChannelMapping.values().contains(newVoiceChannel);
                    m_voiceChannelMapping.insert(CComSystem::Com1, newVoiceChannel);

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
                auto oldVoiceChannel = m_voiceChannelMapping.value(CComSystem::Com2);
                if (oldVoiceChannel)
                {
                    m_voiceChannelMapping.remove(CComSystem::Com2);

                    // If the voice channel is not used by anybody else
                    if (!m_voiceChannelMapping.values().contains(oldVoiceChannel))
                    {
                        oldVoiceChannel->leaveVoiceRoom();
                        m_unusedVoiceChannels.push_back(oldVoiceChannel);
                    }
                    else
                    {
                        emit this->changedVoiceRooms(this->getComVoiceRooms(), false);
                    }
                }

                if (newRoomCom2.isValid())
                {
                    auto newVoiceChannel = getVoiceChannelBy(newRoomCom2);
                    newVoiceChannel->setOwnAircraftCallsign(ownCallsign);
                    newVoiceChannel->setUserId(id);
                    bool inUse = m_voiceChannelMapping.values().contains(newVoiceChannel);
                    m_voiceChannelMapping.insert(CComSystem::Com2, newVoiceChannel);

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

        CCallsignSet CContextAudio::getRoomCallsigns(CComSystem::ComUnit comUnitValue) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }

            const auto voiceChannel = m_voiceChannelMapping.value(comUnitValue);
            return voiceChannel ? voiceChannel->getVoiceRoomCallsigns() : CCallsignSet();
        }

        Network::CUserList CContextAudio::getRoomUsers(CComSystem::ComUnit comUnit) const
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
            const CTime t = m_selcalPlayer->play(90, selcal);
            const int ms = t.toMs();
            if (ms > 10)
            {
                // As of https://dev.swift-project.org/T558 play additional notification
                const QPointer<const CContextAudio> myself(this);
                QTimer::singleShot(ms, this, [ = ]
                {
                    if (!sApp || sApp->isShuttingDown() || !myself) { return; }
                    this->playNotification(CNotificationSounds::NotificationTextMessageSupervisor, true);
                });
            }
        }

        void CContextAudio::playNotification(CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume) const
        {
            Q_ASSERT(m_voice);
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << notification; }

            const CSettings settings = m_audioSettings.getThreadLocal();
            const bool play = !considerSettings || settings.isNotificationFlagSet(notification);
            if (!play) { return; }
            if (notification == CNotificationSounds::PTTClickKeyDown && (considerSettings && settings.noAudioTransmission()))
            {
                if (!this->canTalk())
                {
                    // warning sound
                    notification = CNotificationSounds::NotificationNoAudioTransmission;
                }
            }

            if (volume < 0 || volume > 100)
            {
                volume = 90;
                if (considerSettings) { volume = qMax(25, settings.getNotificationVolume()); }
            }
            m_notificationPlayer.play(notification, volume);
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

        void CContextAudio::setVoiceSetup(const CVoiceSetup &setup)
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_voice) { m_voice->setVoiceSetup(setup); }
        }

        CVoiceSetup CContextAudio::getVoiceSetup() const
        {
            if (m_debugEnabled) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_voice ? m_voice->getVoiceSetup() : CVoiceSetup();
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
                if (v >= 0 && v <= CSettings::MaxAudioVolume)
                {
                    this->setVoiceOutputVolume(v);
                    return true;
                }
            }
            return false;
        }

        void CContextAudio::setVoiceTransmission(bool enable, COM com)
        {
            // FIXME: Use the 'active' channel instead of hardcoded COM1
            // FIXME: Use com
            Q_UNUSED(com);
            if (!m_voiceChannelMapping.contains(CComSystem::Com1)) { return; }
            QSharedPointer<IVoiceChannel> voiceChannelCom1 = m_voiceChannelMapping.value(CComSystem::Com1);
            IAudioMixer::OutputPort mixerOutputPort = m_voiceChannelOutputPortMapping.value(voiceChannelCom1);
            if (enable)
            {
                m_audioMixer->makeMixerConnection(IAudioMixer::InputMicrophone, mixerOutputPort);
            }
            else
            {
                // Remove for both output ports, just in case.
                m_audioMixer->removeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputVoiceChannel1);
                m_audioMixer->removeMixerConnection(IAudioMixer::InputMicrophone, IAudioMixer::OutputVoiceChannel2);
            }
        }

        void CContextAudio::setVoiceTransmissionCom1(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM1);
        }

        void CContextAudio::setVoiceTransmissionCom2(bool enabled)
        {
            this->setVoiceTransmission(enabled, COM2);
        }

        void CContextAudio::setVoiceTransmissionComActive(bool enabled)
        {
            this->setVoiceTransmission(enabled, COMActive);
        }

        void CContextAudio::onConnectionStatusChanged(
            IVoiceChannel::ConnectionStatus oldStatus,
            IVoiceChannel::ConnectionStatus newStatus)
        {
            Q_UNUSED(oldStatus);

            switch (newStatus)
            {
            case IVoiceChannel::Connected:
                emit this->changedVoiceRooms(getComVoiceRooms(), true);
                break;
            case IVoiceChannel::Disconnecting: break;
            case IVoiceChannel::Connecting: break;
            case IVoiceChannel::ConnectingFailed:
            case IVoiceChannel::DisconnectedError:
                CLogMessage(this).warning(u"Voice channel disconnecting error");
                Q_FALLTHROUGH();
            case IVoiceChannel::Disconnected:
                emit this->changedVoiceRooms(getComVoiceRooms(), false);
                break;
            default:
                break;
            }
        }

        void CContextAudio::onUserJoinedRoom(const CCallsign & /**callsign**/)
        {
            emit this->changedVoiceRoomMembers();
        }

        void CContextAudio::onUserLeftRoom(const CCallsign & /**callsign**/)
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

        void CContextAudio::onChangedAudioSettings()
        {
            const CSettings s = m_audioSettings.get();
            const QString dir = s.getNotificationSoundDirectory();
            m_notificationPlayer.updateDirectory(dir);
            this->setVoiceOutputVolume(s.getAudioVolume());
        }

        void CContextAudio::audioIncreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() * 1.2);
            this->setVoiceOutputVolume(v);
        }

        void CContextAudio::audioDecreaseVolume(bool enabled)
        {
            if (!enabled) { return; }
            const int v = qRound(this->getVoiceOutputVolume() / 1.2);
            this->setVoiceOutputVolume(v);
        }

        QSharedPointer<IVoiceChannel> CContextAudio::getVoiceChannelBy(const CVoiceRoom &voiceRoom)
        {
            QSharedPointer<IVoiceChannel> voiceChannel;
            for (const auto &channel : as_const(m_voiceChannelMapping))
            {
                if (channel->getVoiceRoom().getVoiceRoomUrl() == voiceRoom.getVoiceRoomUrl())
                {
                    voiceChannel = channel;
                    break;
                }
            }

            // If we haven't found a valid voice channel pointer, get an unused one
            if (!voiceChannel)
            {
                Q_ASSERT(!m_unusedVoiceChannels.isEmpty());
                voiceChannel = m_unusedVoiceChannels.takeFirst();
            }

            return voiceChannel;
        }


#ifdef Q_OS_MAC
        void CContextAudio::delayedInitMicrophone()
        {
            m_voiceInputDevice = m_voice->createInputDevice();
            m_voice->connectVoice(m_voiceInputDevice.get(), m_audioMixer.get(), IAudioMixer::InputMicrophone);
            CLogMessage(this).info(u"MacOS delayed input device init");
        }
#endif
    } // namespace
} // namespace
