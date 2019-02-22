/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAUDIO_H
#define BLACKCORE_CONTEXT_CONTEXTAUDIO_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/context.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/audio/voicesetup.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/userlist.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc
{
    class CDBusServer;
    namespace Audio { class CAudioDeviceInfo; }
    namespace Aviation { class CCallsign; }
}

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTAUDIO_INTERFACENAME "org.swift_project.blackcore.contextaudio"

//! DBus object path for context
#define BLACKCORE_CONTEXTAUDIO_OBJECTPATH "/audio"

//! @}

namespace BlackCore
{
    namespace Context
    {
        //! Audio context interface
        class BLACKCORE_EXPORT IContextAudio : public CContext
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)

        protected:
            //! Constructor
            IContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : CContext(mode, runtime) {}

        public:
            //! Interface name
            static const QString &InterfaceName();

            //! Object path
            static const QString &ObjectPath();

            //! Hotkey identifier for PTT (push to talk)
            static const QString &pttHotkeyAction();

            //! Hotkey icon for PTT (push to talk)
            static const QPixmap &pttHotkeyIcon();

            //! \copydoc CContext::getPathAndContextId()
            virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

            //! Factory method
            static IContextAudio *create(CCoreFacade *runtime, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

            //! Destructor
            virtual ~IContextAudio() override {}

        signals:
            //! Voice rooms changed
            //! \details the flag indicates, whether a room got connected or disconnected
            void changedVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms, bool connected);

            //! Voice room members changed
            void changedVoiceRoomMembers();

            //! Audio volume changed
            //! \sa setVoiceOutputVolume
            void changedAudioVolume(int volume);

            //! Mute changed
            void changedMute(bool muted);

            //! Changed audio devices (e.g. device enabled/disable)
            void changedAudioDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Changed slection of audio devices
            void changedSelectedAudioDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

        public slots:
            //! Get voice rooms for COM1, COM2:
            virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRoomsWithAudioStatus() const = 0;

            //! Get voice rooms for COM1, COM2, but without latest audio status
            virtual BlackMisc::Audio::CVoiceRoomList getComVoiceRooms() const = 0;

            //! Get voice room per com unit
            virtual BlackMisc::Audio::CVoiceRoom getVoiceRoom(BlackMisc::Aviation::CComSystem::ComUnit comUnit, bool withAudioStatus) const = 0;

            //! Set voice rooms
            virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms) = 0;

            //! Leave all voice rooms
            virtual void leaveAllVoiceRooms() = 0;

            //! Room user callsigns
            virtual BlackMisc::Aviation::CCallsignSet getRoomCallsigns(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const = 0;

            //! Room users
            virtual BlackMisc::Network::CUserList getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnit) const = 0;

            //! Audio devices
            virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const = 0;

            //! Audio runs where
            virtual BlackMisc::CIdentifier audioRunsWhere() const = 0;

            //! Info string about audio
            QString audioRunsWhereInfo() const;

            //! Get current audio device
            //! \return input and output devices
            virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const = 0;

            //! Set current audio device
            //! \param audioDevice can be input or audio device
            virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) = 0;

            //! Set voice output volume (0..300)
            virtual void setVoiceOutputVolume(int volume) = 0;

            //! Voice output volume (0..300)
            virtual int getVoiceOutputVolume() const = 0;

            //! Set mute state
            virtual void setMute(bool mute) = 0;

            //! Is muted?
            virtual bool isMuted() const = 0;

            //! Play SELCAL tone
            virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const = 0;

            //! Play notification sound
            //! \param notification CSoundGenerator::Notification
            //! \param considerSettings consider settings (notification on/off), false means settings ignored
            virtual void playNotification(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification, bool considerSettings) const = 0;

            //! Enable audio loopback
            virtual void enableAudioLoopback(bool enable = true) = 0;

            //! Is loobback enabled?
            virtual bool isAudioLoopbackEnabled() const = 0;

            //! Get voice setup
            virtual BlackMisc::Audio::CVoiceSetup getVoiceSetup() const = 0;

            //! Set voice setup
            virtual void setVoiceSetup(const BlackMisc::Audio::CVoiceSetup &setup) = 0;
        };
    } // ns
} // ns

#endif // guard
