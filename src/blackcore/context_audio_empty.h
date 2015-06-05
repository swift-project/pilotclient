/* Copyright (C) 2015
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTAUDIO_EMPTY_H
#define BLACKCORE_CONTEXTAUDIO_EMPTY_H

#include "blackcoreexport.h"
#include "context_audio.h"
#include "dbus_server.h"
#include "voice.h"

namespace BlackCore
{
    class IVoiceChannel;

    //! Audio context implementation
    class BLACKCORE_EXPORT CContextAudioEmpty : public IContextAudio
    {
    public:

        //! Constructor
        CContextAudioEmpty(CRuntime *runtime) : IContextAudio(CRuntimeConfig::NotUsed, runtime) {}

    public slots:
        //! \copydoc IContextAudio::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CVoiceRoomList();
        }

        //! \copydoc IContextAudio::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CVoiceRoomList();
        }

        //! \copydoc IContextAudio::getVoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom(int comUnitValue, bool withAudioStatus) const override
        {
            Q_UNUSED(comUnitValue);
            Q_UNUSED(withAudioStatus);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CVoiceRoom();
        }

        //! \copydoc IContextAudio::setComVoiceRooms
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &newRooms) override
        {
            Q_UNUSED(newRooms);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::setOwnCallsignForRooms
        virtual void setOwnCallsignForRooms(const BlackMisc::Aviation::CCallsign &callsign) override
        {
            Q_UNUSED(callsign);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::getRoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignSet getRoomCallsigns(int comUnitValue) const override
        {
            Q_UNUSED(comUnitValue);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Aviation::CCallsignSet();
        }

        //! \copydoc IContextAudio::getRoomUsers()
        virtual BlackMisc::Network::CUserList getRoomUsers(int comUnitValue) const override
        {
            Q_UNUSED(comUnitValue);
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Network::CUserList();
        }

        //! \copydoc IContextAudio::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CAudioDeviceInfoList();
        }

        //! \copydoc IContextAudio::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return BlackMisc::Audio::CAudioDeviceInfoList();
        }

        //! \copydoc IContextAudio::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) override
        {
            Q_UNUSED(audioDevice);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContext::setVoiceOutputVolume
        virtual void setVoiceOutputVolume(int volume) override
        {
            Q_UNUSED(volume);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContext::getVoiceOutputVolume
        virtual int getVoiceOutputVolume() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return 0;
        }

        //! \copydoc ICOntext::setMute
        virtual void setMute(bool muted) override
        {
            Q_UNUSED(muted);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::isMuted()
        virtual bool isMuted() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextAudio::playSelcalTone()
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override
        {
            Q_UNUSED(selcal);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::playNotification()
        virtual void playNotification(uint notification, bool considerSettings) const override
        {
            Q_UNUSED(notification);
            Q_UNUSED(considerSettings);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc IContextAudio::enableAudioLoopback()
        virtual void enableAudioLoopback(bool enable = true) override
        {
            Q_UNUSED(enable);
            logEmptyContextWarning(Q_FUNC_INFO);
        }

        //! \copydoc ICOntextAudio::isAudioLoopbackEnabled
        virtual bool isAudioLoopbackEnabled() const override
        {
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

        //! \copydoc IContextAudio::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override
        {
            Q_UNUSED(commandLine);
            Q_UNUSED(originator);
            logEmptyContextWarning(Q_FUNC_INFO);
            return false;
        }

    };
} // namespace

#endif // guard
