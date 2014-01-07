/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_voice.h"
#include "coreruntime.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Voice;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextVoice::CContextVoice(CCoreRuntime *parent) :
        IContextVoice(parent), m_voice(nullptr), m_currentInputDevice(), m_currentOutputDevice()
    {
        // 1. Init by "network driver"
        this->m_voice = new CVoiceVatlib(this);
        this->m_currentInputDevice = this->m_voice->defaultAudioInputDevice();
        this->m_currentOutputDevice = this->m_voice->defaultAudioOutputDevice();
    }

    /*
     * Cleanup
     */
    CContextVoice::~CContextVoice()
    {
        Q_ASSERT(this->m_voice);
        this->leaveAllVoiceRooms();
    }

    /*
     * Voice rooms for COM
     */
    CVoiceRoomList CContextVoice::getComVoiceRoomsWithAudioStatus()
    {
        Q_ASSERT(this->m_voice);
        return this->m_voice->getComVoiceRoomsWithAudioStatus();
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
        devices.push_back(this->m_currentInputDevice);
        devices.push_back(this->m_currentOutputDevice);
        return devices;
    }

    /*
     * Set current device
     */
    void CContextVoice::setCurrentAudioDevice(const CAudioDevice &audioDevice)
    {
        Q_ASSERT(this->m_voice);
        Q_ASSERT(audioDevice.getType() != CAudioDevice::Unknown);
        this->log(Q_FUNC_INFO, audioDevice.toQString());
        if (audioDevice.getType() == CAudioDevice::InputDevice)
        {
            this->m_voice->setInputDevice(audioDevice);
            this->m_currentInputDevice = audioDevice;
        }
        else
        {
            this->m_voice->setOutputDevice(audioDevice);
            this->m_currentOutputDevice = audioDevice;
        }
    }

    /*
     * Set volumnes
     */
    void CContextVoice::setVolumes(const CComSystem &com1, const CComSystem &com2)
    {
        Q_ASSERT(this->m_voice);
        this->log(Q_FUNC_INFO, com1.toQString(), com2.toQString());
        this->m_voice->setRoomOutputVolume(IVoice::COM1, com1.getVolumeInput());
        this->m_voice->setRoomOutputVolume(IVoice::COM2, com2.getVolumeInput());
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
    QList<CCallsign> CContextVoice::getCom1RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        QSet<QString> signs = this->m_voice->getVoiceRoomCallsings(IVoice::COM1);
        QList<CCallsign> callsigns;
        foreach(QString sign, signs)
        {
            callsigns.append(sign);
        }
        return callsigns;
    }

    /*
     * Room 2 callsigns
     */
    QList<CCallsign> CContextVoice::getCom2RoomCallsigns() const
    {
        Q_ASSERT(this->m_voice);
        QSet<QString> signs = this->m_voice->getVoiceRoomCallsings(IVoice::COM2);
        QList<CCallsign> callsigns;
        foreach(QString sign, signs)
        {
            callsigns.append(sign);
        }
        return callsigns;
    }



} // namespace
