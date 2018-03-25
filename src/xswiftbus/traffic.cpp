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
#include "XPMPMultiplayer.h"
#include "XPMPPlaneRenderer.h"
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <cassert>
#include <cstring>
#include <cmath>
#include <ctime>
#include <algorithm>

// clazy:excludeall=reserve-candidates

namespace XSwiftBus
{
    CTraffic::Plane::Plane(void *id_, const std::string &callsign_, const std::string &aircraftIcao_, const std::string &airlineIcao_, const std::string &livery_, const std::string &modelName_)
        : id(id_), callsign(callsign_), aircraftIcao(aircraftIcao_), airlineIcao(airlineIcao_), livery(livery_), modelName(modelName_)
    {
        std::memset(static_cast<void *>(&surfaces), 0, sizeof(surfaces));
        surfaces.lights.bcnLights = surfaces.lights.landLights = surfaces.lights.navLights = surfaces.lights.strbLights = 1;

        surfaces.size = sizeof(surfaces);
        xpdr.size = sizeof(xpdr);

        std::strncpy(label, callsign.c_str(), sizeof(label));
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        surfaces.lights.timeOffset = static_cast<uint16_t>(std::rand() % 0xffff);
    }

    CTraffic::CTraffic(CDBusConnection *dbusConnection) :
        CDBusObject(dbusConnection)
    {
        registerDBusObjectPath(XSWIFTBUS_TRAFFIC_INTERFACENAME, XSWIFTBUS_TRAFFIC_OBJECTPATH);
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

        std::string csl = dir + "CSL";
        std::string related = dir + "related.txt";
        std::string doc8643 = dir + "Doc8643.txt";
        std::string lights = dir + "lights.png";
        auto err = XPMPMultiplayerInitLegacyData(csl.c_str(), related.c_str(), lights.c_str(), doc8643.c_str(),
                   "C172", preferences, preferences);
        if (*err) { s_legacyDataOK = false; }
    }

    bool CTraffic::initialize()
    {
        if (! s_legacyDataOK) { return false; }

        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep;
        auto err = XPMPMultiplayerInit(preferences, preferences, dir.c_str());
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

    void CTraffic::emitSimFrame()
    {
        sendDBusSignal("simFrame");
    }

    void CTraffic::emitRemoteAircraftData(const std::string &callsign, double latitude, double longitude, double elevation, double modelVerticalOffset)
    {
        CDBusMessage signalRemoteAircraftData = CDBusMessage::createSignal(XSWIFTBUS_TRAFFIC_OBJECTPATH, XSWIFTBUS_TRAFFIC_INTERFACENAME, "remoteAircraftData");
        signalRemoteAircraftData.beginArgumentWrite();
        signalRemoteAircraftData.appendArgument(callsign);
        signalRemoteAircraftData.appendArgument(latitude);
        signalRemoteAircraftData.appendArgument(longitude);
        signalRemoteAircraftData.appendArgument(elevation);
        signalRemoteAircraftData.appendArgument(modelVerticalOffset);
        sendDBusMessage(signalRemoteAircraftData);
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

    bool CTraffic::loadPlanesPackage(const std::string &path)
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "LegacyData" + g_sep;

        std::string related = dir + "related.txt";
        std::string doc8643 = dir + "Doc8643.txt";
        auto err = XPMPLoadCSLPackage(path.c_str(), related.c_str(), doc8643.c_str());
        if (*err) { return false; }
        return true;
    }

    void CTraffic::setDefaultIcao(const std::string &defaultIcao)
    {
        XPMPSetDefaultPlaneICAO(defaultIcao.c_str());
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

    void CTraffic::addPlane(const std::string &callsign, const std::string &modelName, const std::string &aircraftIcao, const std::string &airlineIcao, const std::string &livery)
    {
        XPMPPlaneID id = nullptr;
        if (modelName.empty())
        {
            id = XPMPCreatePlane(aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), getPlaneData, static_cast<void *>(this));
        }
        else
        {
            id = XPMPCreatePlaneWithModelName(modelName.c_str(), aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), getPlaneData, static_cast<void *>(this));
        }

        if (id)
        {
            Plane *plane = new Plane(id, callsign, aircraftIcao, airlineIcao, livery, modelName);
            m_planesByCallsign[callsign] = plane;
            m_planesById[id] = plane;
        }
    }

    void CTraffic::removePlane(const std::string &callsign)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        Plane *plane = planeIt->second;
        m_planesByCallsign.erase(callsign);
        m_planesById.erase(plane->id);
        XPMPDestroyPlane(plane->id);
        delete plane;
    }

    void CTraffic::removeAllPlanes()
    {
        for (const auto &kv : m_planesByCallsign)
        {
            Plane *plane = kv.second;
            assert(plane);
            XPMPDestroyPlane(plane->id);
            delete plane;
        }
        m_planesByCallsign.clear();
        m_planesById.clear();
    }

    void CTraffic::setPlanePosition(const std::string &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        Plane *plane = planeIt->second;
        if (!plane) { return; }
        plane->position.lat = latitude;
        plane->position.lon = longitude;
        plane->position.elevation = altitude;
        plane->position.pitch = static_cast<float>(pitch);
        plane->position.roll = static_cast<float>(roll);
        plane->position.heading = static_cast<float>(heading);
    }

    void CTraffic::setPlaneSurfaces(const std::string &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
                                    double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround)
    {
        (void) onGround;
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        Plane *plane = planeIt->second;
        if (!plane) { return; }

        plane->hasSurfaces = true;
        plane->targetGearPosition = static_cast<float>(gear);
        plane->surfaces.flapRatio = static_cast<float>(flap);
        plane->surfaces.spoilerRatio = static_cast<float>(spoiler);
        plane->surfaces.speedBrakeRatio = static_cast<float>(speedBrake);
        plane->surfaces.slatRatio = static_cast<float>(slat);
        plane->surfaces.wingSweep = static_cast<float>(wingSweep);
        plane->surfaces.thrust = static_cast<float>(thrust);
        plane->surfaces.yokePitch = static_cast<float>(elevator);
        plane->surfaces.yokeHeading = static_cast<float>(rudder);
        plane->surfaces.yokeRoll = static_cast<float>(aileron);
        plane->surfaces.lights.landLights = landLight;
        plane->surfaces.lights.bcnLights = beaconLight;
        plane->surfaces.lights.strbLights = strobeLight;
        plane->surfaces.lights.navLights = navLight;
        plane->surfaces.lights.flashPattern = lightPattern;
    }

    void CTraffic::setPlaneTransponder(const std::string &callsign, int code, bool modeC, bool ident)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        Plane *plane = planeIt->second;
        if (!plane) { return; }
        plane->hasXpdr = true;
        plane->xpdr.code = code;
        if (ident) { plane->xpdr.mode = xpmpTransponderMode_ModeC_Ident; }
        else if (modeC) { plane->xpdr.mode = xpmpTransponderMode_ModeC; }
        else { plane->xpdr.mode = xpmpTransponderMode_Standby; }
    }

    void CTraffic::requestRemoteAircraftData()
    {
        if (m_planesByCallsign.empty()) { return; }
        for (const auto &kv : m_planesByCallsign)
        {
            Plane *plane = kv.second;
            assert(plane);
            double lat = plane->position.lat;
            double lon = plane->position.lon;
            double elevation = plane->position.elevation;
            double groundElevation = plane->terrainProbe.getElevation(lat, lon, elevation);
            if (std::isnan(groundElevation)) { groundElevation = 0.0; }
            double fudgeFactor = 3.0;
            actualVertOffsetInfo(plane->modelName.c_str(), nullptr, &fudgeFactor);
            emitRemoteAircraftData(plane->callsign, lat, lon, groundElevation, fudgeFactor);
        }
    }

    const char *introspection_traffic =
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
#include "org.swift_project.xswiftbus.traffic.xml"
        ;

    DBusHandlerResult CTraffic::dbusMessageHandler(const CDBusMessage &message_)
    {
        CDBusMessage message(message_);
        const std::string sender = message.getSender();
        const dbus_uint32_t serial = message.getSerial();
        const bool wantsReply = message.wantsReply();

        if (message.getInterfaceName() == DBUS_INTERFACE_INTROSPECTABLE)
        {
            if (message.getMethodName() == "Introspect")
            {
                sendDBusReply(sender, serial, introspection_traffic);
            }
        }
        else if (message.getInterfaceName() == XSWIFTBUS_TRAFFIC_INTERFACENAME)
        {
            if (message.getMethodName() == "initialize")
            {
                sendDBusReply(sender, serial, initialize());
            }
            else if (message.getMethodName() == "cleanup")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([ = ]()
                {
                    cleanup();
                });
            }
            else if (message.getMethodName() == "loadPlanesPackage")
            {
                std::string path;
                message.beginArgumentRead();
                message.getArgument(path);
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, loadPlanesPackage(path));
                });
            }
            else if (message.getMethodName() == "setDefaultIcao")
            {
                std::string defaultIcao;
                message.beginArgumentRead();
                message.getArgument(defaultIcao);
                queueDBusCall([ = ]()
                {
                    setDefaultIcao(defaultIcao);
                });
            }
            else if (message.getMethodName() == "setDrawingLabels")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                bool drawing = true;
                message.beginArgumentRead();
                message.getArgument(drawing);
                queueDBusCall([ = ]()
                {
                    setDrawingLabels(drawing);
                });

            }
            else if (message.getMethodName() == "isDrawingLabels")
            {
                queueDBusCall([ = ]()
                {
                    sendDBusReply(sender, serial, isDrawingLabels());
                });
            }
            else if (message.getMethodName() == "setMaxPlanes")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                int planes = 100;
                message.beginArgumentRead();
                message.getArgument(planes);
                queueDBusCall([ = ]()
                {
                    setMaxPlanes(planes);
                });
            }
            else if (message.getMethodName() == "setMaxDrawDistance")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                double nauticalMiles = 100;
                message.beginArgumentRead();
                message.getArgument(nauticalMiles);
                queueDBusCall([ = ]()
                {
                    setMaxDrawDistance(nauticalMiles);
                });

            }
            else if (message.getMethodName() == "addPlane")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                std::string modelName;
                std::string aircraftIcao;
                std::string airlineIcao;
                std::string livery;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(modelName);
                message.getArgument(aircraftIcao);
                message.getArgument(airlineIcao);
                message.getArgument(livery);

                queueDBusCall([ = ]()
                {
                    addPlane(callsign, modelName, aircraftIcao, airlineIcao, livery);
                });
            }
            else if (message.getMethodName() == "removePlane")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                message.beginArgumentRead();
                message.getArgument(callsign);
                queueDBusCall([ = ]()
                {
                    removePlane(callsign);
                });
            }
            else if (message.getMethodName() == "removeAllPlanes")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([ = ]()
                {
                    removeAllPlanes();
                });
            }
            else if (message.getMethodName() == "setPlanePosition")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                double latitude = 0.0;
                double longitude = 0.0;
                double altitude = 0.0;
                double pitch = 0.0;
                double roll = 0.0;
                double heading = 0.0;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(latitude);
                message.getArgument(longitude);
                message.getArgument(altitude);
                message.getArgument(pitch);
                message.getArgument(roll);
                message.getArgument(heading);
                queueDBusCall([ = ]()
                {
                    setPlanePosition(callsign, latitude, longitude, altitude, pitch, roll, heading);
                });
            }
            else if (message.getMethodName() == "setPlaneSurfaces")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                double gear = 0.0;
                double flap = 0.0;
                double spoiler = 0.0;
                double speedBrake = 0.0;
                double slat = 0.0;
                double wingSweep = 0.0;
                double thrust = 0.0;
                double elevator = 0.0;
                double rudder = 0.0;
                double aileron = 0.0;
                bool landLight = false;
                bool beaconLight = false;
                bool strobeLight = false;
                bool navLight = false;
                bool lightPattern = false;
                bool onGround = false;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(gear);
                message.getArgument(flap);
                message.getArgument(spoiler);
                message.getArgument(speedBrake);
                message.getArgument(slat);
                message.getArgument(wingSweep);
                message.getArgument(thrust);
                message.getArgument(elevator);
                message.getArgument(rudder);
                message.getArgument(aileron);
                message.getArgument(landLight);
                message.getArgument(beaconLight);
                message.getArgument(strobeLight);
                message.getArgument(navLight);
                message.getArgument(lightPattern);
                message.getArgument(onGround);
                queueDBusCall([ = ]()
                {
                    setPlaneSurfaces(callsign, gear, flap, spoiler, speedBrake, slat, wingSweep, thrust, elevator,
                                     rudder, aileron, landLight, beaconLight, strobeLight, navLight, lightPattern,
                                     onGround);
                });
            }
            else if (message.getMethodName() == "setPlaneTransponder")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                int code = 0;
                bool modeC = false;
                bool ident = false;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(code);
                message.getArgument(modeC);
                message.getArgument(ident);
                queueDBusCall([ = ]()
                {
                    setPlaneTransponder(callsign, code, modeC, ident);
                });
            }
            else if (message.getMethodName() == "requestRemoteAircraftData")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                queueDBusCall([ = ]()
                {
                    requestRemoteAircraftData();
                });
            }
            else
            {
                // Unknown message. Tell DBus that we cannot handle it
                return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }
        }
        return DBUS_HANDLER_RESULT_HANDLED;
    }

    int CTraffic::processDBus()
    {
        emitSimFrame();
        invokeQueuedDBusCalls();
        return 1;
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
        auto planeIt = m_planesById.find(id);
        assert(planeIt != m_planesById.end());
        Plane *plane = planeIt->second;
        if (!plane) { return xpmpData_Unavailable; }

        switch (dataType)
        {
        case xpmpDataType_Position:
            {
                const auto io_position = static_cast<XPMPPlanePosition_t *>(io_data);
                io_position->lat = plane->position.lat;
                io_position->lon = plane->position.lon;
                io_position->elevation = plane->position.elevation;
                io_position->pitch = plane->position.pitch;
                io_position->roll = plane->position.roll;
                io_position->heading = plane->position.heading;
                std::strncpy(io_position->label, plane->label, sizeof(plane->label)); // fixme don't need to copy on every frame
                return xpmpData_NewData;
            }

        case xpmpDataType_Surfaces:
            if (plane->hasSurfaces)
            {
                const auto now = std::chrono::system_clock::now();

                if (plane->surfaces.gearPosition != plane->targetGearPosition)
                {
                    // interpolate gear position
                    constexpr float gearMoveTimeMs = 5000;
                    const auto gearPositionDiffRemaining = plane->targetGearPosition - plane->surfaces.gearPosition;

                    auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - plane->prevSurfacesLerpTime);
                    const auto gearPositionDiffThisFrame = (diffMs.count()) / gearMoveTimeMs;
                    plane->surfaces.gearPosition += std::copysign(gearPositionDiffThisFrame, gearPositionDiffRemaining);
                    plane->surfaces.gearPosition = std::max(0.0f, std::min(plane->surfaces.gearPosition, 1.0f));
                }
                plane->prevSurfacesLerpTime = now;
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
