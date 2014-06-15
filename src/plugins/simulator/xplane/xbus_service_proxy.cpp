/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "xbus_service_proxy.h"
#include "blackcore/dbus_server.h"
#include <QMetaMethod>

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CXBusServiceProxy::CXBusServiceProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(BlackCore::CDBusServer::ServiceName, ObjectPath(), InterfaceName(), connection, this);
            if (! dummy) { relaySignals(); }
        }

        void CXBusServiceProxy::relaySignals()
        {
            // TODO can this be refactored into CGenericDBusInterface?
            for (int i = 0, count = metaObject()->methodCount(); i < count; ++i)
            {
                auto method = metaObject()->method(i);
                if (method.methodType() == QMetaMethod::Signal)
                {
                    m_dbusInterface->connection().connect(m_dbusInterface->service(), m_dbusInterface->path(), m_dbusInterface->interface(),
                        method.name(), this, method.methodSignature());
                }
            }
        }

        QString CXBusServiceProxy::getAircraftModelPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelPath"));
        }

        QString CXBusServiceProxy::getAircraftModelFilename() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelFilename"));
        }

        QString CXBusServiceProxy::getAircraftModelLivery() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftModelLivery"));
        }

        QString CXBusServiceProxy::getAircraftIcaoCode() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getAircraftIcaoCode"));
        }

        int CXBusServiceProxy::getXPlaneVersionMajor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMajor"));
        }

        int CXBusServiceProxy::getXPlaneVersionMinor() const
        {
            return m_dbusInterface->callDBusRet<int>(QLatin1String("getXPlaneVersionMinor"));
        }

        QString CXBusServiceProxy::getXPlaneInstallationPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlaneInstallationPath"));
        }

        QString CXBusServiceProxy::getXPlanePreferencesPath() const
        {
            return m_dbusInterface->callDBusRet<QString>(QLatin1String("getXPlanePreferencesPath"));
        }

        double CXBusServiceProxy::getLatitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLatitude"));
        }

        double CXBusServiceProxy::getLongitude() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getLongitude"));
        }

        double CXBusServiceProxy::getAltitudeMSL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getAltitudeMSL"));
        }

        double CXBusServiceProxy::getHeightAGL() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getHeightAGL"));
        }

        double CXBusServiceProxy::getGroundSpeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getGroundSpeed"));
        }

        double CXBusServiceProxy::getIndicatedAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getIndicatedAirspeed"));
        }

        double CXBusServiceProxy::getTrueAirspeed() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueAirspeed"));
        }

        double CXBusServiceProxy::getPitch() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getPitch"));
        }

        double CXBusServiceProxy::getRoll() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getRoll"));
        }

        double CXBusServiceProxy::getTrueHeading() const
        {
            return m_dbusInterface->callDBusRet<double>(QLatin1String("getTrueHeading"));
        }

    }
}