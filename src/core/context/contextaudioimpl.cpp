// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextaudioimpl.h"

#include <QPointer>
#include <QTimer>
#include <QtGlobal>

#include "core/afv/clients/afvclient.h"
#include "misc/dbusserver.h"

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::misc::aviation;
using namespace swift::core::afv::clients;

namespace swift::core::context
{
    CContextAudio::CContextAudio(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
        : CContextAudioBase(mode, runtime)
    {
        connect(this, &CContextAudio::changedLocalAudioDevices, this, &CContextAudio::onChangedLocalDevices,
                Qt::QueuedConnection);
    }

    CContextAudio *CContextAudio::registerWithDBus(CDBusServer *server)
    {
        if (!server || getMode() != CCoreFacadeConfig::LocalInDBusServer) { return this; }

        // remark that registers all SIGNALS, not only the interface ons
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

    void CContextAudio::unRegisterDevicesFor(const CIdentifier &identifier)
    {
        m_registeredDevices.unRegisterDevices(identifier);
    }

    void CContextAudio::registerAudioCallsign(const CCallsign &callsign, const CIdentifier &identifier)
    {
        m_registeredCallsigns.insert(identifier, callsign);
    }

    void CContextAudio::unRegisterAudioCallsign(const CCallsign &callsign, const CIdentifier &identifier)
    {
        m_registeredCallsigns.remove(identifier);
        Q_UNUSED(callsign)
    }

    bool CContextAudio::hasRegisteredAudioCallsign(const CCallsign &callsign) const
    {
        for (const CCallsign &cs : m_registeredCallsigns.values())
        {
            if (callsign == cs) { return true; }
        }
        return false;
    }

    CAudioDeviceInfoList CContextAudio::getRegisteredDevices() const { return m_registeredDevices; }

} // namespace swift::core::context
