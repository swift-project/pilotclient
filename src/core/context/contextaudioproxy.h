// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTVOICE_PROXY_H
#define SWIFT_CORE_CONTEXT_CONTEXTVOICE_PROXY_H

#include <QString>

#include "core/context/contextaudio.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/audio/notificationsounds.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/comsystem.h"
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
} // namespace swift::misc

namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Audio context proxy
        //! \ingroup dbus
        class SWIFT_CORE_EXPORT CContextAudioProxy : public CContextAudioBase
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTAUDIO_INTERFACENAME)
            friend class IContextAudio;

        public:
            //! Destructor
            virtual ~CContextAudioProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \copydoc swift::core::context::CContextAudioBase::getRegisteredDevices
            virtual swift::misc::audio::CAudioDeviceInfoList getRegisteredDevices() const override;

            //! \copydoc swift::core::context::CContextAudioBase::registerDevices
            virtual void registerDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc swift::core::context::CContextAudioBase::unRegisterDevices
            virtual void unRegisterDevices(const swift::misc::audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc swift::core::context::CContextAudioBase::unRegisterDevicesFor
            virtual void unRegisterDevicesFor(const swift::misc::CIdentifier &identifier) override;

            //! \copydoc swift::core::context::CContextAudioBase::registerAudioCallsign
            virtual void registerAudioCallsign(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::CIdentifier &identifier) override;

            //! \copydoc swift::core::context::CContextAudioBase::unRegisterAudioCallsign
            virtual void unRegisterAudioCallsign(const swift::misc::aviation::CCallsign &callsign,
                                                 const swift::misc::CIdentifier &identifier) override;

            //! \copydoc swift::core::context::CContextAudioBase::hasRegisteredAudioCallsign
            virtual bool hasRegisteredAudioCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface;

            //! Relay connection signals to local signals
            //! No idea why this has to be wired and is not done automatically
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Contructor
            CContextAudioProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
                : CContextAudioBase(mode, runtime), m_dBusInterface(nullptr)
            {}

            //! DBus version constructor
            CContextAudioProxy(const QString &serviceName, QDBusConnection &connection,
                               CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // namespace context
} // namespace swift::core

#endif // guard
