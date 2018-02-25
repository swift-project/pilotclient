/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xswiftbustrafficproxy.h"

#include <QLatin1String>

class QDBusConnection;

#define XSWIFTBUS_SERVICENAME "org.swift-project.xswiftbus"

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXSwiftBusTrafficProxy::CXSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (! dummy) { m_dbusInterface->relayParentSignals(); }
        }

        bool CXSwiftBusTrafficProxy::initialize()
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("initialize"));
        }

        void CXSwiftBusTrafficProxy::cleanup()
        {
            m_dbusInterface->callDBus(QLatin1String("cleanup"));
        }

        bool CXSwiftBusTrafficProxy::loadPlanesPackage(const QString &path)
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("loadPlanesPackage"), path);
        }

        void CXSwiftBusTrafficProxy::setDefaultIcao(const QString &defaultIcao)
        {
            m_dbusInterface->callDBus(QLatin1String("setDefaultIcao"), defaultIcao);
        }

        void CXSwiftBusTrafficProxy::setDrawingLabels(bool drawing)
        {
            m_dbusInterface->callDBus(QLatin1String("setDrawingLabels"), drawing);
        }

        bool CXSwiftBusTrafficProxy::isDrawingLabels() const
        {
            return m_dbusInterface->callDBusRet<bool>(QLatin1String("isDrawingLabels"));
        }

        void CXSwiftBusTrafficProxy::setMaxPlanes(int planes)
        {
            m_dbusInterface->callDBus(QLatin1String("setMaxPlanes"), planes);
        }

        void CXSwiftBusTrafficProxy::setMaxDrawDistance(float nauticalMiles)
        {
            m_dbusInterface->callDBus(QLatin1String("setMaxDrawDistance"), nauticalMiles);
        }

        void CXSwiftBusTrafficProxy::addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlane"), callsign, modelName, aircraftIcao, airlineIcao, livery);
        }

        void CXSwiftBusTrafficProxy::removePlane(const QString &callsign)
        {
            m_dbusInterface->callDBus(QLatin1String("removePlane"), callsign);
        }

        void CXSwiftBusTrafficProxy::removeAllPlanes()
        {
            m_dbusInterface->callDBus(QLatin1String("removeAllPlanes"));
        }

        void CXSwiftBusTrafficProxy::addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime, qint64 timeOffset)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlanePosition"), callsign, latitude, longitude, altitude, pitch, roll, heading, relativeTime, timeOffset);
        }

        void CXSwiftBusTrafficProxy::setPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlanePosition"), callsign, latitude, longitude, altitude, pitch, roll, heading);
        }

        void CXSwiftBusTrafficProxy::addPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround, qint64 relativeTime, qint64 timeOffset)
        {
            m_dbusInterface->callDBus(QLatin1String("addPlaneSurfaces"), callsign, gear, flap, spoiler, speedBrake, slat, wingSweep, thrust, elevator, rudder, aileron,
                landLight, beaconLight, strobeLight, navLight, lightPattern, onGround, relativeTime, timeOffset);
        }

        void CXSwiftBusTrafficProxy::setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlaneSurfaces"), callsign, gear, flap, spoiler, speedBrake, slat, wingSweep, thrust, elevator, rudder, aileron,
                landLight, beaconLight, strobeLight, navLight, lightPattern, onGround);
        }

        void CXSwiftBusTrafficProxy::setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlaneTransponder"), callsign, code, modeC, ident);
        }

        void CXSwiftBusTrafficProxy::setInterpolatorMode(const QString &callsign, bool spline)
        {
            m_dbusInterface->callDBus(QLatin1String("setInterpolatorMode"), callsign, spline);
        }

        void CXSwiftBusTrafficProxy::requestRemoteAircraftData()
        {
            m_dbusInterface->callDBus(QLatin1String("requestRemoteAircraftData"));
        }
    }
}
