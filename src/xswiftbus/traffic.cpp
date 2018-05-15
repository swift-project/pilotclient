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
#include "XPLMGraphics.h"
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
        XPLMRegisterDrawCallback(drawCallback, xplm_Phase_Airplanes, 1, this);
    }

    CTraffic::~CTraffic()
    {
        XPLMUnregisterDrawCallback(drawCallback, xplm_Phase_Airplanes, 1, this);
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

    void CTraffic::emitSimFrame()
    {
        if (m_emitSimFrame) { sendDBusSignal("simFrame"); }
        m_emitSimFrame = !m_emitSimFrame;
    }

    void CTraffic::emitPlaneAdded(const std::string &callsign)
    {
        CDBusMessage signalPlaneAdded = CDBusMessage::createSignal(XSWIFTBUS_TRAFFIC_OBJECTPATH, XSWIFTBUS_TRAFFIC_INTERFACENAME, "remoteAircraftAdded");
        signalPlaneAdded.beginArgumentWrite();
        signalPlaneAdded.appendArgument(callsign);
        sendDBusMessage(signalPlaneAdded);
    }

    void CTraffic::emitPlaneAddingFailed(const std::string &callsign)
    {
        CDBusMessage signalPlaneAddingFailed = CDBusMessage::createSignal(XSWIFTBUS_TRAFFIC_OBJECTPATH, XSWIFTBUS_TRAFFIC_INTERFACENAME, "remoteAircraftAddingFailed");
        signalPlaneAddingFailed.beginArgumentWrite();
        signalPlaneAddingFailed.appendArgument(callsign);
        sendDBusMessage(signalPlaneAddingFailed);
    }

    void CTraffic::orbitRemotePlane(const std::string &callsign)
    {
        m_planeViewCallsign = callsign;

        /* This is the hotkey callback.  First we simulate a joystick press and
         * release to put us in 'free view 1'.  This guarantees that no panels
         * are showing and we are an external view. */
        XPLMCommandButtonPress(xplm_joy_v_fr1);
        XPLMCommandButtonRelease(xplm_joy_v_fr1);

        /* Now we control the camera until the view changes. */
        XPLMControlCamera(xplm_ControlCameraUntilViewChanges, orbitPlaneFunc, this);
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
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt != m_planesByCallsign.end()) { return; }

        XPMPPlaneID id = nullptr;
        if (modelName.empty())
        {
            id = XPMPCreatePlane(aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), getPlaneData, static_cast<void *>(this));
        }
        else
        {
            id = XPMPCreatePlaneWithModelName(modelName.c_str(), aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), getPlaneData, planeLoaded, static_cast<void *>(this));
        }

        if (id)
        {
            Plane *plane = new Plane(id, callsign, aircraftIcao, airlineIcao, livery, modelName);
            m_planesByCallsign[callsign] = plane;
            m_planesById[id] = plane;

            // Create view menu item
            CMenuItem planeViewMenuItem = m_planeViewSubMenu.item(callsign, [this, callsign] { orbitRemotePlane(callsign); });
            m_planeViewMenuItems[callsign] = planeViewMenuItem;
        }
    }

    void CTraffic::removePlane(const std::string &callsign)
    {
        auto menuItemIt = m_planeViewMenuItems.find(callsign);
        if (menuItemIt != m_planeViewMenuItems.end())
        {
            m_planeViewSubMenu.removeItem(menuItemIt->second);
            m_planeViewMenuItems.erase(menuItemIt);
        }

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

        for (const auto &kv : m_planeViewMenuItems)
        {
            CMenuItem item = kv.second;
            m_planeViewSubMenu.removeItem(item);
        }

        m_planesByCallsign.clear();
        m_planesById.clear();
        m_planeViewMenuItems.clear();
    }

    void CTraffic::setPlanePositions(const std::vector<std::string> &callsigns, std::vector<double> latitudes, std::vector<double> longitudes, std::vector<double> altitudes,
                                     std::vector<double> pitches, std::vector<double> rolles, std::vector<double> headings)
    {
        for (size_t i = 0; i < callsigns.size(); i++)
        {
            auto planeIt = m_planesByCallsign.find(callsigns.at(i));
            if (planeIt == m_planesByCallsign.end()) { return; }

            Plane *plane = planeIt->second;
            if (!plane) { return; }
            plane->position.lat = latitudes.at(i);
            plane->position.lon = longitudes.at(i);
            plane->position.elevation = altitudes.at(i);
            plane->position.pitch = static_cast<float>(pitches.at(i));
            plane->position.roll = static_cast<float>(rolles.at(i));
            plane->position.heading = static_cast<float>(headings.at(i));
        }
    }

    void CTraffic::setPlaneSurfaces(const std::vector<std::string> &callsign, const std::vector<double> &gear, const std::vector<double> &flap, const std::vector<double> &spoiler,
                                    const std::vector<double> &speedBrake, const std::vector<double> &slat, const std::vector<double> &wingSweep, const std::vector<double> &thrust,
                                    const std::vector<double> &elevator, const std::vector<double> &rudder, const std::vector<double> &aileron, const std::vector<bool> &landLight,
                                    const std::vector<bool> &beaconLight, const std::vector<bool> &strobeLight, const std::vector<bool> &navLight, const std::vector<int> &lightPattern, const std::vector<bool> &onGround)
    {
        (void)onGround;

        for (size_t i = 0; i < callsign.size(); i++)
        {
            auto planeIt = m_planesByCallsign.find(callsign.at(i));
            if (planeIt == m_planesByCallsign.end()) { return; }

            Plane *plane = planeIt->second;
            if (!plane) { return; }

            plane->hasSurfaces = true;
            plane->targetGearPosition = static_cast<float>(gear.at(i));
            plane->surfaces.flapRatio = static_cast<float>(flap.at(i));
            plane->surfaces.spoilerRatio = static_cast<float>(spoiler.at(i));
            plane->surfaces.speedBrakeRatio = static_cast<float>(speedBrake.at(i));
            plane->surfaces.slatRatio = static_cast<float>(slat.at(i));
            plane->surfaces.wingSweep = static_cast<float>(wingSweep.at(i));
            plane->surfaces.thrust = static_cast<float>(thrust.at(i));
            plane->surfaces.yokePitch = static_cast<float>(elevator.at(i));
            plane->surfaces.yokeHeading = static_cast<float>(rudder.at(i));
            plane->surfaces.yokeRoll = static_cast<float>(aileron.at(i));
            plane->surfaces.lights.landLights = landLight.at(i);
            plane->surfaces.lights.bcnLights = beaconLight.at(i);
            plane->surfaces.lights.strbLights = strobeLight.at(i);
            plane->surfaces.lights.navLights = navLight.at(i);
            plane->surfaces.lights.flashPattern = lightPattern.at(i);
        }
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

    void CTraffic::getRemoteAircraftsData(std::vector<std::string> &callsigns, std::vector<double> &latitudesDeg, std::vector<double> &longitudesDeg,
                                          std::vector<double> &elevationsM, std::vector<double> &verticalOffsets)
    {
        if (callsigns.empty() || m_planesByCallsign.empty()) { return; }

        const auto requestedCallsigns = callsigns;
        callsigns.clear();
        latitudesDeg.clear();
        longitudesDeg.clear();
        elevationsM.clear();
        verticalOffsets.clear();

        for (const auto &requestedCallsign : requestedCallsigns)
        {
            auto planeIt = m_planesByCallsign.find(requestedCallsign);
            if (planeIt == m_planesByCallsign.end()) { continue; }

            Plane *plane = planeIt->second;
            assert(plane);

            double lat = plane->position.lat;
            double lon = plane->position.lon;
            double groundElevation = plane->terrainProbe.getElevation(lat, lon, plane->position.elevation);
            if (std::isnan(groundElevation)) { groundElevation = 0.0; }
            double fudgeFactor = 3.0;
            XPMPGetVerticalOffset(plane->id, &fudgeFactor);

            callsigns.push_back(requestedCallsign);
            latitudesDeg.push_back(lat);
            longitudesDeg.push_back(lon);
            elevationsM.push_back(groundElevation);
            verticalOffsets.push_back(fudgeFactor);
        }
    }

    double CTraffic::getEelevationAtPosition(const std::string &callsign, double latitude, double longitude, double altitude)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt != m_planesByCallsign.end())
        {
            Plane *plane = planeIt->second;
            return plane->terrainProbe.getElevation(latitude, longitude, altitude);
        }
        else
        {
            return m_terrainProbe.getElevation(latitude, longitude, altitude);
        }
    }

    void CTraffic::setFollowedAircraft(const std::string &callsign)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        orbitRemotePlane(callsign);
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
            else if (message.getMethodName() == "setPlanePositions")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::vector<std::string> callsigns;
                std::vector<double> latitudes;
                std::vector<double> longitudes;
                std::vector<double> altitudes;
                std::vector<double> pitches;
                std::vector<double> rolles;
                std::vector<double> headings;
                message.beginArgumentRead();
                message.getArgument(callsigns);
                message.getArgument(latitudes);
                message.getArgument(longitudes);
                message.getArgument(altitudes);
                message.getArgument(pitches);
                message.getArgument(rolles);
                message.getArgument(headings);
                queueDBusCall([ = ]()
                {
                    setPlanePositions(callsigns, latitudes, longitudes, altitudes, pitches, rolles, headings);
                });
            }
            else if (message.getMethodName() == "setPlaneSurfaces")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::vector<std::string> callsign;
                std::vector<double> gear;
                std::vector<double> flap;
                std::vector<double> spoiler;
                std::vector<double> speedBrake;
                std::vector<double> slat;
                std::vector<double> wingSweep;
                std::vector<double> thrust;
                std::vector<double> elevator;
                std::vector<double> rudder;
                std::vector<double> aileron;
                std::vector<bool> landLight;
                std::vector<bool> beaconLight;
                std::vector<bool> strobeLight;
                std::vector<bool> navLight;
                std::vector<int> lightPattern;
                std::vector<bool> onGround;
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
            else if (message.getMethodName() == "getRemoteAircraftsData")
            {
                std::vector<std::string> requestedcallsigns;
                message.beginArgumentRead();
                message.getArgument(requestedcallsigns);
                queueDBusCall([ = ]()
                {
                    std::vector<std::string> callsigns = requestedcallsigns;
                    std::vector<double> latitudesDeg;
                    std::vector<double> longitudesDeg;
                    std::vector<double> elevationsM;
                    std::vector<double> verticalOffsets;
                    getRemoteAircraftsData(callsigns, latitudesDeg, longitudesDeg, elevationsM, verticalOffsets);
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(callsigns);
                    reply.appendArgument(latitudesDeg);
                    reply.appendArgument(longitudesDeg);
                    reply.appendArgument(elevationsM);
                    reply.appendArgument(verticalOffsets);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getEelevationAtPosition")
            {
                std::string callsign;
                double latitude;
                double longitude;
                double altitude;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(latitude);
                message.getArgument(longitude);
                message.getArgument(altitude);
                queueDBusCall([ = ]()
                {
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(callsign);
                    reply.appendArgument(getEelevationAtPosition(callsign, latitude, longitude, altitude));
                    ;                   sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "setFollowedAircraft")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::string callsign;
                message.beginArgumentRead();
                message.getArgument(callsign);
                queueDBusCall([ = ]()
                {
                    setFollowedAircraft(callsign);
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

    int CTraffic::orbitPlaneFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon)
    {
        auto *traffic = static_cast<CTraffic *>(refcon);

        if (isLosingControl == 1)
        {
            // traffic->m_planeViewCallsign.clear();
            return 0;
        }

        if (cameraPosition)
        {
            int w, h, x, y;
            // First get the screen size and mouse location. We will use this to decide
            // what part of the orbit we are in. The mouse will move us up-down and around.
            // fixme: In a future update, change the orbit only while right mouse button is pressed.
            XPLMGetScreenSize(&w, &h);
            XPLMGetMouseLocation(&x, &y);
            double heading = 360.0 * static_cast<double>(x) / static_cast<double>(w);
            double pitch = 20.0 * ((static_cast<double>(y) / static_cast<double>(h)) * 2.0 - 1.0);

            // Now calculate where the camera should be positioned to be 200
            // meters from the plane and pointing at the plane at the pitch and
            // heading we wanted above.
            static const double PI = std::acos(-1);
            double dx = -50.0 * sin(heading * PI / 180.0);
            double dz = 50.0 * cos(heading * PI / 180.0);
            double dy = -50.0 * tan(pitch * PI / 180.0);

            auto planeIt = traffic->m_planesByCallsign.find(traffic->m_planeViewCallsign);
            if (planeIt == traffic->m_planesByCallsign.end()) { return 0; }
            Plane *plane = planeIt->second;

            double lx, ly, lz;
            static const double kFtToMeters = 0.3048;
            XPLMWorldToLocal(plane->position.lat, plane->position.lon, plane->position.elevation * kFtToMeters, &lx, &ly, &lz);

            // Fill out the camera position info.
            cameraPosition->x = static_cast<float>(lx + dx);
            cameraPosition->y = static_cast<float>(ly + dy);
            cameraPosition->z = static_cast<float>(lz + dz);
            cameraPosition->pitch = static_cast<float>(pitch);
            cameraPosition->heading = static_cast<float>(heading);
            cameraPosition->roll = 0;
        }

        // Return 1 to indicate we want to keep controlling the camera.
        return 1;
    }

    int CTraffic::drawCallback(XPLMDrawingPhase phase, int isBefore, void *refcon)
    {
        (void)phase;
        (void)isBefore;
        CTraffic *traffic = static_cast<CTraffic *>(refcon);

        // The draw callback is called several times per frame. We need this only once.
        if (traffic->m_worldRenderType.get() == 0)
        {
            traffic->emitSimFrame();
        }

        return 1;
    }
}

//! \endcond
