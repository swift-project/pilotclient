/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#include "blackmisc/sharedstate/dbus/hubproxy.h"
#include "blackmisc/sharedstate/dbus/duplexproxy.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/identifier.h"

namespace BlackMisc
{
    namespace SharedState
    {
        namespace DBus
        {
            CHubProxy::CHubProxy(const QDBusConnection &connection, const QString &service, QObject* parent) : IHub(parent), m_service(service)
            {
                m_interface = new CGenericDBusInterface(service, BLACKMISC_HUB_PATH, BLACKMISC_HUB_INTERFACE, connection, this);
                m_interface->relayParentSignals();
            }

            bool CHubProxy::isConnected() const
            {
                return m_interface->isValid();
            }

            std::pair<QSharedPointer<IDuplex>, QFuture<void>> CHubProxy::getDuplex()
            {
                auto duplex = QSharedPointer<CDuplexProxy>::create(m_interface->connection(), m_service, this);
                connect(duplex.get(), &QObject::destroyed, this, [ = ] { closeDuplex(CIdentifier::anonymous()); });
                return std::make_pair(duplex, openDuplexAsync(CIdentifier::anonymous()));
            }

            bool CHubProxy::openDuplex(const CIdentifier& client)
            {
                return m_interface->callDBusRet<bool>(QLatin1String("openDuplex"), client);
            }

            void CHubProxy::closeDuplex(const CIdentifier& client)
            {
                m_interface->callDBus(QLatin1String("closeDuplex"), client);
            }

            QFuture<void> CHubProxy::openDuplexAsync(const CIdentifier &client)
            {
                return m_interface->callDBusFuture<bool>(QLatin1String("openDuplex"), client);
            }

            CHubProxy::~CHubProxy()
            {
                closeDuplex(CIdentifier::anonymous());
            }
        }
    }
}
