/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTVOICE_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTVOICE_PROXY_H

#include <QObject>
#include <QString>

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/userlist.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Audio { class CAudioDeviceInfo; }
    namespace Aviation { class CCallsign; }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Audio context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextAudioProxy : public IContextAudio
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
            friend class IContextAudio;

        public:
            //! Destructor
            virtual ~CContextAudioProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            // interface overrides
            //! \publicsection
            //! @{
            virtual void setComVoiceRooms(const BlackMisc::Audio::CVoiceRoomList &voiceRooms) override;
            virtual BlackMisc::Network::CUserList getRoomUsers(BlackMisc::Aviation::CComSystem::ComUnit comUnitValue) const override;
            virtual void leaveAllVoiceRooms() override;
            virtual BlackMisc::CIdentifier audioRunsWhere() const override;
            virtual BlackMisc::Audio::CAudioDeviceInfoList getAudioDevices() const override;
            virtual BlackMisc::Audio::CAudioDeviceInfoList getCurrentAudioDevices() const override;
            virtual void setCurrentAudioDevice(const BlackMisc::Audio::CAudioDeviceInfo &audioDevice) override;
            virtual void setVoiceOutputVolume(int volume) override;
            virtual int getVoiceOutputVolume() const override;
            virtual void setMute(bool muted) override;
            virtual bool isMuted() const override;
            virtual void playSelcalTone(const BlackMisc::Aviation::CSelcal &selcal) const override;
            virtual void playNotification(BlackMisc::Audio::CNotificationSounds::NotificationFlag notification, bool considerSettings, int volume = -1) const override;
            virtual void enableAudioLoopback(bool enable = true) override;
            virtual bool isAudioLoopbackEnabled() const override;
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            virtual BlackMisc::Audio::CVoiceSetup getVoiceSetup() const override;
            virtual void setVoiceSetup(const BlackMisc::Audio::CVoiceSetup &setup) override;
            //! @}

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface;

            //! Relay connection signals to local signals
            //! No idea why this has to be wired and is not done automatically
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Contructor
            CContextAudioProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextAudio(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard
