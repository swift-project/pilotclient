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
#include <QDBusConnection>

#define XSWIFTBUS_SERVICENAME "org.swift-project.xswiftbus"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXSwiftBusTrafficProxy::CXSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
        {
            m_dbusInterface = new BlackMisc::CGenericDBusInterface(XSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
            if (!dummy) { m_dbusInterface->relayParentSignals(); }
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

        void CXSwiftBusTrafficProxy::setPlanePositions(const QStringList &callsigns, const QList<double> &latitudes, const QList<double> &longitudes, const QList<double> &altitudes,
                const QList<double> &pitches, const QList<double> &rolles, const QList<double> &headings)
        {
            m_dbusInterface->callDBus(QLatin1String("setPlanePositions"), callsigns, latitudes, longitudes, altitudes, pitches, rolles, headings);
        }

        void CXSwiftBusTrafficProxy::setPlaneSurfaces(const QStringList &callsign, const QList<double> &gear, const QList<double> &flap, const QList<double> &spoiler,
                const QList<double> &speedBrake, const QList<double> &slat, const QList<double> &wingSweep, const QList<double> &thrust,
                const QList<double> &elevator, const QList<double> &rudder, const QList<double> &aileron, const QList<bool> &landLight,
                const QList<bool> &beaconLight, const QList<bool> &strobeLight, const QList<bool> &navLight, const QList<int> &lightPattern, const QList<bool> &onGround)
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

        void CXSwiftBusTrafficProxy::getRemoteAircraftsData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter)
        {
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                QDBusPendingReply<QStringList, QList<double>, QList<double>, QList<double>, QList<double>> reply = *watcher;
                if (!reply.isError())
                {
                    QStringList callsigns = reply.argumentAt<0>();
                    QList<double> latitudesDeg = reply.argumentAt<1>();
                    QList<double> longitudesDeg = reply.argumentAt<2>();
                    QList<double> elevationsM = reply.argumentAt<3>();
                    QList<double> verticalOffsets = reply.argumentAt<4>();
                    setter(callsigns, latitudesDeg, longitudesDeg, elevationsM, verticalOffsets);
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getRemoteAircraftsData"), callback, callsigns);
        }

        void CXSwiftBusTrafficProxy::getEelevationAtPosition(const CCallsign &callsign, double latitude, double longitude, double altitude,
                const ElevationCallback &setter)
        {
            std::function<void(QDBusPendingCallWatcher *)> callback = [ = ](QDBusPendingCallWatcher * watcher)
            {
                QDBusPendingReply<QString, double> reply = *watcher;
                if (!reply.isError())
                {
                    CCallsign cs(reply.argumentAt<0>());
                    double elevationMeters = reply.argumentAt<1>();
                    CAltitude elevationAlt(elevationMeters, CLengthUnit::m());
                    elevationAlt.switchUnit(CLengthUnit::ft());
                    CElevationPlane elevation(CLatitude(latitude, CAngleUnit::deg()),
                                              CLongitude(longitude, CAngleUnit::deg()),
                                              elevationAlt);
                    elevation.setSinglePointRadius();
                    setter(elevation, cs);
                }
                watcher->deleteLater();
            };
            m_dbusInterface->callDBusAsync(QLatin1String("getEelevationAtPosition"), callback, callsign.asString(), latitude, longitude, altitude);
        }

        void CXSwiftBusTrafficProxy::setFollowedAircraft(const QString &callsign)
        {
            m_dbusInterface->callDBus(QLatin1String("setFollowedAircraft"), callsign);
        }
    }
}
