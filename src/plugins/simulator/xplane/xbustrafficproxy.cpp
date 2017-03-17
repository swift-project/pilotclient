/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xbustrafficproxy.h"

#include <QLatin1String>

class QDBusConnection;

#define XBUS_SERVICENAME "org.swift-project.xbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CXBusTrafficProxy::CXBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (! dummy) { m_dbusInterface->relayParentSignals(); }
        }

        bool CXBusTrafficProxy::initialize()
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("initialize"));
        }

        void CXBusTrafficProxy::cleanup()
        {
            m_dbusInterface->callDBus(QLatin1String("cleanup"));
        }

        bool CXBusTrafficProxy::loadPlanesPackage(const QString &path)
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("loadPlanesPackage"), path);
        }

        void CXBusTrafficProxy::setDefaultIcao(const QString &defaultIcao)
        {
            m_dbusInterface->callDBus(QLatin1String("setDefaultIcao"), defaultIcao);
        }

        void CXBusTrafficProxy::setDrawingLabels(bool drawing)
        {
            m_dbusInterface->callDBus(QLatin1String("setDrawingLabels"), drawing);
        }

        bool CXBusTrafficProxy::isDrawingLabels() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isDrawingLabels"));
        }

        void CXBusTrafficProxy::updateInstalledModels() const
        {
            m_dbusInterface->callDBus(QLatin1String("updateInstalledModels"));
        }

        void CXBusTrafficProxy::setMaxPlanes(int planes)
        {
            m_dbusInterface->callDBus(QLatin1String("setMaxPlanes"), planes);
        }

        void CXBusTrafficProxy::setMaxDrawDistance(float nauticalMiles)
        {
            m_dbusInterface->callDBus(QLatin1String("setMaxDrawDistance"), nauticalMiles);
        }

        void CXBusTrafficProxy::addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlane"), callsign, modelName, aircraftIcao, airlineIcao, livery);
        }

        void CXBusTrafficProxy::removePlane(const QString &callsign)
        {
            m_dbusInterface->callDBus(QLatin1String("removePlane"), callsign);
        }

        void CXBusTrafficProxy::removeAllPlanes()
        {
            m_dbusInterface->callDBus(QLatin1String("removeAllPlanes"));
        }

        void CXBusTrafficProxy::addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlanePosition"), callsign, latitude, longitude, altitude, pitch, roll, heading, relativeTime);
        }

        void CXBusTrafficProxy::addPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround, qint64 relativeTime)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlaneSurfaces"), callsign, gear, flap, spoiler, speedBrake, slat, wingSweep, thrust, elevator, rudder, aileron,
                landLight, beaconLight, strobeLight, navLight, lightPattern, onGround, relativeTime);
        }

        void CXBusTrafficProxy::setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlaneTransponder"), callsign, code, modeC, ident);
        }

        void CXBusTrafficProxy::setInterpolatorMode(const QString &callsign, bool spline)
        {
            m_dbusInterface->callDBus(QLatin1String("setInterpolatorMode"), callsign, spline);
        }

    }
}
