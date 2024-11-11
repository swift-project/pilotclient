// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTVOICE_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTVOICE_PROXY_H

#include <QObject>
#include <QString>

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/corefacadeconfig.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/audio/notificationsounds.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/identifier.h"
#include "misc/network/userlist.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
    namespace Audio
    {
        class CAudioDeviceInfo;
    }
    namespace aviation
    {
        class CCallsign;
    }
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
            //! \copydoc BlackCore::Context::CContextAudioBase::getRegisteredDevices
            virtual swift::misc::audio::CAudioDeviceInfoList getRegisteredDevices() const override;

            //! \copydoc BlackCore::Context::CContextAudioBase::registerDevices
            virtual void registerDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterDevices
            virtual void unRegisterDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterDevicesFor
            virtual void unRegisterDevicesFor(const swift::misc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::registerAudioCallsign
            virtual void registerAudioCallsign(const swift::misc::aviation::CCallsign &callsign, const swift::misc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterAudioCallsign
            virtual void unRegisterAudioCallsign(const swift::misc::aviation::CCallsign &callsign, const swift::misc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::hasRegisteredAudioCallsign
            virtual bool hasRegisteredAudioCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface;

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
