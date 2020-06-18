/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "plugin.h"
#include "traffic.h"
#include "utils.h"
#include "XPMPMultiplayer.h"
#include "XPMPPlaneRenderer.h"
#include "XPLMGraphics.h"
#include <XPLM/XPLMProcessing.h>
#include <XPLM/XPLMUtilities.h>
#include <XPLM/XPLMPlanes.h>
#include <XPLM/XPLMPlugin.h>
#include "blackmisc/simulation/xplane/qtfreeutils.h"
#include <cassert>
#include <cstring>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <limits>

// clazy:excludeall=reserve-candidates

using namespace BlackMisc::Simulation::XPlane::QtFreeUtils;

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

    CTraffic *CTraffic::s_instance = nullptr;

    // *INDENT-OFF*
    CTraffic::CTraffic(CSettingsProvider *settingsProvider) :
        CDBusObject(settingsProvider),
        m_followPlaneViewNextCommand("org/swift-project/xswiftbus/follow_next_plane", "Changes plane view to follow next plane in sequence", [this] { followNextPlane(); }),
        m_followPlaneViewPreviousCommand("org/swift-project/xswiftbus/follow_previous_plane", "Changes plane view to follow previous plane in sequence", [this] { followPreviousPlane(); })
    {
        assert(!s_instance);
        s_instance = this;
        XPLMRegisterDrawCallback(drawCallback, xplm_Phase_Airplanes, 1, this);
        XPLMRegisterKeySniffer(followAircraftKeySniffer, 1, this);

        // init labels
        this->setDrawingLabels(this->getSettings().isDrawingLabels());
    }
    // *INDENT-ON*

    CTraffic::~CTraffic()
    {
        XPLMUnregisterDrawCallback(drawCallback, xplm_Phase_Airplanes, 1, this);
        cleanup();
        assert(s_instance == this);
        s_instance = nullptr;
    }

    static bool s_legacyDataOK = true;

    void CTraffic::setPlaneViewMenu(const CMenu &planeViewSubMenu)
    {
        m_followPlaneViewSubMenu = planeViewSubMenu;
    }

    void CTraffic::initLegacyData()
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "LegacyData" + g_sep;

        std::string csl = dir + "CSL";
        std::string related = dir + "related.txt";
        std::string doc8643 = dir + "Doc8643.txt";
        std::string lights  = dir + "lights.png";

        auto err = XPMPMultiplayerInitLegacyData(csl.c_str(), related.c_str(), lights.c_str(), doc8643.c_str(),
                   "C172", preferences, preferences);
        if (*err) { s_legacyDataOK = false; }
    }

    bool CTraffic::initialize()
    {
        if (! s_legacyDataOK) { return false; }

        if (! m_initialized)
        {
            auto err = XPMPMultiplayerInit(preferences, preferences);
            if (*err) { cleanup(); }
            else { m_initialized = true; }
        }

        return m_initialized;
    }

    bool CTraffic::acquireMultiplayerPlanes(std::string *owner)
    {
        if (! m_enabledMultiplayer)
        {
            auto err = XPMPMultiplayerEnable();
            if (*err)
            {
                cleanup();
            }
            else
            {
                m_enabledMultiplayer = true;
                XPMPLoadPlanesIfNecessary();
            }
        }

        int totalAircraft;
        int activeAircraft;
        XPLMPluginID controller;
        XPLMCountAircraft(&totalAircraft, &activeAircraft, &controller);

        char pluginName[256];
        XPLMGetPluginInfo(controller, pluginName, nullptr, nullptr, nullptr);
        *owner = std::string(pluginName);
        return m_enabledMultiplayer;
    }

    void CTraffic::cleanup()
    {
        removeAllPlanes();

        if (m_enabledMultiplayer)
        {
            m_enabledMultiplayer = false;
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

    void CTraffic::switchToFollowPlaneView(const std::string &callsign)
    {
        if (CTraffic::ownAircraftString() != callsign && !this->containsCallsign(callsign))
        {
            INFO_LOG("Cannot switch to follow " + callsign);
            return;
        }

        m_followPlaneViewCallsign = callsign;

        /* This is the hotkey callback.  First we simulate a joystick press and
         * release to put us in 'free view 1'.  This guarantees that no panels
         * are showing and we are an external view. */
        XPLMCommandButtonPress(xplm_joy_v_fr1);
        XPLMCommandButtonRelease(xplm_joy_v_fr1);

        /* Now we control the camera until the view changes. */
        INFO_LOG("Switch to follow " + callsign);
        XPLMControlCamera(xplm_ControlCameraUntilViewChanges, CTraffic::orbitPlaneFunc, this);
    }

    void CTraffic::followNextPlane()
    {
        if (m_planesByCallsign.empty() || m_followPlaneViewCallsign.empty()) { return; }
        auto callsignIt = std::find(m_followPlaneViewSequence.begin(), m_followPlaneViewSequence.end(), m_followPlaneViewCallsign);

        // If we are not at the end, increase by one
        if (callsignIt != m_followPlaneViewSequence.end()) { callsignIt++; }
        // If we were already at the end or reached it now, start from the beginning
        if (callsignIt == m_followPlaneViewSequence.end()) { callsignIt = m_followPlaneViewSequence.begin(); }

        m_followPlaneViewCallsign = *callsignIt;
    }

    void CTraffic::followPreviousPlane()
    {
        if (m_planesByCallsign.empty() || m_followPlaneViewCallsign.empty()) { return; }
        auto callsignIt = std::find(m_followPlaneViewSequence.rbegin(), m_followPlaneViewSequence.rend(), m_followPlaneViewCallsign);

        // If we are not at the end, increase by one
        if (callsignIt != m_followPlaneViewSequence.rend()) { callsignIt++; }
        // If we were already at the end or reached it now, start from the beginning
        if (callsignIt == m_followPlaneViewSequence.rend()) { callsignIt = m_followPlaneViewSequence.rbegin(); }

        m_followPlaneViewCallsign = *callsignIt;
    }

    bool CTraffic::containsCallsign(const std::string &callsign) const
    {
        //! \fixme can be removed with C++20, as it then has a contains function
        if (callsign.empty()) { return false; }
        const auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return false; }
        return true;
    }

    // changed T709
    // static int g_maxPlanes = 100;
    // static float g_drawDistance = 50.0f;

    int CTraffic::preferences(const char *section, const char *name, int def)
    {
        if (!s_instance) { return def; }

        if (strcmp(section, "planes") == 0 && strcmp(name, "max_full_count") == 0)
        {
            return s_instance->getSettings().getMaxPlanes(); // preferences
        }
        else if (strcmp(section, "debug") == 0 && strcmp(name, "allow_obj8_async_load") == 0)
        {
            // the setting allow_obj8_async_load (in the debug section) controls
            // whether or not async loading is enabled: 1 means enabled, 0 means disabled
            return 1;
        }
        else if (strcmp(section, "debug") == 0 && strcmp(name, "render_phases") == 0)
        {
            return s_instance->getConfig().getDebugMode() ? 1 : 0;
        }
        else if (strcmp(section, "debug") == 0 && strcmp(name, "tcas_traffic") == 0)
        {
            return s_instance->getConfig().getTcasEnabled() ? 1 : 0;
        }
        return def;
    }

    float CTraffic::preferences(const char *section, const char *name, float def)
    {
        if (!s_instance) { return def; }

        if (strcmp(section, "planes") == 0 && strcmp(name, "full_distance") == 0)
        {
            return static_cast<float>(s_instance->getSettings().getMaxDrawDistanceNM()); // preferences
        }
        return def;
    }

    std::string CTraffic::loadPlanesPackage(const std::string &path)
    {
        initXPlanePath();
        auto dir = g_xplanePath + "Resources" + g_sep + "plugins" + g_sep + "xswiftbus" + g_sep + "LegacyData" + g_sep;

        std::string related = dir + "related.txt";
        std::string doc8643 = dir + "Doc8643.txt";
        auto err = XPMPLoadCSLPackage(path.c_str(), related.c_str(), doc8643.c_str());
        if (*err) { return err; }
        return {};
    }

    void CTraffic::setDefaultIcao(const std::string &defaultIcao)
    {
        XPMPSetDefaultPlaneICAO(defaultIcao.c_str());
    }

    void CTraffic::setDrawingLabels(bool drawing)
    {
        CSettings s = this->getSettings();
        if (s.isDrawingLabels() != drawing)
        {
            s.setDrawingLabels(drawing);
            this->setSettings(s);
        }

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
        CSettings s = this->getSettings();
        if (s.setMaxPlanes(planes)) { this->setSettings(s); }
    }

    void CTraffic::setMaxDrawDistance(double nauticalMiles)
    {
        CSettings s = this->getSettings();
        if (s.setMaxDrawDistanceNM(nauticalMiles)) { this->setSettings(s); }
    }

    void CTraffic::addPlane(const std::string &callsign, const std::string &modelName, const std::string &aircraftIcao, const std::string &airlineIcao, const std::string &livery)
    {
        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt != m_planesByCallsign.end()) { return; }

        XPMPPlaneID id = nullptr;
        if (modelName.empty())
        {
            id = XPMPCreatePlane(aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), getPlaneData, planeLoaded, static_cast<void *>(this));
        }
        else
        {
            const std::string nt = this->getSettings().getNightTextureMode();
            id = XPMPCreatePlaneWithModelName(modelName.c_str(), aircraftIcao.c_str(), airlineIcao.c_str(), livery.c_str(), nt.c_str(), getPlaneData, planeLoaded, static_cast<void *>(this));
        }

        if (!id)
        {
            emitPlaneAddingFailed(callsign);
            return;
        }

        Plane *plane = new Plane(id, callsign, aircraftIcao, airlineIcao, livery, modelName);
        m_planesByCallsign[callsign] = plane;
        m_planesById[id] = plane;

        // Create view menu item
        CMenuItem planeViewMenuItem = m_followPlaneViewSubMenu.item(callsign, [this, callsign] { switchToFollowPlaneView(callsign); });
        m_followPlaneViewMenuItems[callsign] = planeViewMenuItem;
        m_followPlaneViewSequence.push_back(callsign);
    }

    void CTraffic::removePlane(const std::string &callsign)
    {
        auto menuItemIt = m_followPlaneViewMenuItems.find(callsign);
        if (menuItemIt != m_followPlaneViewMenuItems.end())
        {
            m_followPlaneViewSubMenu.removeItem(menuItemIt->second);
            m_followPlaneViewMenuItems.erase(menuItemIt);
        }

        auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt == m_planesByCallsign.end()) { return; }

        m_followPlaneViewSequence.erase(std::remove(m_followPlaneViewSequence.begin(), m_followPlaneViewSequence.end(), callsign),
                                        m_followPlaneViewSequence.end());

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

        for (const auto &kv : m_followPlaneViewMenuItems)
        {
            CMenuItem item = kv.second;
            m_followPlaneViewSubMenu.removeItem(item);
        }

        m_planesByCallsign.clear();
        m_planesById.clear();
        m_followPlaneViewMenuItems.clear();
        m_followPlaneViewSequence.clear();
    }

    void CTraffic::setPlanesPositions(const std::vector<std::string> &callsigns, std::vector<double> latitudesDeg, std::vector<double> longitudesDeg, std::vector<double> altitudesFt,
                                      std::vector<double> pitchesDeg, std::vector<double> rollsDeg, std::vector<double> headingsDeg, const std::vector<bool> &onGrounds)
    {
        const bool setOnGround = onGrounds.size() == callsigns.size();
        for (size_t i = 0; i < callsigns.size(); i++)
        {
            auto planeIt = m_planesByCallsign.find(callsigns.at(i));
            if (planeIt == m_planesByCallsign.end()) { continue; }

            Plane *plane = planeIt->second;
            if (!plane) { continue; }
            plane->position.lat = latitudesDeg.at(i);
            plane->position.lon = longitudesDeg.at(i);
            plane->position.elevation = altitudesFt.at(i);
            plane->position.pitch   = static_cast<float>(pitchesDeg.at(i));
            plane->position.roll    = static_cast<float>(rollsDeg.at(i));
            plane->position.heading = static_cast<float>(headingsDeg.at(i));

            if (setOnGround) { plane->isOnGround = onGrounds.at(i); }
        }
    }

    void CTraffic::setPlanesSurfaces(const std::vector<std::string> &callsigns, const std::vector<double> &gears, const std::vector<double> &flaps, const std::vector<double> &spoilers,
                                     const std::vector<double> &speedBrakes, const std::vector<double> &slats, const std::vector<double> &wingSweeps, const std::vector<double> &thrusts,
                                     const std::vector<double> &elevators, const std::vector<double> &rudders, const std::vector<double> &ailerons,
                                     const std::vector<bool> &landLights, const std::vector<bool> &taxiLights,
                                     const std::vector<bool> &beaconLights, const std::vector<bool> &strobeLights, const std::vector<bool> &navLights, const std::vector<int> &lightPatterns)
    {
        const bool bundleTaxiLandingLights = this->getSettings().isBundlingTaxiAndLandingLights();

        for (size_t i = 0; i < callsigns.size(); i++)
        {
            auto planeIt = m_planesByCallsign.find(callsigns.at(i));
            if (planeIt == m_planesByCallsign.end()) { continue; }

            Plane *plane = planeIt->second;
            if (!plane) { continue; }

            plane->hasSurfaces = true;
            plane->targetGearPosition = static_cast<float>(gears.at(i));
            plane->surfaces.flapRatio = static_cast<float>(flaps.at(i));
            plane->surfaces.spoilerRatio = static_cast<float>(spoilers.at(i));
            plane->surfaces.speedBrakeRatio = static_cast<float>(speedBrakes.at(i));
            plane->surfaces.slatRatio = static_cast<float>(slats.at(i));
            plane->surfaces.wingSweep = static_cast<float>(wingSweeps.at(i));
            plane->surfaces.thrust = static_cast<float>(thrusts.at(i));
            plane->surfaces.yokePitch = static_cast<float>(elevators.at(i));
            plane->surfaces.yokeHeading = static_cast<float>(rudders.at(i));
            plane->surfaces.yokeRoll = static_cast<float>(ailerons.at(i));
            if (bundleTaxiLandingLights)
            {
                const bool on = landLights.at(i) || taxiLights.at(i);
                plane->surfaces.lights.landLights = on;
                plane->surfaces.lights.taxiLights = on;
            }
            else
            {
                plane->surfaces.lights.landLights = landLights.at(i);
                plane->surfaces.lights.taxiLights = taxiLights.at(i);
            }
            plane->surfaces.lights.bcnLights = beaconLights.at(i);
            plane->surfaces.lights.strbLights = strobeLights.at(i);
            plane->surfaces.lights.navLights = navLights.at(i);
            plane->surfaces.lights.flashPattern = static_cast<unsigned int>(lightPatterns.at(i));
        }
    }

    void CTraffic::setPlanesTransponders(const std::vector<std::string> &callsigns, const std::vector<int> &codes, const std::vector<bool> &modeCs, const std::vector<bool> &idents)
    {
        for (size_t i = 0; i < callsigns.size(); i++)
        {
            auto planeIt = m_planesByCallsign.find(callsigns.at(i));
            if (planeIt == m_planesByCallsign.end()) { continue; }

            Plane *plane = planeIt->second;
            if (!plane) { continue; }

            plane->hasXpdr = true;
            plane->xpdr.code = codes.at(i);
            if (idents.at(i)) { plane->xpdr.mode = xpmpTransponderMode_ModeC_Ident; }
            else if (modeCs.at(i)) { plane->xpdr.mode = xpmpTransponderMode_ModeC; }
            else { plane->xpdr.mode = xpmpTransponderMode_Standby; }
        }
    }

    void CTraffic::getRemoteAircraftData(std::vector<std::string> &callsigns, std::vector<double> &latitudesDeg, std::vector<double> &longitudesDeg,
                                         std::vector<double> &elevationsM, std::vector<bool> &waterFlags, std::vector<double> &verticalOffsets) const
    {
        if (callsigns.empty() || m_planesByCallsign.empty()) { return; }

        const auto requestedCallsigns = callsigns;
        callsigns.clear();
        latitudesDeg.clear();
        longitudesDeg.clear();
        elevationsM.clear();
        verticalOffsets.clear();
        waterFlags.clear();

        for (const auto &requestedCallsign : requestedCallsigns)
        {
            const auto planeIt = m_planesByCallsign.find(requestedCallsign);
            if (planeIt == m_planesByCallsign.end()) { continue; }

            const Plane *plane = planeIt->second;
            assert(plane);

            const double latDeg = plane->position.lat;
            const double lonDeg = plane->position.lon;
            double groundElevation = 0.0;
            bool isWater = false;
            if (getSettings().isTerrainProbeEnabled())
            {
                // we expect elevation in meters
                groundElevation = plane->terrainProbe.getElevation(latDeg, lonDeg, plane->position.elevation, requestedCallsign, isWater).front();
                if (std::isnan(groundElevation)) { groundElevation = 0.0; }
            }
            double fudgeFactor = 3.0;
            bool hasOffset = XPMPGetVerticalOffset(plane->id, &fudgeFactor);

            callsigns.push_back(requestedCallsign);
            latitudesDeg.push_back(latDeg);
            longitudesDeg.push_back(lonDeg);
            elevationsM.push_back(groundElevation);
            waterFlags.push_back(isWater);
            verticalOffsets.push_back(hasOffset ? fudgeFactor : std::numeric_limits<decltype(fudgeFactor)>::quiet_NaN());
        }
    }

    std::array<double, 3> CTraffic::getElevationAtPosition(const std::string &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters, bool &o_isWater) const
    {
        if (!getSettings().isTerrainProbeEnabled()) { return {{ std::numeric_limits<double>::quiet_NaN(), latitudeDeg, longitudeDeg }}; }

        const auto planeIt = m_planesByCallsign.find(callsign);
        if (planeIt != m_planesByCallsign.end())
        {
            const Plane *plane = planeIt->second;
            return plane->terrainProbe.getElevation(latitudeDeg, longitudeDeg, altitudeMeters, callsign, o_isWater);
        }
        else
        {
            return m_terrainProbe.getElevation(latitudeDeg, longitudeDeg, altitudeMeters, callsign + " (plane not found)", o_isWater);
        }
    }

    void CTraffic::setFollowedAircraft(const std::string &callsign)
    {
        this->switchToFollowPlaneView(callsign);
    }

    void CTraffic::dbusDisconnectedHandler()
    {
        removeAllPlanes();
    }

    static const char *introspection_traffic =
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
#include "org.swift_project.xswiftbus.traffic.xml"
        ;

    DBusHandlerResult CTraffic::dbusMessageHandler(const CDBusMessage &message_)
    {
        CDBusMessage message(message_);
        const std::string sender = message.getSender();
        const dbus_uint32_t serial = message.getSerial();
        const bool wantsReply = message.wantsReply();

        // Debug message if needed
        // { const std::string d = "dbusMessageHandler: " + message.getMethodName(); INFO_LOG(d.c_str()); }

        if (message.getInterfaceName() == DBUS_INTERFACE_INTROSPECTABLE)
        {
            if (message.getMethodName() == "Introspect")
            {
                sendDBusReply(sender, serial, introspection_traffic);
            }
        }
        else if (message.getInterfaceName() == XSWIFTBUS_TRAFFIC_INTERFACENAME)
        {
            if (message.getMethodName() == "acquireMultiplayerPlanes")
            {
                queueDBusCall([ = ]()
                {
                    std::string owner;
                    bool acquired = acquireMultiplayerPlanes(&owner);
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(acquired);
                    reply.appendArgument(owner);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "initialize")
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
            else if (message.getMethodName() == "setPlanesPositions")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::vector<std::string> callsigns;
                std::vector<double> latitudes;
                std::vector<double> longitudes;
                std::vector<double> altitudes;
                std::vector<double> pitches;
                std::vector<double> rolls;
                std::vector<double> headings;
                std::vector<bool> onGrounds;
                message.beginArgumentRead();
                message.getArgument(callsigns);
                message.getArgument(latitudes);
                message.getArgument(longitudes);
                message.getArgument(altitudes);
                message.getArgument(pitches);
                message.getArgument(rolls);
                message.getArgument(headings);
                message.getArgument(onGrounds);
                queueDBusCall([ = ]()
                {
                    setPlanesPositions(callsigns, latitudes, longitudes, altitudes, pitches, rolls, headings, onGrounds);
                });
            }
            else if (message.getMethodName() == "setPlanesSurfaces")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::vector<std::string> callsigns;
                std::vector<double> gears;
                std::vector<double> flaps;
                std::vector<double> spoilers;
                std::vector<double> speedBrakes;
                std::vector<double> slats;
                std::vector<double> wingSweeps;
                std::vector<double> thrusts;
                std::vector<double> elevators;
                std::vector<double> rudders;
                std::vector<double> ailerons;
                std::vector<bool> landLights;
                std::vector<bool> taxiLights;
                std::vector<bool> beaconLights;
                std::vector<bool> strobeLights;
                std::vector<bool> navLights;
                std::vector<int> lightPatterns;
                message.beginArgumentRead();
                message.getArgument(callsigns);
                message.getArgument(gears);
                message.getArgument(flaps);
                message.getArgument(spoilers);
                message.getArgument(speedBrakes);
                message.getArgument(slats);
                message.getArgument(wingSweeps);
                message.getArgument(thrusts);
                message.getArgument(elevators);
                message.getArgument(rudders);
                message.getArgument(ailerons);
                message.getArgument(landLights);
                message.getArgument(taxiLights);
                message.getArgument(beaconLights);
                message.getArgument(strobeLights);
                message.getArgument(navLights);
                message.getArgument(lightPatterns);
                queueDBusCall([ = ]()
                {
                    setPlanesSurfaces(callsigns, gears, flaps, spoilers, speedBrakes, slats, wingSweeps, thrusts, elevators,
                                      rudders, ailerons, landLights, taxiLights, beaconLights, strobeLights, navLights, lightPatterns);
                });
            }
            else if (message.getMethodName() == "setPlanesTransponders")
            {
                maybeSendEmptyDBusReply(wantsReply, sender, serial);
                std::vector<std::string> callsigns;
                std::vector<int> codes;
                std::vector<bool> modeCs;
                std::vector<bool> idents;
                message.beginArgumentRead();
                message.getArgument(callsigns);
                message.getArgument(codes);
                message.getArgument(modeCs);
                message.getArgument(idents);
                queueDBusCall([ = ]()
                {
                    setPlanesTransponders(callsigns, codes, modeCs, idents);
                });
            }
            else if (message.getMethodName() == "getRemoteAircraftData")
            {
                std::vector<std::string> requestedCallsigns;
                message.beginArgumentRead();
                message.getArgument(requestedCallsigns);
                queueDBusCall([ = ]()
                {
                    std::vector<std::string> callsigns = requestedCallsigns;
                    std::vector<double> latitudesDeg;
                    std::vector<double> longitudesDeg;
                    std::vector<double> elevationsM;
                    std::vector<bool>   waterFlags;
                    std::vector<double> verticalOffsets;
                    getRemoteAircraftData(callsigns, latitudesDeg, longitudesDeg, elevationsM, waterFlags, verticalOffsets);
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(callsigns);
                    reply.appendArgument(latitudesDeg);
                    reply.appendArgument(longitudesDeg);
                    reply.appendArgument(elevationsM);
                    reply.appendArgument(waterFlags);
                    reply.appendArgument(verticalOffsets);
                    sendDBusMessage(reply);
                });
            }
            else if (message.getMethodName() == "getElevationAtPosition")
            {
                std::string callsign;
                double latitudeDeg;
                double longitudeDeg;
                double altitudeMeters;
                message.beginArgumentRead();
                message.getArgument(callsign);
                message.getArgument(latitudeDeg);
                message.getArgument(longitudeDeg);
                message.getArgument(altitudeMeters);
                queueDBusCall([ = ]()
                {
                    bool isWater = false;
                    const auto elevation = getElevationAtPosition(callsign, latitudeDeg, longitudeDeg, altitudeMeters, isWater);
                    CDBusMessage reply = CDBusMessage::createReply(sender, serial);
                    reply.beginArgumentWrite();
                    reply.appendArgument(callsign);
                    reply.appendArgument(elevation[0]);
                    reply.appendArgument(elevation[1]);
                    reply.appendArgument(elevation[2]);
                    reply.appendArgument(isWater);
                    sendDBusMessage(reply);
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

    int CTraffic::process()
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
        const auto planeIt = m_planesById.find(id);
        // assert(planeIt != m_planesById.end());
        if (planeIt == m_planesById.end()) { return xpmpData_Unavailable; } // less drastic version
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

                static const float epsilon = std::numeric_limits<float>::epsilon();
                const float f = plane->surfaces.gearPosition - plane->targetGearPosition;
                if (std::abs(f) > epsilon)
                {
                    // interpolate gear position
                    constexpr float gearMoveTimeMs = 5000;
                    const auto gearPositionDiffRemaining = plane->targetGearPosition - plane->surfaces.gearPosition;

                    const auto diffMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - plane->prevSurfacesLerpTime);
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
            break;

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
        }
        return xpmpData_Unavailable;
    }

    int CTraffic::orbitPlaneFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon)
    {
        constexpr bool DEBUG = false;
        // DEBUG_LOG_C("Follow aircraft entry", DEBUG);
        if (isLosingControl == 1)
        {
            // do NOT use refcon here, might be invalid
            INFO_LOG("Loosing camera control");
            return 0;
        }

        auto *traffic = static_cast<CTraffic *>(refcon);
        if (!traffic)
        {
            ERROR_LOG("Cannot convert CTraffic object");
            return 0;
        }

        // nothing we can do
        if (!cameraPosition)
        {
            ERROR_LOG("No camera object");
            traffic->m_followPlaneViewCallsign.clear();
            return 0;
        }

        // Ideally we would like to test against right mouse button, but X-Plane SDK does not
        // allow that.
        if (!traffic->m_deltaCameraPosition.isInitialized || traffic->m_isSpacePressed)
        {
            int w = 0, h = 0, x = 0, y = 0;
            // First get the screen size and mouse location. We will use this to decide
            // what part of the orbit we are in. The mouse will move us up-down and around.
            // fixme: In a future update, change the orbit only while right mouse button is pressed.
            XPLMGetScreenSize(&w, &h);
            XPLMGetMouseLocation(&x, &y);
            // cppcheck-suppress knownConditionTrueFalse
            if (DEBUG) { DEBUG_LOG("Follow aircraft coordinates w,h,x,y: " + std::to_string(w) + " " + std::to_string(h) + " " + std::to_string(x) + " " + std::to_string(y)); }
            if (traffic->m_lastMouseX == x && traffic->m_lastMouseY == y && traffic->m_lastMouseX >= 0 && traffic->m_lastMouseY >= 0)
            {
                // mouse NOT moving, we lost focus or we do NOT move anymore
                // to avoid issues we reset the space key, see https://discordapp.com/channels/539048679160676382/539925070550794240/614162134644949002
                traffic->m_isSpacePressed = false;
            }

            // avoid follow aircraft in too small windows
            // int cannot be NaN
            if (w < 100 || h < 100)
            {
                WARNING_LOG("Screen w/h too small " + std::to_string(w) + "/" + std::to_string(h));
                return 0;
            }

            if (x < 1 || y < 1 || x >= w || y >= h)
            {
                WARNING_LOG("Screen w/h, and x/y" + std::to_string(w) + "/" + std::to_string(h) + " | " + std::to_string(x) + "/" + std::to_string(y));
                return 0;
            }

            // the 1.25 factor allows to turn around completely
            traffic->m_deltaCameraPosition.headingDeg = normalizeToZero360Deg(1.25 * 360.0 * static_cast<double>(x) / static_cast<double>(w)); // range 0-360
            double usedCameraPitchDeg                 = 60.0 - (60.0 * 2.0 * static_cast<double>(y) / static_cast<double>(h)); // range +-

            // make sure we can use it with tan in range +-90 degrees and the result of tan not getting too high
            // we limit to +-85deg, tan 45deg: 1 | tan 60deg: 1.73 | tan 85deg: 11.4
            if (usedCameraPitchDeg >= 85.0) { usedCameraPitchDeg = 85.0; }
            else if (usedCameraPitchDeg <= -85.0) { usedCameraPitchDeg = -85.0; }
            traffic->m_deltaCameraPosition.pitchDeg = usedCameraPitchDeg;

            // Now calculate where the camera should be positioned to be x
            // meters from the plane and pointing at the plane at the pitch and
            // heading we wanted above.
            const double distanceMeterM = traffic->m_followAircraftDistanceMultiplier * static_cast<double>(std::max(10, traffic->getSettings().getFollowAircraftDistanceM()));
            static const double PI = std::acos(-1);
            traffic->m_deltaCameraPosition.dxMeters = -distanceMeterM * sin(traffic->m_deltaCameraPosition.headingDeg * PI / 180.0);
            traffic->m_deltaCameraPosition.dzMeters =  distanceMeterM * cos(traffic->m_deltaCameraPosition.headingDeg * PI / 180.0);
            traffic->m_deltaCameraPosition.dyMeters = -distanceMeterM * tan(traffic->m_deltaCameraPosition.pitchDeg   * PI / 180.0);

            traffic->m_deltaCameraPosition.isInitialized = true;
        }

        double lxMeters = 0, lyMeters = 0, lzMeters = 0; // normally init not needed, just to avoid any issues
        static const double kFtToMeters = 0.3048;

        std::string modelName = "unknown";
        if (traffic->m_followPlaneViewCallsign == CTraffic::ownAircraftString())
        {
            lxMeters = traffic->m_ownAircraftPositionX.get();
            lyMeters = traffic->m_ownAircraftPositionY.get();
            lzMeters = traffic->m_ownAircraftPositionZ.get();
            modelName = CTraffic::ownAircraftString();
        }
        else
        {
            if (traffic->m_planesByCallsign.empty())
            {
                INFO_LOG("Follow aircraft, no planes to follow");
                traffic->m_followPlaneViewCallsign.clear();
                return 0;
            }

            if (traffic->m_followPlaneViewCallsign.empty())
            {
                INFO_LOG("Follow aircraft, no callsign to follow");
                traffic->m_followPlaneViewCallsign.clear();
                traffic->m_followAircraftDistanceMultiplier = 1.0;
                return 0;
            }

            const auto planeIt = traffic->m_planesByCallsign.find(traffic->m_followPlaneViewCallsign);
            if (planeIt == traffic->m_planesByCallsign.end())
            {
                INFO_LOG("Follow aircraft, no plane found for callsign " + traffic->m_followPlaneViewCallsign);
                traffic->m_followPlaneViewCallsign.clear();
                traffic->m_followAircraftDistanceMultiplier = 1.0;
                return 0;
            }

            const Plane *plane = planeIt->second;
            if (!plane)
            {
                ERROR_LOG("Follow aircraft, no plane from iterator for callsign " + traffic->m_followPlaneViewCallsign);
                traffic->m_followPlaneViewCallsign.clear();
                traffic->m_followAircraftDistanceMultiplier = 1.0;
                return 0;
            }

            modelName = plane->modelName;
            if (!isValidPosition(plane->position))
            {
                WARNING_LOG("Invalid follow aircraft position for " + plane->callsign);
                WARNING_LOG("Pos: " + pos2String(plane->position));
                return 0;
            }

            // avoid underflow of camera into ground
            if (plane->isOnGround)
            {
                if (traffic->m_deltaCameraPosition.dyMeters < 10) { traffic->m_deltaCameraPosition.dyMeters = 10; }
            }

            XPLMWorldToLocal(plane->position.lat, plane->position.lon, plane->position.elevation * kFtToMeters, &lxMeters, &lyMeters, &lzMeters);
        }

        // Fill out the camera position info.
        cameraPosition->x = static_cast<float>(lxMeters + traffic->m_deltaCameraPosition.dxMeters);
        cameraPosition->y = static_cast<float>(lyMeters + traffic->m_deltaCameraPosition.dyMeters);
        cameraPosition->z = static_cast<float>(lzMeters + traffic->m_deltaCameraPosition.dzMeters);
        cameraPosition->pitch   = CTraffic::normalizeToPlusMinus180Deg(static_cast<float>(traffic->m_deltaCameraPosition.pitchDeg));
        cameraPosition->heading = CTraffic::normalizeToPlusMinus180Deg(static_cast<float>(traffic->m_deltaCameraPosition.headingDeg));
        cameraPosition->roll = 0.0;
        cameraPosition->zoom = 1.0;

        if (!isValidPosition(cameraPosition))
        {
            WARNING_LOG("Invalid camera aircraft position");
            WARNING_LOG("Pos: " + pos2String(cameraPosition));
            traffic->m_followAircraftDistanceMultiplier = 1.0;
            return 0;
        }

        // cppcheck-suppress knownConditionTrueFalse
        if (DEBUG)
        {
            DEBUG_LOG("Camera: " + pos2String(cameraPosition));
            DEBUG_LOG("Follow aircraft " + traffic->m_followPlaneViewCallsign + " " + modelName);
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

        traffic->m_countFrame++;

        // slow updates by modulo
        if (traffic->m_countFrame % 250 == 0)
        {
            // update labels
            const CSettings s = traffic->getSettings();
            if (traffic->isDrawingLabels() != s.isDrawingLabels())
            {
                traffic->setDrawingLabels(s.isDrawingLabels());
            }
        }

        return 1;
    }

    int CTraffic::followAircraftKeySniffer(char character, XPLMKeyFlags flags, char virtualKey, void *refcon)
    {
        (void) character;
        CTraffic *traffic = static_cast<CTraffic *>(refcon);
        if (!traffic || traffic->m_followPlaneViewCallsign.empty()) { return 1; } // totally ignore if nothing is being followed

        // We are only interested in Space key
        if (virtualKey == XPLM_VK_SPACE)
        {
            // if XPlane looses focus it can happen that key down is NOT reset
            // for the camera we use the init flag instead, so it is only run once
            if (flags & xplm_DownFlag) { traffic->m_isSpacePressed = true;  }
            if (flags & xplm_UpFlag)   { traffic->m_isSpacePressed = false; }
        }
        else if (virtualKey == XPLM_VK_DOWN   && (flags & xplm_UpFlag)) { traffic->m_followAircraftDistanceMultiplier /= 1.2; }
        else if (virtualKey == XPLM_VK_UP     && (flags & xplm_UpFlag)) { traffic->m_followAircraftDistanceMultiplier *= 1.2; }
        else if (virtualKey == XPLM_VK_ESCAPE && (flags & xplm_UpFlag)) { traffic->m_followAircraftDistanceMultiplier = 1.0; }

        /* Return 1 to pass the keystroke to plugin windows and X-Plane.
         * Returning 0 would consume the keystroke. */
        return 1;
    }

    bool CTraffic::isPlusMinusOne(float v)
    {
        if (std::isnan(v)) { return false; }
        if (v > 1.00001f || v < -1.00001f) { return false; }
        return true;
    }

    bool CTraffic::isPlusMinus180(float v)
    {
        if (std::isnan(v)) { return false; }
        if (v > 180.00001f || v < -180.00001f) { return false; }
        return true;
    }

    bool CTraffic::isPlusMinus180(double v)
    {
        if (std::isnan(v)) { return false; }
        if (v > 180.00001 || v < -180.00001) { return false; }
        return true;
    }

    bool CTraffic::isZeroTo360(double v)
    {
        if (std::isnan(v)) { return false; }
        if (v < 0 || v >= 360.0) { return false; }
        return true;
    }

    float CTraffic::normalizeToPlusMinus180Deg(float v)
    {
        if (std::isnan(v)) { return 0.0f; }
        return static_cast<float>(normalizeToPlusMinus180Deg(static_cast<double>(v)));
    }

    double CTraffic::normalizeToPlusMinus180Deg(double v)
    {
        if (std::isnan(v)) { return 0.0; }
        const double n = normalizeValue(v, -180.0, 180.0);
        if (n <= -180.0) { return 180.0; }
        if (n >   180.0) { return 180.0; }
        return n;
    }

    float CTraffic::normalizeToZero360Deg(float v)
    {
        if (std::isnan(v)) { return 0.0f; }
        return static_cast<float>(normalizeToZero360Deg(static_cast<double>(v)));

    }

    double CTraffic::normalizeToZero360Deg(double v)
    {
        if (std::isnan(v)) { return 0.0; }
        const double n = normalizeValue(v, 0, 360.0);
        if (n >= 360.0) { return 0.0;}
        if (n < 0.0)    { return 0.0;}
        return n;
    }

    bool CTraffic::isValidPosition(const XPMPPlanePosition_t &position)
    {
        if (!isPlusMinus180(position.lat)) { return false; }
        if (!isPlusMinus180(position.lon)) { return false; }
        if (!isPlusMinus180(position.pitch))   { return false; }
        if (!isPlusMinus180(position.roll))    { return false; }
        if (!isPlusMinus180(position.heading)) { return false; }
        if (position.elevation < -2000.0 || position.elevation > 100000.0) { return false; }

        return true;
    }

    bool CTraffic::isValidPosition(const XPLMCameraPosition_t *camPos)
    {
        if (!isPlusMinus180(camPos->roll))    { return false; }
        if (!isPlusMinus180(camPos->pitch))   { return false; }
        if (!isPlusMinus180(camPos->heading)) { return false; }

        // x, y, z not in -1..1 range
        if (std::isnan(camPos->x)) { return false; }
        if (std::isnan(camPos->y)) { return false; }
        if (std::isnan(camPos->z)) { return false; }

        return true;
    }

    std::string CTraffic::pos2String(const XPMPPlanePosition_t &position)
    {
        return "lat, lon, el: " +
               std::to_string(position.lat) + "/" + std::to_string(position.lon) + "/" + std::to_string(position.elevation) +
               " prh: " +
               std::to_string(position.pitch) + "/" + std::to_string(position.roll) + "/" + std::to_string(position.heading);
    }

    std::string CTraffic::pos2String(const XPLMCameraPosition_t *camPos)
    {
        if (!camPos) { return "camera position invalid"; }
        return "x, y, z: " +
               std::to_string(camPos->x) + "/" + std::to_string(camPos->y) + "/" + std::to_string(camPos->z) +
               " zoom: " +
               std::to_string(camPos->zoom) +
               " prh: " +
               std::to_string(camPos->pitch) + "/" + std::to_string(camPos->roll) + "/" + std::to_string(camPos->heading);
    }
} // ns

//! \endcond
