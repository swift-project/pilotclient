/* Copyright (C) 2020
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

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
            CDuplexProxy::CDuplexProxy(const QDBusConnection &connection, const QString &service, QObject *parent) : IDuplex(parent)
            {
                const QString path = CIdentifier::anonymous().toDBusObjectPath(BLACKMISC_DUPLEX_PATH_ROOT);
                m_interface = new CGenericDBusInterface(service, path, BLACKMISC_DUPLEX_INTERFACE, connection, this);
                m_interface->relayParentSignals();
            }

            void CDuplexProxy::postEvent(const QString &channel, const CVariant &param)
            {
                m_interface->callDBus(QLatin1String("postEvent"), channel, param);
            }

            void CDuplexProxy::setSubscription(const QString &channel, const CVariantList &filters)
            {
                m_interface->callDBus(QLatin1String("setSubscription"), channel, filters);
            }

            void CDuplexProxy::requestPeerSubscriptions()
            {
                m_interface->callDBus(QLatin1String("requestPeerSubscriptions"));
            }

            void CDuplexProxy::submitRequest(const QString &channel, const CVariant &param, quint32 token)
            {
                m_interface->callDBus(QLatin1String("submitRequest"), channel, param, token);
            }

            void CDuplexProxy::reply(const CVariant &param, quint32 token)
            {
                m_interface->callDBus(QLatin1String("reply"), param, token);
            }

            void CDuplexProxy::advertise(const QString &channel)
            {
                m_interface->callDBus(QLatin1String("advertise"), channel);
            }

            void CDuplexProxy::withdraw(const QString &channel)
            {
                m_interface->callDBus(QLatin1String("withdraw"), channel);
            }
        }
    }
}
