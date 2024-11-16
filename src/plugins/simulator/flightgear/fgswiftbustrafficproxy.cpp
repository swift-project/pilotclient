// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "fgswiftbustrafficproxy.h"

#include <QDBusConnection>
#include <QLatin1String>

#define FGSWIFTBUS_SERVICENAME "org.swift-project.fgswiftbus"

using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::physical_quantities;

namespace swift::simplugin::flightgear
{
    CFGSwiftBusTrafficProxy::CFGSwiftBusTrafficProxy(QDBusConnection &connection, QObject *parent, bool dummy) : QObject(parent)
    {
        m_dbusInterface = new swift::misc::CGenericDBusInterface(FGSWIFTBUS_SERVICENAME, ObjectPath(), InterfaceName(), connection, this);
        if (!dummy)
        {
            bool s;
            s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                   "simFrame", this, SIGNAL(simFrame()));
            Q_ASSERT(s);

            s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                   "remoteAircraftAdded", this, SIGNAL(remoteAircraftAdded(QString)));
            Q_ASSERT(s);

            s = connection.connect(QString(), "/fgswiftbus/traffic", "org.swift_project.fgswiftbus.traffic",
                                   "remoteAircraftAddingFailed", this, SIGNAL(remoteAircraftAddingFailed(QString)));
            Q_ASSERT(s);
        }
    }

    MultiplayerAcquireInfo CFGSwiftBusTrafficProxy::acquireMultiplayerPlanes()
    {
        QDBusPendingReply<bool, QString> reply = m_dbusInterface->asyncCall(QLatin1String("acquireMultiplayerPlanes"));
        reply.waitForFinished();
        if (reply.isError())
        {
            swift::misc::CLogMessage(this).debug(u"CFGSwiftBusTrafficProxy::acquireMultiplayerPlanes returned: %1") << reply.error().message();
        }
        MultiplayerAcquireInfo info;
        info.hasAcquired = reply.argumentAt<0>();
        info.owner = reply.argumentAt<1>();
        return info;
    }

    bool CFGSwiftBusTrafficProxy::initialize()
    {
        return m_dbusInterface->callDBusRet<bool>(QLatin1String("initialize"));
    }

    void CFGSwiftBusTrafficProxy::cleanup()
    {
        m_dbusInterface->callDBus(QLatin1String("cleanup"));
    }

    void CFGSwiftBusTrafficProxy::addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
    {
        m_dbusInterface->callDBus(QLatin1String("addPlane"), callsign, modelName, aircraftIcao, airlineIcao, livery);
    }

    void CFGSwiftBusTrafficProxy::removePlane(const QString &callsign)
    {
        m_dbusInterface->callDBus(QLatin1String("removePlane"), callsign);
    }

    void CFGSwiftBusTrafficProxy::removeAllPlanes()
    {
        m_dbusInterface->callDBus(QLatin1String("removeAllPlanes"));
    }

    void CFGSwiftBusTrafficProxy::setPlanesPositions(const PlanesPositions &planesPositions)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesPositions"),
                                  planesPositions.callsigns, planesPositions.latitudesDeg, planesPositions.longitudesDeg,
                                  planesPositions.altitudesFt, planesPositions.pitchesDeg, planesPositions.rollsDeg,
                                  planesPositions.headingsDeg, planesPositions.groundSpeedKts, planesPositions.onGrounds);
    }

    void CFGSwiftBusTrafficProxy::setPlanesSurfaces(const PlanesSurfaces &planesSurfaces)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesSurfaces"),
                                  planesSurfaces.callsigns, planesSurfaces.gears, planesSurfaces.flaps,
                                  planesSurfaces.spoilers, planesSurfaces.speedBrakes, planesSurfaces.slats,
                                  planesSurfaces.wingSweeps, planesSurfaces.thrusts, planesSurfaces.elevators,
                                  planesSurfaces.rudders, planesSurfaces.ailerons,
                                  planesSurfaces.landLights, planesSurfaces.taxiLights,
                                  planesSurfaces.beaconLights, planesSurfaces.strobeLights,
                                  planesSurfaces.navLights, planesSurfaces.lightPatterns);
    }

    void CFGSwiftBusTrafficProxy::setPlanesTransponders(const PlanesTransponders &planesTransponders)
    {
        m_dbusInterface->callDBus(QLatin1String("setPlanesTransponders"),
                                  planesTransponders.callsigns, planesTransponders.codes,
                                  planesTransponders.modeCs, planesTransponders.idents);
    }

    void CFGSwiftBusTrafficProxy::getRemoteAircraftData(const QStringList &callsigns, const RemoteAircraftDataCallback &setter) const
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QStringList, QList<double>, QList<double>, QList<double>, QList<double>> reply = *watcher;
            if (!reply.isError())
            {
                const QStringList callsigns = reply.argumentAt<0>();
                const QList<double> latitudesDeg = reply.argumentAt<1>();
                const QList<double> longitudesDeg = reply.argumentAt<2>();
                const QList<double> elevationsM = reply.argumentAt<3>();
                const QList<double> verticalOffsets = reply.argumentAt<4>();
                setter(callsigns, latitudesDeg, longitudesDeg, elevationsM, verticalOffsets);
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getRemoteAircraftData"), callback, callsigns);
    }

    void CFGSwiftBusTrafficProxy::getElevationAtPosition(const CCallsign &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters,
                                                         const ElevationCallback &setter) const
    {
        std::function<void(QDBusPendingCallWatcher *)> callback = [=](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QString, double> reply = *watcher;
            if (!reply.isError())
            {
                const CCallsign cs(reply.argumentAt<0>());
                const double elevationMeters = reply.argumentAt<1>();
                const CAltitude elevationAlt = std::isnan(elevationMeters) ? CAltitude::null() : CAltitude(elevationMeters, CLengthUnit::m(), CLengthUnit::ft());
                const CElevationPlane elevation(CLatitude(latitudeDeg, CAngleUnit::deg()),
                                                CLongitude(longitudeDeg, CAngleUnit::deg()),
                                                elevationAlt, CElevationPlane::singlePointRadius());
                setter(elevation, cs);
            }
            watcher->deleteLater();
        };
        m_dbusInterface->callDBusAsync(QLatin1String("getElevationAtPosition"), callback, callsign.asString(), latitudeDeg, longitudeDeg, altitudeMeters);
    }
} // namespace swift::simplugin::flightgear
