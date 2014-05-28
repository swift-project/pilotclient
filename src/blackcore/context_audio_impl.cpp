/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_audio_impl.h"
#include "context_network.h"
#include "context_ownaircraft.h"
#include "context_application.h"

#include "blacksound/soundgenerator.h"
#include "blackmisc/notificationsounds.h"
#include "blackmisc/voiceroomlist.h"

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
        this->m_voice = new CVoiceVatlib();
        m_voice->moveToThread(&m_threadVoice);
        m_threadVoice.start();

        // 2. Hotkeys
        m_keyboard = IKeyboard::getInstance();

        // 3. own aircraft, if possible
        if (this->getIContextOwnAircraft()) m_voice->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());

        // 4. Signal / slots
        connect(this->m_voice, &CVoiceVatlib::micTestFinished, this, &CContextAudio::audioTestCompleted);
        connect(this->m_voice, &CVoiceVatlib::squelchTestFinished, this, &CContextAudio::audioTestCompleted);
        connect(this->m_voice, &CVoiceVatlib::connectionStatusChanged, this, &CContextAudio::connectionStatusChanged);
        if (this->getIContextApplication()) this->connect(this->m_voice, &IVoice::statusMessage, this->getIContextApplication(), &IContextApplication::sendStatusMessage);
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_voice->getComVoiceRoomsWithAudioStatus();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom1VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, withAudioStatus);
        if (withAudioStatus)
            return this->m_voice->getComVoiceRoomsWithAudioStatus()[0];
        else
            return this->m_voice->getComVoiceRooms()[0];
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextAudio::getCom2VoiceRoom(bool withAudioStatus) const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, withAudioStatus);
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_voice->getComVoiceRooms();
    }

    /*
     * Leave all voice rooms
     */
    void CContextAudio::leaveAllVoiceRooms()
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        this->m_voice->leaveAllVoiceRooms();
    }

    /*
     * Audio devices
     */
    CAudioDeviceList CContextAudio::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_voice->audioDevices();
    }

    /*
     * Audio default devices
     */
    CAudioDeviceList CContextAudio::getCurrentAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, audioDevice.toQString());
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, com1.toQString(), com2.toQString());
        qint32 vol1 = com1.getVolumeOutput();
        qint32 vol2 = com2.getVolumeOutput();
        this->m_voice->setRoomOutputVolume(IVoice::COM1, vol1);
        this->m_voice->setRoomOutputVolume(IVoice::COM2, vol2);
        this->m_voice->switchAudioOutput(IVoice::COM1, com1.isEnabled());
        this->m_voice->switchAudioOutput(IVoice::COM2, com2.isEnabled());
    }

    /*
     * Muted?
     */
    bool CContextAudio::isMuted() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_voice->isMuted();
    }

    /*
     * Set voice rooms
     */
    void CContextAudio::setComVoiceRooms(const CVoiceRoomList &newRooms)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(newRooms.size() == 2);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, newRooms.toQString());
        CVoiceRoomList currentRooms =  this->m_voice->getComVoiceRooms();
        CVoiceRoom currentRoom1 = currentRooms[0];
        CVoiceRoom currentRoom2 = currentRooms[1];
        CVoiceRoom newRoom1 = newRooms[0];
        CVoiceRoom newRoom2 = newRooms[1];

        bool changed = false;

        // changed rooms?  But only compare on "URL",  not status as connected etc.
        if (currentRoom1.getVoiceRoomUrl() != newRoom1.getVoiceRoomUrl())
        {
            this->m_voice->leaveVoiceRoom(IVoice::COM1);
            if (newRoom1.isValid()) this->m_voice->joinVoiceRoom(IVoice::COM1, newRoom1);
            changed = true;
        }
        if (currentRoom2.getVoiceRoomUrl() != newRoom2.getVoiceRoomUrl())
        {
            this->m_voice->leaveVoiceRoom(IVoice::COM2);
            if (newRoom2.isValid()) this->m_voice->joinVoiceRoom(IVoice::COM2, newRoom2);
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        CCallsignList callsigns = this->m_voice->getVoiceRoomCallsigns(IVoice::COM1);
        qDebug() << "1" << callsigns;
        return callsigns;
    }

    /*
     * Room 2 callsigns
     */
    CCallsignList CContextAudio::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        CCallsignList callsigns = this->m_voice->getVoiceRoomCallsigns(IVoice::COM2);
        qDebug() << "2" << callsigns;
        return callsigns;
    }

    /*
     * Room 1 users
     */
    Network::CUserList CContextAudio::getCom1RoomUsers() const
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(this->getRuntime());
        if (!this->getRuntime()->getIContextNetwork()) return Network::CUserList();
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
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
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->getIContextNetwork()->getUsersForCallsigns(this->getCom2RoomCallsigns());
    }

    /*
     * SELCAL tone
     */
    void CContextAudio::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, selcal.toQString());
        CAudioDevice outputDevice = m_voice->getCurrentOutputDevice();
        BlackSound::CSoundGenerator::playSelcal(90, selcal, outputDevice);
    }

    /*
     * Notification
     */
    void CContextAudio::playNotification(uint notification) const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, QString::number(notification));
        BlackSound::CSoundGenerator::playNotificationSound(90, static_cast<BlackSound::CNotificationSounds::Notification>(notification));
    }

    /*
     * Mic test.
     */
    void CContextAudio::runMicrophoneTest()
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        this->m_voice->runMicrophoneTest();
    }

    /*
     * Squelch test.
     */
    void CContextAudio::runSquelchTest()
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        this->m_voice->runSquelchTest();
    }

    /*
     * Microphone test
     */
    QString CContextAudio::getMicrophoneTestResult() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_voice->micTestResultAsString();
    }

    /*
     * Squelch value
     */
    double CContextAudio::getSquelchValue() const
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return static_cast<double>(this->m_voice->inputSquelch());
    }

    void CContextAudio::enableAudioLoopback(bool enable)
    {
        Q_ASSERT(this->m_voice);
        if (this->getRuntime()->isSlotLogForAudioEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        m_voice->enableAudioLoopback(enable);
    }

    /*
     * Settings changed
     */
    void CContextAudio::settingsChanged(uint typeValue)
    {
        if (this->getIContextOwnAircraft()) m_voice->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
        if (!this->getIContextSettings()) return;
        IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
        if (type == IContextSettings::SettingsHotKeys)
        {
            CKeyboardKeyList hotKeys = this->getIContextSettings()->getHotkeys();
            Q_ASSERT(!hotKeys.isEmpty());
            CKeyboardKey pttKey = hotKeys.findBy(&BlackMisc::Hardware::CKeyboardKey::getFunction, BlackMisc::Hardware::CKeyboardKey::HotkeyPtt).front();
            m_keyboard->unregisterHotkey(m_handlePtt);
            m_handlePtt = m_keyboard->registerHotkey(pttKey, m_voice, &CVoiceVatlib::handlePushToTalk);
        }
    }

    /*
     * Connection status changed
     */
    void CContextAudio::connectionStatusChanged(IVoice::ComUnit comUnit, IVoice::ConnectionStatus oldStatus, IVoice::ConnectionStatus newStatus)
    {
        Q_UNUSED(comUnit);
        Q_UNUSED(oldStatus);

        switch (newStatus)
        {
        case IVoice::Connected:
            emit this->changedVoiceRooms(this->m_voice->getComVoiceRooms());
            break;
        case IVoice::Disconnecting:
            emit this->changedVoiceRooms(this->m_voice->getComVoiceRooms());
            // good chance to update aircraft
            if (this->getIContextOwnAircraft()) m_voice->setMyAircraftCallsign(this->getIContextOwnAircraft()->getOwnAircraft().getCallsign());
            break;
        default:
            break;
        }
    }

} // namespace
