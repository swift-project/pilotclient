/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextaudioimpl.h"
#include "blackmisc/dbusserver.h"

#include <QTimer>
#include <QtGlobal>
#include <QPointer>


using namespace BlackMisc;
using namespace BlackMisc::Audio;

namespace BlackCore
{
    namespace Context
    {
        CContextAudio::CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextAudio(mode, runtime)
        { }

        CContextAudio *CContextAudio::registerWithDBus(CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) { return this; }
            server->addObject(IContextAudio::ObjectPath(), this);
            return this;
        }

        void CContextAudio::registerDevices(const CAudioDeviceInfoList &devices)
        {
            if (devices.isEmpty()) { return; }
            m_registeredDevices.registerDevices(devices);
        }

        void CContextAudio::unRegisterDevices(const CAudioDeviceInfoList &devices)
        {
            m_registeredDevices.unRegisterDevices(devices);
        }

        CAudioDeviceInfoList CContextAudio::getRegisteredDevices() const
        {
            return m_registeredDevices;
        }

    } // namespace
} // namespace
