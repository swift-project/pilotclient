/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTAUDIO_IMPL_H

#include "blackcore/context/contextaudio.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/userlist.h"

#include <QHash>
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
            // Interface implementations for DBus
            //! \publicsection
            //! @{
            virtual void registerDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;
            virtual void unRegisterDevices(const BlackMisc::Audio::CAudioDeviceInfoList &devices) override;
            virtual BlackMisc::Audio::CAudioDeviceInfoList getRegisteredDevices() const override;
            //! @}

        protected:
            //! Constructor
            CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextAudio *registerWithDBus(BlackMisc::CDBusServer *server);

        private:
            BlackMisc::Audio::CAudioDeviceInfoList m_registeredDevices;
        };
    } // namespace
} // namespace

#endif // guard
