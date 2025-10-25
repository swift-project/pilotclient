// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "xswiftbustrafficproxy.h"

#include <cmath>

#include <QDBusConnection>
#include <QLatin1String>

#include "misc/logmessage.h"

#define XSWIFTBUS_SERVICENAME "org.swift-project.xswiftbus"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::simplugin::xplane
{
    const QStringList &CXSwiftBusTrafficProxy::getLogCategories()
    {
        static const QStringList cats { CLogCategories::driver(), CLogCategories::dbus() };
        return cats;
    }

    CXSwiftBusTrafficProxy::CXSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy)
        : QObject(parent)
    {
        m_dbusInterface = new swift::misc::CGenericDBusInterface(XSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(),
                                                                 connection, this);
        if (!dummy)
        {
            bool s {};
            s = connection.connect(QString(), "/xswiftbus/traffic", "org.swift_project.xswiftbus.traffic", "simFrame",
                                   this, SIGNAL(simFrame()));
            Q_ASSERT(s);

            s = connection.connect(QString(), "/xswiftbus/traffic", "org.swift_project.xswiftbus.traffic",
                                   "remoteAircraftAdded", this, SIGNAL(remoteAircraftAdded(QString)));
            Q_ASSERT(s);

            s = connection.connect(QString(), "/xswiftbus/traffic", "org.swift_project.xswiftbus.traffic",
                                   "remoteAircraftAddingFailed", this, SIGNAL(remoteAircraftAddingFailed(QString)));
            Q_ASSERT(s);
        }
    }

    MultiplayerAcquireInfo CXSwiftBusTrafficProxy::acquireMultiplayerPlanes()
    {
        QDBusPendingReply<bool, QString> reply = m_dbusInterface->asyncCall(QLatin1String("acquireMultiplayerPlanes"));
        reply.waitForFinished();
        if (reply.isError())
        {
            CLogMessage(this).debug(u"CXSwiftBusTrafficProxy::acquireMultiplayerPlanes returned: %1")
                << reply.error().message();
        }
        MultiplayerAcquireInfo info;
        info.hasAcquired = reply.argumentAt<0>();
        info.owner = reply.argumentAt<1>();
        return info;
    }

    bool CXSwiftBusTrafficProxy::initialize()
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("initialize"));
    }

    void CXSwiftBusTrafficProxy::cleanup() { m_dbusInterface->callDBus(QLatin1String("cleanup")); }

    QString CXSwiftBusTrafficProxy::loadPlanesPackage(const QString &path)
    {
        return m_dbusInterface->callDBusRet<QString>(QLatin1String("loadPlanesPackage"), path);
    }

    void CXSwiftBusTrafficProxy::setDefaultIcao(const QString &defaultIcao)
    {
        m_dbusInterface->callDBus(QLatin1String("setDefaultIcao"), defaultIcao);
    }

    void CXSwiftBusTrafficProxy::setMaxPlanes(int planes)
    {
        m_dbusInterface->callDBus(QLatin1String("setMaxPlanes"), planes);
    }

    void CXSwiftBusTrafficProxy::setMaxDrawDistance(double nauticalMiles)
    {
        m_dbusInterface->callDBus(QLatin1String("setMaxDrawDistance"), nauticalMiles);
    }

    void CXSwiftBusTrafficProxy::addPlane(const QString &callsign, const QString &modelName,
                                          const QString &aircraftIcao, const QString &airlineIcao,
                                          const QString &livery)
    {
        m_dbusInterface->callDBus(QLatin1String("addPlane"), callsign, modelName, aircraftIcao, airlineIcao, livery);
    }

    void CXSwiftBusTrafficProxy::removePlane(const QString &callsign)
    {
        m_dbusInterface->callDBus(QLatin1String("removePlane"), callsign);
    }

    void CXSwiftBusTrafficProxy::removeAllPlanes() { m_dbusInterface->callDBus(QLatin1String("removeAllPlanes")); }

    void CXSwiftBusTrafficProxy::setPlanesPositions(const PlanesPositions &planesPositions)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesPositions"), planesPositions.callsigns,
                                  planesPositions.latitudesDeg, planesPositions.longitudesDeg,
                                  planesPositions.altitudesFt, planesPositions.pitchesDeg, planesPositions.rollsDeg,
                                  planesPositions.headingsDeg, planesPositions.onGrounds);
    }

    void CXSwiftBusTrafficProxy::setPlanesSurfaces(const PlanesSurfaces &planesSurfaces)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesSurfaces"), planesSurfaces.callsigns, planesSurfaces.gears,
                                  planesSurfaces.flaps, planesSurfaces.spoilers, planesSurfaces.speedBrakes,
                                  planesSurfaces.slats, planesSurfaces.wingSweeps, planesSurfaces.thrusts,
                                  planesSurfaces.elevators, planesSurfaces.rudders, planesSurfaces.ailerons,
                                  planesSurfaces.landLights, planesSurfaces.taxiLights, planesSurfaces.beaconLights,
                                  planesSurfaces.strobeLights, planesSurfaces.navLights, planesSurfaces.lightPatterns);
    }

    void CXSwiftBusTrafficProxy::setPlanesTransponders(const PlanesTransponders &planesTransponders)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesTransponders"), planesTransponders.callsigns,
                                  planesTransponders.codes, planesTransponders.modeCs, planesTransponders.idents);
    }

    void CXSwiftBusTrafficProxy::setInterpolatorMode(const QString &callsign, bool spline)
    {
        m_dbusInterface->callDBus(QLatin1String("setInterpolatorMode"), callsign, spline);
    }

    void CXSwiftBusTrafficProxy::getRemoteAircraftData(const QStringList &callsigns,
                                                       const RemoteAircraftDataCallback &setter) const
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QStringList, QList<double>, QList<double>, QList<double>, QList<bool>, QList<double>>
                reply = *watcher;
            if (!reply.isError())
            {
                const QStringList callsigns = reply.argumentAt<0>();
                const QList<double> latitudesDeg = reply.argumentAt<1>();
                const QList<double> longitudesDeg = reply.argumentAt<2>();
                const QList<double> elevationsM = reply.argumentAt<3>();
                const QList<bool> waterFlags = reply.argumentAt<4>();
                const QList<double> verticalOffsets = reply.argumentAt<5>();

                setter(callsigns, latitudesDeg, longitudesDeg, elevationsM, waterFlags, verticalOffsets);
            }
            else
            {
                const QString errorMsg = reply.error().message();
                CLogMessage(this).warning(u"xswiftbus DBus error getRemoteAircraftData: %1") << errorMsg;
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getRemoteAircraftData"), callback, callsigns);
    }

    void CXSwiftBusTrafficProxy::getElevationAtPosition(const CCallsign &callsign, double latitudeDeg,
                                                        double longitudeDeg, double altitudeMeters,
                                                        const ElevationCallback &setter) const
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QString, double, double, double, bool> reply = *watcher;
            if (!reply.isError())
            {
                const CCallsign cs(reply.argumentAt<0>());
                const double elevationMeters = reply.argumentAt<1>();
                const double latitudeDegrees = reply.argumentAt<2>();
                const double longitudeDegrees = reply.argumentAt<3>();
                const CAltitude elevationAlt = std::isnan(elevationMeters) ?
                                                   CAltitude::null() :
                                                   CAltitude(elevationMeters, CLengthUnit::m(), CLengthUnit::ft());
                const CElevationPlane elevation(CLatitude(latitudeDegrees, CAngleUnit::deg()),
                                                CLongitude(longitudeDegrees, CAngleUnit::deg()), elevationAlt,
                                                CElevationPlane::singlePointRadius());
                const bool isWater = reply.argumentAt<4>();
                setter(elevation, cs, isWater);
                // CLogMessage(this).debug(u"XPlane elv. response: '%1' %2 %3 %4 %5") << cs.asString() << latitudeDeg <<
                // longitudeDeg << elevationMeters << isWater;
            }
            watcher->deleteLater();
        };

        m_dbusInterface->callDBusAsync(QLatin1String("getElevationAtPosition"), callback, callsign.asString(),
                                       latitudeDeg, longitudeDeg, altitudeMeters);
        // CLogMessage(this).debug(u"XPlane elv. request: '%1' %2 %3 %4") << callsign.asString() << latitudeDeg <<
        // longitudeDeg << altitudeMeters;
    }

    void CXSwiftBusTrafficProxy::setFollowedAircraft(const QString &callsign)
    {
        m_dbusInterface->callDBus(QLatin1String("setFollowedAircraft"), callsign);
    }
} // namespace swift::simplugin::xplane
