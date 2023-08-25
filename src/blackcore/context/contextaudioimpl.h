// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H

#include "blackcore/context/contextaudio.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/userlist.h"

#include <QHash>
#include <QMap>
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <memory>

// clazy:excludeall=const-signal-or-slot

namespace BlackCore
{
    class CCoreFacade;

    namespace Context
    {
        //! Audio context implementation
        class BLACKCORE_EXPORT CContextAudio : public CContextAudioBase
        {
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTAUDIO_INTERFACENAME)
            Q_OBJECT

            friend class BlackCore::CCoreFacade;
            friend class IContextAudio;

        public slots:
            //! \copydoc BlackCore::Context::CContextAudioBase::getRegisteredDevices
            virtual BlackMisc::Audio::CAudioDeviceInfoList getRegisteredDevices() const override;

            //! \copydoc BlackCore::Context::CContextAudioBase::registerDevices
            virtual void registerDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterDevices
            virtual void unRegisterDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterDevicesFor
            virtual void unRegisterDevicesFor(const BlackMisc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::registerAudioCallsign
            virtual void registerAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::unRegisterAudioCallsign
            virtual void unRegisterAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CIdentifier &identifier) override;

            //! \copydoc BlackCore::Context::CContextAudioBase::hasRegisteredAudioCallsign
            virtual bool hasRegisteredAudioCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        protected:
            //! Constructor
            CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextAudio *registerWithDBus(BlackMisc::CDBusServer *server);

        private:
            BlackMisc::Audio::CAudioDeviceInfoList m_registeredDevices;
            QMap<BlackMisc::CIdentifier, BlackMisc::Aviation::CCallsign> m_registeredCallsigns;
        };
    } // namespace
} // namespace

#endif // guard
