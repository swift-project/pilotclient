/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "traffic.h"
#include "utils.h"
#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/aviation/callsign.h"
#include "XPMPMultiplayer.h"
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <QDateTime>
#include <QStringList>
#include <cstring>
#include <cmath>

// clazy:excludeall=reserve-candidates

namespace XSwiftBus
{

    CTraffic::Plane::Plane(void *id_, QString callsign_, QString aircraftIcao_, QString airlineIcao_, QString livery_)
        : id(id_), callsign(callsign_), aircraftIcao(aircraftIcao_), airlineIcao(airlineIcao_), livery(livery_), interpolator(callsign)
    {
        std::memset(static_cast<void *>(&surfaces), 0, sizeof(surfaces));
        surfaces.lights.bcnLights = surfaces.lights.landLights = surfaces.lights.navLights = surfaces.lights.strbLights = 1;

        surfaces.size = sizeof(surfaces);
        xpdr.size = sizeof(xpdr);

        std::strncpy(label, qPrintable(callsign), sizeof(label));
        surfaces.lights.timeOffset = static_cast<quint16>(qrand() % 0xffff);
    }

    BlackMisc::Simulation::CInterpolationHints CTraffic::Plane::hints()
    {
        //! \todo MS 865 CInterpolationAndRenderingSetup allows to setup interpolation in the GUI
        //        Also to disable aircraft parts / or logging parts (log file). I wonder if you want to consider it here
        //        e.g. interpolator->getInterpolatorSetup().getLogCallsigns().contains(callsign)
        //        if the setup is needed more than once, store it here to avoid multiple locks
        using namespace BlackMisc::PhysicalQuantities;
        using namespace BlackMisc::Aviation;
        using namespace BlackMisc::Simulation;

        CInterpolationAndRenderingSetup setup;
        CInterpolationHints hints;
        CPartsStatus status;
        constexpr double fudgeFactor = 3.0; //! \fixme Value should be different for each plane, derived from the CSL model geometry
        hints.setAircraftParts(interpolator.getInterpolatedParts(-1, setup, status));
        hints.setCGAboveGround({ fudgeFactor, CLengthUnit::m() }); // fudge factor
        hints.setElevationProvider([this](const auto & situation)
        {
            const auto meters = terrainProbe.getElevation(
                                    situation.latitude().value(CAngleUnit::deg()),
                                    situation.longitude().value(CAngleUnit::deg()),
                                    situation.getAltitude().value(CLengthUnit::m()));
            if (std::isnan(meters)) { return CAltitude::null(); }
            return CAltitude(CLength(meters, CLengthUnit::m()), CAltitude::MeanSeaLevel);
        });
        return hints;
    }

    CTraffic::CTraffic(QObject *parent) :
        QObject(parent)
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
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "LegacyData" + g_sep;

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

    int g_maxPlanes = 100;
    float g_drawDistance = 50.0f;

    int CTraffic::preferences(const char *section, const char *name, int def)
    {
        if (strcmp(section, "planes") == 0 && strcmp(name, "max_full_count") == 0)
        {
            return g_maxPlanes;
        }
        else if (strcmp(section, "debug") == 0 && strcmp(name, "allow_obj8_async_load") == 0)
        {
            return true;
        }
        return def;
    }

    float CTraffic::preferences(const char *section, const char *name, float def)
    {
        if (strcmp(section, "planes") == 0 && strcmp(name, "full_distance") == 0)
        {
            return g_drawDistance;
        }
        return def;
    }

    bool CTraffic::loadPlanesPackage(const QString &path)
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "LegacyData" + g_sep;

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

    void CTraffic::setMaxPlanes(int planes)
    {
        g_maxPlanes = planes;
    }

    void CTraffic::setMaxDrawDistance(double nauticalMiles)
    {
        g_drawDistance = static_cast<float>(nauticalMiles);
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
        for (auto plane : BlackMisc::as_const(m_planesByCallsign))
        {
            XPMPDestroyPlane(plane->id);
            delete plane;
        }
        m_planesByCallsign.clear();
        m_planesById.clear();
    }

    void CTraffic::addPlanePosition(const QString &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading, qint64 relativeTime, qint64 timeOffset)
    {
        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            using namespace BlackMisc::PhysicalQuantities;
            using namespace BlackMisc::Aviation;
            CAircraftSituation situation(
                callsign,
                BlackMisc::Geo::CCoordinateGeodetic(latitude, longitude, altitude),
                CHeading(heading, CHeading::True, CAngleUnit::deg()),
                CAngle(pitch, CAngleUnit::deg()),
                CAngle(roll, CAngleUnit::deg()),
                CSpeed(0, CSpeedUnit::kts())
            );
            situation.setMSecsSinceEpoch(relativeTime + QDateTime::currentMSecsSinceEpoch());
            situation.setTimeOffsetMs(timeOffset);
            plane->interpolator.addAircraftSituation(situation);
        }
    }

    void CTraffic::addPlaneSurfaces(const QString &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
                                    double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround, qint64 relativeTime, qint64 timeOffset)
    {
        const auto surfaces = std::make_pair(relativeTime + timeOffset + QDateTime::currentMSecsSinceEpoch(), [ = ](Plane * plane)
        {
            plane->hasSurfaces = true;
            plane->targetGearPosition = gear;
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
        });

        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            if (plane->hasSurfaces) { plane->pendingSurfaces.push_back(surfaces); }
            else { surfaces.second(plane); }

            BlackMisc::Aviation::CAircraftParts parts;
            parts.setOnGround(onGround);
            parts.setMSecsSinceEpoch(relativeTime + QDateTime::currentMSecsSinceEpoch());
            parts.setTimeOffsetMs(timeOffset);
            plane->interpolator.addAircraftParts(parts);
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

    void CTraffic::setInterpolatorMode(const QString &callsign, bool spline)
    {
        const auto plane = m_planesByCallsign.value(callsign, nullptr);
        if (plane)
        {
            plane->interpolator.setMode(spline ? BlackMisc::Simulation::CInterpolatorMulti::ModeSpline
                                        : BlackMisc::Simulation::CInterpolatorMulti::ModeLinear);
        }
        else if (callsign.isEmpty())
        {
            for (const auto &callsign : BlackMisc::makeKeysRange(BlackMisc::as_const(m_planesByCallsign)))
            {
                setInterpolatorMode(callsign, spline);
            }
        }
    }

    //! memcmp function which ignores the header ("size" member) and compares only the payload (the rest of the struct)
    template <typename T>
    int memcmpPayload(T *dst, T *src)
    {
        return std::memcmp(reinterpret_cast<char *>(dst) + sizeof(dst->size),
                           reinterpret_cast<char *>(src) + sizeof(src->size),
                           sizeof(*dst) - sizeof(dst->size));
    }

    int CTraffic::getPlaneData(void *id, int dataType, void *io_data)
    {
        auto plane = m_planesById.value(id, nullptr);
        if (! plane) { return xpmpData_Unavailable; }

        switch (dataType)
        {
        case xpmpDataType_Position:
            {
                BlackMisc::Simulation::CInterpolationAndRenderingSetup setup;
                BlackMisc::Simulation::CInterpolationStatus status;
                const auto situation = plane->interpolator.getInterpolatedSituation(-1, setup, plane->hints(), status);
                if (! status.hasValidSituation()) { return xpmpData_Unavailable; }

                //! \fixme KB 2018-01 commented out with T229. Change detection needs to go somewhere else
                // if (! status.hasChangedPosition()) { return xpmpData_Unchanged; }

                using namespace BlackMisc::PhysicalQuantities;
                using namespace BlackMisc::Aviation;
                const auto io_position = static_cast<XPMPPlanePosition_t *>(io_data);
                io_position->lat = situation.latitude().value(CAngleUnit::deg());
                io_position->lon = situation.longitude().value(CAngleUnit::deg());
                io_position->elevation = situation.getAltitude().value(CLengthUnit::ft());
                io_position->pitch = static_cast<float>(situation.getPitch().value(CAngleUnit::deg()));
                io_position->roll = static_cast<float>(situation.getBank().value(CAngleUnit::deg()));
                io_position->heading = static_cast<float>(situation.getHeading().value(CAngleUnit::deg()));
                std::strncpy(io_position->label, plane->label, sizeof(plane->label)); // fixme don't need to copy on every frame
                return xpmpData_NewData;
            }

        case xpmpDataType_Surfaces:
            if (plane->hasSurfaces)
            {
                const auto currentTime = QDateTime::currentMSecsSinceEpoch();
                while (! plane->pendingSurfaces.isEmpty() && plane->pendingSurfaces.constFirst().first <= currentTime)
                {
                    //! \todo if gear is currently retracted, look ahead and pull gear position from pendingSurfaces up to 5 seconds in the future
                    plane->pendingSurfaces.constFirst().second(plane);
                    plane->pendingSurfaces.pop_front();
                }
                if (plane->surfaces.gearPosition != plane->targetGearPosition)
                {
                    // interpolate gear position
                    constexpr float gearMoveTimeMs = 5000;
                    const auto gearPositionDiffRemaining = plane->targetGearPosition - plane->surfaces.gearPosition;
                    const auto gearPositionDiffThisFrame = (currentTime - plane->prevSurfacesLerpTime) / gearMoveTimeMs;
                    plane->surfaces.gearPosition += std::copysign(gearPositionDiffThisFrame, gearPositionDiffRemaining);
                    plane->surfaces.gearPosition = qBound(0.0f, plane->surfaces.gearPosition, 1.0f);
                }
                plane->prevSurfacesLerpTime = currentTime;
                const auto io_surfaces = static_cast<XPMPPlaneSurfaces_t *>(io_data);

                if (memcmpPayload(io_surfaces, &plane->surfaces))
                {
                    std::memcpy(io_surfaces, &plane->surfaces, sizeof(*io_surfaces));
                    return xpmpData_NewData;
                }
                else { return xpmpData_Unchanged; }
            }

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

//! \endcond
