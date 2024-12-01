// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTAUDIO_IMPL_H
#define SWIFT_CORE_CONTEXT_CONTEXTAUDIO_IMPL_H

#include <QMap>
#include <QString>

#include "core/context/contextaudio.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/network/userlist.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::core
{
    class CCoreFacade;

    namespace context
    {
        //! Audio context implementation
        class SWIFT_CORE_EXPORT CContextAudio : public CContextAudioBase
        {
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTAUDIO_INTERFACENAME)
            Q_OBJECT

            friend class swift::core::CCoreFacade;
            friend class IContextAudio;

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

        protected:
            //! Constructor
            CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextAudio *registerWithDBus(swift::misc::CDBusServer *server);

        private:
            swift::misc::audio::CAudioDeviceInfoList m_registeredDevices;
            QMap<swift::misc::CIdentifier, swift::misc::aviation::CCallsign> m_registeredCallsigns;
        };
    } // namespace context
} // namespace swift::core

#endif // SWIFT_CORE_CONTEXT_CONTEXTAUDIO_IMPL_H
