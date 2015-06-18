/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "traffic.h"
#include "XPMPMultiplayer.h"
#include "XPMPMultiplayerCSL.h"
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <QDateTime>
#include <QStringList>
#include <cstring>
#include <cmath>

namespace XBus
{

    CTraffic::Plane::Plane(void *id_, QString callsign_, QString aircraftIcao_, QString airlineIcao_, QString livery_)
        : id(id_), callsign(callsign_), aircraftIcao(aircraftIcao_), airlineIcao(airlineIcao_), livery(livery_)
    {
        position0.size = sizeof(position0);
        position1.size = sizeof(position1);
        surfaces.size = sizeof(surfaces);
        xpdr.size = sizeof(xpdr);

        std::strncpy(position1.label, qPrintable(callsign), sizeof(position1.label));
        surfaces.lights.timeOffset = static_cast<quint16>(qrand() % 0xffff);
    }

    QString g_xplanePath;
    QString g_sep;

    void initXPlanePath()
    {
        char xplanePath[512];
        XPLMGetSystemPath(xplanePath);
#ifdef Q_OS_MAC
        HFS2PosixPath(xplanePath, xplanePath, sizeof(xplanePath));
        g_sep = "/";
#else
        g_sep = XPLMGetDirectorySeparator();
#endif
        g_xplanePath = xplanePath;
    }

    CTraffic::CTraffic(QObject *parent) : QObject(parent)
    {
    }

    CTraffic::~CTraffic()
    {
        cleanup();
    }

    bool s_legacyDataOK = true;

    void CTraffic::initLegacyData()
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xbus" + g_sep + "LegacyData" + g_sep;

        auto err = XPMPMultiplayerInitLegacyData(qPrintable(dir + "CSL"), qPrintable(dir + "related.txt"),
            qPrintable(dir + "lights.png"), qPrintable(dir + "Doc8643.txt"), "C172", preferences, preferences);
        if (*err) { s_legacyDataOK = false; }
    }

    bool CTraffic::initialize()
    {
        if (! s_legacyDataOK) { return false; }

        auto err = XPMPMultiplayerInit(preferences, preferences);
        if (*err) { cleanup(); return false; }
        m_initialized = true;

        err = XPMPMultiplayerEnable();
        if (*err) { cleanup(); return false; }
        m_enabled = true;

        XPMPLoadPlanesIfNecessary();
        return true;
    }

    void CTraffic::cleanup()
    {
        removeAllPlanes();

        if (m_enabled)
        {
            m_enabled = false;
            XPMPMultiplayerDisable();
        }

        if (m_initialized)
        {
            m_initialized = false;
            XPMPMultiplayerCleanup();
        }
    }

    int CTraffic::preferences(const char *section, const char *name, int def)
    {
        Q_UNUSED(name);
        Q_UNUSED(section);
        // TODO [planes] max_full_count
        return def;
    }

    float CTraffic::preferences(const char *section, const char *name, float def)
    {
        // TODO [planes] full_distance
        Q_UNUSED(name);
        Q_UNUSED(section);
        return def;
    }

    bool CTraffic::loadPlanesPackage(const QString &path)
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xbus" + g_sep + "LegacyData" + g_sep;

        auto err = XPMPLoadCSLPackage(qPrintable(path), qPrintable(dir + "related.txt"), qPrintable(dir + "Doc8643.txt"));
        if (*err) { return false; }
        return true;
    }

    void CTraffic::setDefaultIcao(const QString &defaultIcao)
    {
        XPMPSetDefaultPlaneICAO(qPrintable(defaultIcao));
    }

    void CTraffic::setDrawingLabels(bool drawing)
    {
        if (drawing)
        {
            XPMPEnableAircraftLabels();
        }
        else
        {
            XPMPDisableAircraftLabels();
        }
    }

    bool CTraffic::isDrawingLabels() const
    {
        return XPMPDrawingAircraftLabels();
    }

    void CTraffic::updateInstalledModels()
    {
        int numberOfModels = XPMPGetNumberOfInstalledModels();
        QStringList modelNames;
        QStringList icaos;
        QStringList airlines;
        QStringList liveries;

        for (int i = 0; i < numberOfModels; ++i)
        {
            const char *modelName = nullptr;
            const char *icao = nullptr;
            const char *airline = nullptr;
            const char *livery = nullptr;
            XPMPGetModelInfo(i, &modelName, &icao, &airline, &livery);
            modelNames.append(QString::fromLocal8Bit(modelName));
            icaos.append(QString::fromLocal8Bit(icao));
            airlines.append(QString::fromLocal8Bit(airline));
            liveries.append(QString::fromLocal8Bit(livery));
        }

        emit installedModelsUpdated(modelNames, icaos, airlines, liveries);
    }

    void CTraffic::addPlane(const QString &callsign, const QString &modelName, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery)
    {
        XPMPPlaneID id = nullptr;
        if (modelName.isEmpty())
        {
            id = XPMPCreatePlane(qPrintable(aircraftIcao), qPrintable(airlineIcao), qPrintable(livery), getPlaneData, static_cast<void *>(this));
        }
        else
        {
            id = XPMPCreatePlaneWithModelName(qPrintable(modelName), qPrintable(aircraftIcao), qPrintable(airlineIcao), qPrintable(livery), getPlaneData, static_cast<void *>(this));
        }

        if (id)
        {
            auto plane = new Plane(id, callsign, aircraftIcao, airlineIcao, livery);
            m_planesByCallsign[callsign] = plane;
            m_planesById[id] = plane;
        }
    }

    void CTraffic::removePlane(const QString &callsign)
    {
        auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            m_planesByCallsign.remove(callsign);
            m_planesById.remove(plane->id);
            XPMPDestroyPlane(plane->id);
            delete plane;
        }
    }

    void CTraffic::removeAllPlanes()
    {
        for (auto plane : m_planesByCallsign)
        {
            XPMPDestroyPlane(plane->id);
            delete plane;
        }
        m_planesByCallsign.clear();
        m_planesById.clear();
    }

    void CTraffic::setPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading)
    {
        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            plane->time0 = plane->time1;
            plane->time1 = QDateTime::currentMSecsSinceEpoch();

            std::memcpy(reinterpret_cast<char *>(&plane->position0) + sizeof(plane->position0.size),
                        reinterpret_cast<char *>(&plane->position1) + sizeof(plane->position1.size),
                        sizeof(plane->position0) - sizeof(plane->position0.size));

            plane->position1.lat = latitude;
            plane->position1.lon = longitude;
            plane->position1.elevation = altitude;
            plane->position1.pitch = static_cast<float>(pitch);
            plane->position1.roll = static_cast<float>(roll);
            plane->position1.heading = static_cast<float>(heading);
        }
    }

    void CTraffic::setPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
        double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern)
    {
        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            plane->hasSurfaces = true;
            plane->surfaces.gearPosition = gear;
            plane->surfaces.flapRatio = flap;
            plane->surfaces.spoilerRatio = spoiler;
            plane->surfaces.speedBrakeRatio = speedBrake;
            plane->surfaces.slatRatio = slat;
            plane->surfaces.wingSweep = wingSweep;
            plane->surfaces.thrust = thrust;
            plane->surfaces.yokePitch = elevator;
            plane->surfaces.yokeHeading = rudder;
            plane->surfaces.yokeRoll = aileron;
            plane->surfaces.lights.landLights = landLight;
            plane->surfaces.lights.bcnLights = beaconLight;
            plane->surfaces.lights.strbLights = strobeLight;
            plane->surfaces.lights.navLights = navLight;
            plane->surfaces.lights.flashPattern = lightPattern;
        }
    }

    void CTraffic::setPlaneTransponder(const QString &callsign, int code, bool modeC, bool ident)
    {
        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            plane->hasXpdr = true;
            plane->xpdr.code = code;
            if (ident) { plane->xpdr.mode = xpmpTransponderMode_ModeC_Ident; }
            else if (modeC) { plane->xpdr.mode = xpmpTransponderMode_ModeC; }
            else { plane->xpdr.mode = xpmpTransponderMode_Standby; }
        }
    }

    // memcmp function which ignores the header ("size" member) and compares only the payload (the rest of the struct)
    template <typename T>
    int memcmpPayload(T *dst, T *src)
    {
        return std::memcmp(reinterpret_cast<char *>(dst) + sizeof(dst->size),
                           reinterpret_cast<char *>(src) + sizeof(src->size),
                           sizeof(*dst) - sizeof(dst->size));
    }

    // linearly interpolate angle in degrees
    template <typename T>
    T lerpDegrees(T from, T to, double factor)
    {
        if (std::fabs(to - from) > 180)
        {
            if (to > from) { to -= 360; }
            else { to += 360; }
        }
        return from + (to - from) * static_cast<T>(factor);
    }

    int CTraffic::getPlaneData(void *id, int dataType, void *io_data)
    {
        auto plane = m_planesById.value(id, nullptr);
        if (! plane) { return xpmpData_Unavailable; }

        switch (dataType)
        {
        case xpmpDataType_Position:
            if (plane->time1)
            {
                const auto io_position = static_cast<XPMPPlanePosition_t *>(io_data);

                if (plane->time0) // we have two positions between which to interpolate
                {
                    const auto currentTime = QDateTime::currentMSecsSinceEpoch();
                    const auto factor = static_cast<double>(currentTime - plane->time0) / (plane->time1 - plane->time0);
                    io_position->lat = lerpDegrees(plane->position0.lat, plane->position1.lat, factor);
                    io_position->lon = lerpDegrees(plane->position0.lon, plane->position1.lon, factor);
                    io_position->pitch = lerpDegrees(plane->position0.pitch, plane->position1.pitch, factor);
                    io_position->roll = lerpDegrees(plane->position0.roll, plane->position1.roll, factor);
                    io_position->heading = lerpDegrees(plane->position0.heading, plane->position1.heading, factor);
                    io_position->elevation = plane->position0.elevation + (plane->position1.elevation - plane->position0.elevation) * factor;
                    return xpmpData_NewData;
                }
                else // we only received one position so far
                {
                    if (memcmpPayload(io_position, &plane->position1))
                    {
                        std::memcpy(io_position, &plane->position1, sizeof(*io_position));
                        return xpmpData_NewData;
                    }
                    else { return xpmpData_Unchanged; }
                }
            }
            else { return xpmpData_Unavailable; }

        case xpmpDataType_Surfaces:
            if (plane->hasSurfaces)
            {
                const auto io_surfaces = static_cast<XPMPPlaneSurfaces_t *>(io_data);

                if (memcmpPayload(io_surfaces, &plane->surfaces))
                {
                    std::memcpy(io_surfaces, &plane->surfaces, sizeof(*io_surfaces));
                    return xpmpData_NewData;
                }
                else { return xpmpData_Unchanged; }
            }
            else { return xpmpData_Unavailable; }

        case xpmpDataType_Radar:
            if (plane->hasXpdr)
            {
                const auto io_xpdr = static_cast<XPMPPlaneRadar_t *>(io_data);

                if (memcmpPayload(io_xpdr, &plane->xpdr))
                {
                    std::memcpy(io_xpdr, &plane->xpdr, sizeof(*io_xpdr));
                    return xpmpData_NewData;
                }
                else { return xpmpData_Unchanged; }
            }
            else { return xpmpData_Unavailable; }

        default: return xpmpData_Unavailable;
        }
    }

}
