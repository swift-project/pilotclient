/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio_impl.h"
#include "context_network.h"

#include "blacksound/soundgenerator.h"

#include <QTimer>


using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Hardware;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextAudio::CContextAudio(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextAudio(mode, runtime),
        m_voice(nullptr),
        m_keyboard(nullptr)
    {
        // 1. Init by "voice driver"
        this->m_voice = new CVoiceVatlib(this);
        m_keyboard = IKeyboard::getInstance();

        // 2. Signal / slots
        connect(this->m_voice, &CVoiceVatlib::micTestFinished, this, &CContextAudio::audioTestCompleted);
        connect(this->m_voice, &CVoiceVatlib::squelchTestFinished, this, &CContextAudio::audioTestCompleted);
    }

    /*
     * Cleanup
     */
    CContextAudio::~CContextAudio()
    {
        this->leaveAllVoiceRooms();
    }

    void CContextAudio::init()
    {
        m_contextSettings = getRuntime()->getIContextSettings();
        connect(m_contextSettings, &IContextSettings::changedSettings, this, &CContextAudio::settingsChanged);
    }

    /*
     * Own aircraft
     */
    void CContextAudio::setOwnAircraft(const CAircraft &ownAircraft)
    {
        Q_ASSERT(this->m_voice);
        this->m_voice->setMyAircraftCallsign(ownAircraft.getCallsign());
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoomList CContextAudio::getComVoiceRoomsWithAudioStatus() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getComVoiceRoomsWithAudioStatus();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom1VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        if (withAudioStatus)
            return this->m_voice->getComVoiceRoomsWithAudioStatus()[0];
        else
            return this->m_voice->getComVoiceRooms()[1];
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom2VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        if (withAudioStatus)
            return this->m_voice->getComVoiceRoomsWithAudioStatus()[1];
        else
            return this->m_voice->getComVoiceRooms()[1];
    }

    /*
     * Voice rooms for COM (const)
     */
    CVoiceRoomList CContextAudio::getComVoiceRooms() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getComVoiceRooms();
    }

    /*
     * Leave all voice rooms
     */
    void CContextAudio::leaveAllVoiceRooms()
    {
        Q_ASSERT(this->m_voice);
        this->m_voice->leaveAllVoiceRooms();
    }

    /*
     * Audio devices
     */
    CAudioDeviceList CContextAudio::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->audioDevices();
    }

    /*
     * Audio default devices
     */
    CAudioDeviceList CContextAudio::getCurrentAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
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
        this->m_voice->setRoomOutputVolume(IVoice::COM1, com1.getVolumeOutput());
        this->m_voice->setRoomOutputVolume(IVoice::COM2, com2.getVolumeOutput());
        this->m_voice->switchAudioOutput(IVoice::COM1, com1.isEnabled());
        this->m_voice->switchAudioOutput(IVoice::COM2, com2.isEnabled());
    }

    /*
     * Muted?
     */
    bool CContextAudio::isMuted() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->isMuted();
    }

    /*
     * Set voice rooms
     */
    void CContextAudio::setComVoiceRooms(const CVoiceRoom &voiceRoomCom1, const CVoiceRoom &voiceRoomCom2)
    {
        Q_ASSERT(this->m_voice);
        CVoiceRoomList currentRooms =  this->m_voice->getComVoiceRoomsWithAudioStatus();
        CVoiceRoom currentRoom1 = currentRooms[0];
        CVoiceRoom currentRoom2 = currentRooms[1];
        if (currentRoom1 != voiceRoomCom1)
        {
            if (currentRoom1.isValid()) this->m_voice->leaveVoiceRoom(IVoice::COM1);
            if (voiceRoomCom1.isValid()) this->m_voice->joinVoiceRoom(IVoice::COM1, voiceRoomCom1);
        }
        if (currentRoom2 != voiceRoomCom2)
        {
            if (currentRoom2.isValid()) this->m_voice->leaveVoiceRoom(IVoice::COM2);
            if (voiceRoomCom2.isValid()) this->m_voice->joinVoiceRoom(IVoice::COM2, voiceRoomCom2);
        }
    }

    /*
     * Room 1 callsigns
     */
    CCallsignList CContextAudio::getCom1RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getVoiceRoomCallsigns(IVoice::COM1);
    }

    /*
     * Room 2 callsigns
     */
    CCallsignList CContextAudio::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getVoiceRoomCallsigns(IVoice::COM2);
    }

    /*
     * Room 1 users
     */
    Network::CUserList CContextAudio::getCom1RoomUsers() const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextNetwork());
        return this->getRuntime()->getIContextNetwork()->
               getUsersForCallsigns(this->getCom1RoomCallsigns());
    }

    /*
     * Room 2 users
     */
    Network::CUserList CContextAudio::getCom2RoomUsers() const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextNetwork());
        return this->getRuntime()->getIContextNetwork()->
               getUsersForCallsigns(this->getCom2RoomCallsigns());
    }

    /*
     * SELCAL tone
     */
    void CContextAudio::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        CAudioDevice outputDevice = m_voice->getCurrentOutputDevice();
        BlackSound::CSoundGenerator::playSelcal(90, selcal, outputDevice);
    }

    /*
     * Notification
     */
    void CContextAudio::playNotification(uint notification) const
    {
        Q_ASSERT(this->m_voice);
        BlackSound::CSoundGenerator::playNotificationSound(90, static_cast<BlackSound::CSoundGenerator::Notification>(notification));
    }

    /*
     * Mic test.
     */
    void CContextAudio::runMicrophoneTest()
    {
        Q_ASSERT(this->m_voice);
        this->m_voice->runMicrophoneTest();
    }

    /*
     * Squelch test.
     */
    void CContextAudio::runSquelchTest()
    {
        Q_ASSERT(this->m_voice);
        this->m_voice->runSquelchTest();
    }

    /*
     * Microphone test
     */
    QString CContextAudio::getMicrophoneTestResult() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->micTestResultAsString();
    }

    /*
     * Squelch value
     */
    double CContextAudio::getSquelchValue() const
    {
        Q_ASSERT(this->m_voice);
        return static_cast<double>(this->m_voice->inputSquelch());
    }

    void CContextAudio::settingsChanged(uint typeValue)
    {
        if (!this->getRuntime()->getIContextSettings()) return;
        IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
        if (type == IContextSettings::SettingsHotKeys)
        {
            CKeyboardKeyList hotKeys = this->getRuntime()->getIContextSettings()->getHotkeys();
            CKeyboardKey pttKey = hotKeys.findBy(&BlackMisc::Hardware::CKeyboardKey::getFunction, BlackMisc::Hardware::CKeyboardKey::HotkeyPtt).front();
            m_keyboard->unregisterHotkey(m_handlePtt);
            m_handlePtt = m_keyboard->registerHotkey(pttKey, m_voice, &CVoiceVatlib::handlePushToTalk);
        }
    }

} // namespace
