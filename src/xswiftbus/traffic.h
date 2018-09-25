/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_TRAFFIC_H
#define BLACKSIM_XSWIFTBUS_TRAFFIC_H

//! \file

#include "dbusobject.h"
#include "command.h"
#include "datarefs.h"
#include "terrainprobe.h"
#include "menus.h"
#include "XPMPMultiplayer.h"
#include "XPLMCamera.h"
#include <XPLM/XPLMDisplay.h>
#include <functional>
#include <utility>

//! \cond PRIVATE
#define XSWIFTBUS_TRAFFIC_INTERFACENAME "org.swift_project.xswiftbus.traffic"
#define XSWIFTBUS_TRAFFIC_OBJECTPATH "/xswiftbus/traffic"
//! \endcond

namespace XSwiftBus
{
    /*!
     * XSwiftBus service object for traffic aircraft which is accessible through DBus
     */
    class CTraffic : public CDBusObject
    {
    public:
        //! Constructor
        CTraffic();

        //! Destructor
        ~CTraffic() override;

        //! DBus interface name
        static const std::string &InterfaceName()
        {
            static std::string s(XSWIFTBUS_TRAFFIC_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const std::string &ObjectPath()
        {
            static std::string s(XSWIFTBUS_TRAFFIC_OBJECTPATH);
            return s;
        }

        //! Set plane view submenu
        void setPlaneViewMenu(const CMenu &planeViewSubMenu);

        //! Called by XPluginStart
        static void initLegacyData();

        //! Initialize the multiplayer planes rendering and return true if successful
        bool initialize();

        //! Reverse the actions of initialize().
        void cleanup();

        //! Load a collection of planes from the given directory and return true if successful
        bool loadPlanesPackage(const std::string &path);

        //! Set the ICAO code to use for aircraft without a model match
        void setDefaultIcao(const std::string &defaultIcao);

        //! Set whether the plugin draws type and callsign labels above aircraft
        void setDrawingLabels(bool drawing);

        //! Get whether the plugin draws type and callsign labels above aircraft
        bool isDrawingLabels() const;

        //! Set the maximum number of aircraft.
        void setMaxPlanes(int planes);

        //! Set the maximum distance at which to draw aircraft (nautical miles).
        void setMaxDrawDistance(double nauticalMiles);

        //! Introduce a new traffic aircraft
        void addPlane(const std::string &callsign, const std::string &modelName, const std::string &aircraftIcao, const std::string &airlineIcao, const std::string &livery);

        //! Remove a traffic aircraft
        void removePlane(const std::string &callsign);

        //! Remove all traffic aircraft
        void removeAllPlanes();

        //! Set the position of multiple traffic aircrafts
        void setPlanesPositions(const std::vector<std::string> &callsigns, std::vector<double> latitudes, std::vector<double> longitudes, std::vector<double> altitudes,
                                std::vector<double> pitches, std::vector<double> rolls, std::vector<double> headings, const std::vector<bool> &onGrounds);

        //! Set the flight control surfaces and lights of multiple traffic aircrafts
        void setPlanesSurfaces(const std::vector<std::string> &callsigns, const std::vector<double> &gears, const std::vector<double> &flaps, const std::vector<double> &spoilers,
                               const std::vector<double> &speedBrakes, const std::vector<double> &slats, const std::vector<double> &wingSweeps, const std::vector<double> &thrusts,
                               const std::vector<double> &elevators, const std::vector<double> &rudders, const std::vector<double> &ailerons, const std::vector<bool> &landLights,
                               const std::vector<bool> &beaconLights, const std::vector<bool> &strobeLights, const std::vector<bool> &navLights, const std::vector<int> &lightPatterns);

        //! Set the transponder of multiple traffic aircraft
        void setPlanesTransponders(const std::vector<std::string> &callsigns, const std::vector<int> &codes, const std::vector<bool> &modeCs, const std::vector<bool> &idents);

        //! Get remote aircrafts data (lat, lon, elevation and CG)
        void getRemoteAircraftData(std::vector<std::string> &callsigns, std::vector<double> &latitudesDeg, std::vector<double> &longitudesDeg,
                                   std::vector<double> &elevationsM, std::vector<double> &verticalOffsets) const;

        //! Get the ground elevation at an arbitrary position
        double getElevationAtPosition(const std::string &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters) const;

        //! Sets the aircraft with callsign to be followed in plane view
        void setFollowedAircraft(const std::string &callsign);

        //! Perform generic processing
        int process();

    protected:
        DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) override;

    private:
        bool m_initialized = false;
        bool m_enabled = false;
        CTerrainProbe m_terrainProbe;

        void emitSimFrame();
        void emitPlaneAdded(const std::string &callsign);
        void emitPlaneAddingFailed(const std::string &callsign);
        void enableFollowPlaneView(const std::string &callsign);
        void followNextPlane();
        void followPreviousPlane();

        static int preferences(const char *section, const char *name, int def);
        static float preferences(const char *section, const char *name, float def);
        static int orbitPlaneFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon);
        static int drawCallback(XPLMDrawingPhase phase, int isBefore, void *refcon);

        struct Plane
        {
            void *id = nullptr;
            std::string callsign;
            std::string aircraftIcao;
            std::string airlineIcao;
            std::string livery;
            std::string modelName;
            bool hasSurfaces = false;
            bool hasXpdr = false;
            char label[32] {};
            CTerrainProbe terrainProbe;
            XPMPPlaneSurfaces_t surfaces;
            float targetGearPosition = 0;
            std::chrono::system_clock::time_point prevSurfacesLerpTime;
            XPMPPlaneRadar_t xpdr;
            XPMPPlanePosition_t position;
            Plane(void *id_, const std::string &callsign_, const std::string &aircraftIcao_, const std::string &airlineIcao_,
                  const std::string &livery_, const std::string &modelName_);
        };

        std::unordered_map<std::string, Plane *> m_planesByCallsign;
        std::unordered_map<void *, Plane *> m_planesById;
        std::vector<std::string> m_followPlaneViewSequence;
        std::chrono::system_clock::time_point m_timestampLastSimFrame = std::chrono::system_clock::now();

        CMenu m_followPlaneViewSubMenu;
        std::unordered_map<std::string, CMenuItem> m_followPlaneViewMenuItems;
        std::string m_followPlaneViewCallsign;
        CCommand m_followPlaneViewNextCommand;
        CCommand m_followPlaneViewPreviousCommand;

        DataRef<xplane::data::sim::graphics::view::world_render_type> m_worldRenderType;
        bool m_emitSimFrame = true;

        int getPlaneData(void *id, int dataType, void *io_data);
        static int getPlaneData(void *id, int dataType, void *io_data, void *self)
        {
            return static_cast<CTraffic *>(self)->getPlaneData(id, dataType, io_data);
        }

        static void planeLoaded(void *id, bool succeeded, void *self)
        {
            auto *traffic = static_cast<CTraffic *>(self);
            auto planeIt = traffic->m_planesById.find(id);
            if (planeIt == traffic->m_planesById.end()) { return; }

            if (succeeded) { traffic->emitPlaneAdded(planeIt->second->callsign); }
            else { traffic->emitPlaneAddingFailed(planeIt->second->callsign); }
        }
    };
} // ns

#endif // guard

