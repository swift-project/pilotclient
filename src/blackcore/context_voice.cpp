/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_voice.h"
#include "context_network.h"
#include "coreruntime.h"
#include "../blacksound/soundgenerator.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Voice;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextVoice::CContextVoice(CCoreRuntime *runtime) : IContextVoice(runtime), m_voice(nullptr)
    {
        Q_ASSERT(runtime);

        // 1. Init by "network driver"
        this->m_voice = new CVoiceVatlib(this);
    }

    /*
     * Cleanup
     */
    CContextVoice::~CContextVoice()
    {
        this->leaveAllVoiceRooms();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoomList CContextVoice::getComVoiceRoomsWithAudioStatus() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getComVoiceRoomsWithAudioStatus();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoom CContextVoice::getCom1VoiceRoom(bool withAudioStatus) const
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
    CVoiceRoom CContextVoice::getCom2VoiceRoom(bool withAudioStatus) const
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
    CVoiceRoomList CContextVoice::getComVoiceRooms() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getComVoiceRooms();
    }

    /*
     * Leave all voice rooms
     */
    void CContextVoice::leaveAllVoiceRooms()
    {
        Q_ASSERT(this->m_voice);
        this->m_voice->leaveAllVoiceRooms();
    }

    /*
     * Audio devices
     */
    CAudioDeviceList CContextVoice::getAudioDevices() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->audioDevices();
    }

    /*
     * Audio default devices
     */
    CAudioDeviceList CContextVoice::getCurrentAudioDevices() const
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
    void CContextVoice::setCurrentAudioDevice(const CAudioDevice &audioDevice)
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
    void CContextVoice::setVolumes(const CComSystem &com1, const CComSystem &com2)
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
    bool CContextVoice::isMuted() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->isMuted();
    }

    /*
     * Set voice rooms
     */
    void CContextVoice::setComVoiceRooms(const CVoiceRoom &voiceRoomCom1, const CVoiceRoom &voiceRoomCom2)
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
    CCallsignList CContextVoice::getCom1RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getVoiceRoomCallsigns(IVoice::COM1);
    }

    /*
     * Room 2 callsigns
     */
    CCallsignList CContextVoice::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getVoiceRoomCallsigns(IVoice::COM2);
    }

    /*
     * Room 1 users
     */
    Network::CUserList CContextVoice::getCom1RoomUsers() const
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
    Network::CUserList CContextVoice::getCom2RoomUsers() const
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
    void CContextVoice::playSelcalTone(const CSelcal &selcal) const
    {
        Q_ASSERT(this->m_voice);
        CAudioDevice outputDevice = m_voice->getCurrentOutputDevice();
        BlackSound::CSoundGenerator::playSelcal(90, selcal, outputDevice);
    }
} // namespace
