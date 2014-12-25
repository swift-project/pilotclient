/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTVOICE_PROXY_H
#define BLACKCORE_CONTEXTVOICE_PROXY_H

#include "context_audio.h"

#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/audiodeviceinfolist.h"
#include "blackmisc/voiceroomlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/avaircraft.h"

namespace BlackCore
{

    //! \brief Audio context proxy
    //! \ingroup dbus
    class CContextAudioProxy : public IContextAudio
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
        friend class IContextAudio;

    public:

        //! Destructor
        virtual ~CContextAudioProxy() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        //! Relay connection signals to local signals
        //! No idea why this has to be wired and is not done automatically
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! Contructor
        CContextAudioProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextAudio(mode, runtime), m_dBusInterface(nullptr) {}

        //! DBus version constructor
        CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    public slots:
        //! \copydoc IContextAudio::getComVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const override;

        //! \copydoc IContextAudio::getComVoiceRoomsWithAudioStatus()
        virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const override;

        //! \copydoc IContextAudio::getCom1VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom1VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::getCom2VoiceRoom
        virtual BlackMisc::Audio::CVoiceRoom getCom2VoiceRoom(bool withAudioStatus) const override;

        //! \copydoc IContextAudio::setComVoiceRooms()
        virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms) override;

        //! \copydoc IContextAudio::getCom1RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom1RoomCallsigns() const override;

        //! \copydoc IContextAudio::getCom2RoomCallsigns()
        virtual BlackMisc::Aviation::CCallsignList getCom2RoomCallsigns() const override;

        //! \copydoc IContextAudio::getCom1RoomUsers()
        virtual BlackMisc::Network::CUserList getCom1RoomUsers() const override;

        //! \copydoc IContextAudio::getCom2RoomUsers()
        virtual BlackMisc::Network::CUserList getCom2RoomUsers() const override;

        //! \copydoc IContextAudio::leaveAllVoiceRooms
        virtual void leaveAllVoiceRooms() override;

        //! \copydoc IContextAudio::getAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override;

        //! \copydoc IContextAudio::getCurrentAudioDevices()
        virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override;

        //! \copydoc IContextAudio::setCurrentAudioDevice()
        virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) override;

        //! \copydoc IContextAudio::setVolumes()
        virtual void setVolumes(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2) override;

        //!\copydoc IContextAudio::setVolumes
        virtual void setVolumes(int com1Volume, int com2Volume) override;

        //! \copydoc IContextAudio::setMute
        virtual void setMute(bool muted) override;

        //! \copydoc IContextAudio::isMuted()
        virtual bool isMuted() const override;

        //! \copydoc IContextAudio::playSelcalTone
        virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;

        //! \copydoc IContextAudio::playNotification
        virtual void playNotification(uint notification, bool considerSettings) const override;

        //! \copydoc IContextAudio::runMicrophoneTest()
        virtual void runMicrophoneTest() override;

        //! \copydoc IContextAudio::runSquelchTest()
        virtual void runSquelchTest() override;

        //! \copydoc IContextAudio::getMicrophoneTestResult()
        virtual QString getMicrophoneTestResult() const override;

        //! \copydoc IContextAudio::getSquelchValue()
        virtual double getSquelchValue() const override;

        //! \copydoc IContextAudio::enableAudioLoopback()
        virtual void enableAudioLoopback(bool enable = true) override;

        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine) override;

    };
}

#endif // guard
