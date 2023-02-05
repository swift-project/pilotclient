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
    namespace Audio    { class CAudioDeviceInfo; }
    namespace Aviation { class CCallsign; }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Audio context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextAudioProxy : public CContextAudioBase
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
            virtual BlackMisc::Audio::CAudioDeviceInfoList getRegisteredDevices() const override;
            virtual void registerDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;
            virtual void unRegisterDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;
            virtual void unRegisterDevicesFor(const BlackMisc::CIdentifier &identifier) override;
            virtual void registerAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign,  const BlackMisc::CIdentifier &identifier)   override;
            virtual void unRegisterAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign,  const BlackMisc::CIdentifier &identifier) override;
            virtual bool hasRegisteredAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface;

            //! Relay connection signals to local signals
            //! No idea why this has to be wired and is not done automatically
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Contructor
            CContextAudioProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : CContextAudioBase(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextAudioProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns

#endif // guard
