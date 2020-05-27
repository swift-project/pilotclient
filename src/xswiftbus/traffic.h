/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKSIM_XSWIFTBUS_TRAFFIC_H
#define BLACKSIM_XSWIFTBUS_TRAFFIC_H

//! \file

#include "dbusobject.h"
#include "settings.h"
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
        CTraffic(CSettingsProvider *settingsProvider);

        //! Destructor
        virtual ~CTraffic() override;

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

        //! Returns whether multiplayer planes have been acquired. If not, owner will be set to the plugin that acquired it.
        bool acquireMultiplayerPlanes(std::string *owner = nullptr);

        //! Reverse the actions of initialize().
        void cleanup();

        //! Load a collection of planes from the given directory and return error message if unsuccessful
        std::string loadPlanesPackage(const std::string &path);

        //! Set the ICAO code to use for aircraft without a model match
        void setDefaultIcao(const std::string &defaultIcao);

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
        void setPlanesPositions(const std::vector<std::string> &callsigns,
                                std::vector<double> latitudesDeg, std::vector<double> longitudesDeg, std::vector<double> altitudesFt,
                                std::vector<double> pitchesDeg, std::vector<double> rollsDeg, std::vector<double> headingsDeg, const std::vector<bool> &onGrounds);

        //! Set the flight control surfaces and lights of multiple traffic aircrafts
        void setPlanesSurfaces(const std::vector<std::string> &callsigns, const std::vector<double> &gears, const std::vector<double> &flaps, const std::vector<double> &spoilers,
                               const std::vector<double> &speedBrakes, const std::vector<double> &slats, const std::vector<double> &wingSweeps, const std::vector<double> &thrusts,
                               const std::vector<double> &elevators, const std::vector<double> &rudders, const std::vector<double> &ailerons,
                               const std::vector<bool> &landLights, const std::vector<bool> &taxiLights,
                               const std::vector<bool> &beaconLights, const std::vector<bool> &strobeLights, const std::vector<bool> &navLights, const std::vector<int> &lightPatterns);

        //! Set the transponder of multiple traffic aircraft
        void setPlanesTransponders(const std::vector<std::string> &callsigns, const std::vector<int> &codes, const std::vector<bool> &modeCs, const std::vector<bool> &idents);

        //! Get remote aircrafts data (lat, lon, elevation and CG)
        void getRemoteAircraftData(std::vector<std::string> &callsigns, std::vector<double> &latitudesDeg, std::vector<double> &longitudesDeg,
                                   std::vector<double> &elevationsM, std::vector<bool> &waterFlags, std::vector<double> &verticalOffsets) const;

        //! Get the ground elevation at an arbitrary position
        std::array<double, 3> getElevationAtPosition(const std::string &callsign, double latitudeDeg, double longitudeDeg, double altitudeMeters, bool &o_isWater) const;

        //! Sets the aircraft with callsign to be followed in plane view
        void setFollowedAircraft(const std::string &callsign);

        //! Perform generic processing
        int process();

        //! Returns the own aircraft string to be used as callsign for setFollowedAircraft()
        static const std::string &ownAircraftString() { static const std::string o = "ownAircraft"; return o; }

    protected:
        //! Handler
        virtual void dbusDisconnectedHandler() override;

        //! Handler
        DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) override;

    private:
        //! Camera
        struct DeltaCameraPosition
        {
            double dxMeters    = 0.0;
            double dyMeters    = 0.0;
            double dzMeters    = 0.0;
            double headingDeg  = 0.0;
            double pitchDeg    = 0.0;
            bool isInitialized = false;
        };

        bool m_initialized        = false;
        bool m_enabledMultiplayer = false;
        CTerrainProbe m_terrainProbe;

        void emitSimFrame();
        void emitPlaneAdded(const std::string &callsign);
        void emitPlaneAddingFailed(const std::string &callsign);
        void switchToFollowPlaneView(const std::string &callsign);
        void followNextPlane();
        void followPreviousPlane();
        bool containsCallsign(const std::string &callsign) const;

        static CTraffic *s_instance;
        static int preferences(const char *section, const char *name, int def);
        static float preferences(const char *section, const char *name, float def);

        static int orbitOwnAircraftFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon);
        static int orbitPlaneFunc(XPLMCameraPosition_t *cameraPosition, int isLosingControl, void *refcon);
        static int drawCallback(XPLMDrawingPhase phase, int isBefore, void *refcon);
        static int followAircraftKeySniffer(char character, XPLMKeyFlags flags, char virtualKey, void *refcon);

        //! Remote aircraft
        struct Plane
        {
            void *id = nullptr;
            std::string callsign;
            std::string aircraftIcao;
            std::string airlineIcao;
            std::string livery;
            std::string modelName;
            std::string nightTextureMode;
            bool hasSurfaces = false;
            bool hasXpdr     = false;
            bool isOnGround  = false;
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

        //! Check functions @{
        static bool isPlusMinusOne(float v);
        static bool isPlusMinus180(float v);
        static bool isPlusMinus180(double v);
        static bool isZeroTo360(double v);
        //! @}

        //! Normalize to (-180, 180] or [0, 360) degrees @{
        static float  normalizeToPlusMinus180Deg(float v);
        static double normalizeToPlusMinus180Deg(double v);
        static float  normalizeToZero360Deg(float v);
        static double normalizeToZero360Deg(double v);
        //! @}

        //! Check the position if values are valid @{
        static bool isValidPosition(const XPMPPlanePosition_t &position);
        static bool isValidPosition(const XPLMCameraPosition_t *camPos);
        //! @}

        //! Pos as string @{
        static std::string pos2String(const XPMPPlanePosition_t &position);
        static std::string pos2String(const XPLMCameraPosition_t *camPos);
        //! @}

        std::unordered_map<std::string, Plane *> m_planesByCallsign;
        std::unordered_map<void *, Plane *> m_planesById;
        std::vector<std::string> m_followPlaneViewSequence;
        // std::chrono::system_clock::time_point m_timestampLastSimFrame = std::chrono::system_clock::now();

        CMenu m_followPlaneViewSubMenu;
        std::unordered_map<std::string, CMenuItem> m_followPlaneViewMenuItems;
        std::string m_followPlaneViewCallsign;
        CCommand m_followPlaneViewNextCommand;
        CCommand m_followPlaneViewPreviousCommand;

        DataRef<xplane::data::sim::graphics::view::world_render_type> m_worldRenderType;
        DataRef<xplane::data::sim::flightmodel::position::local_x> m_ownAircraftPositionX;
        DataRef<xplane::data::sim::flightmodel::position::local_y> m_ownAircraftPositionY;
        DataRef<xplane::data::sim::flightmodel::position::local_z> m_ownAircraftPositionZ;

        bool m_isSpacePressed = false;
        int m_lastMouseX = -1;
        int m_lastMouseY = -1;
        double m_followAircraftDistanceMultiplier = 1.0;
        DeltaCameraPosition m_deltaCameraPosition;

        bool m_emitSimFrame = true;
        int m_countFrame    = 0; //!< allows to do something every n-th frame

        int getPlaneData(void *id, int dataType, void *io_data);
        static int getPlaneData(void *id, int dataType, void *io_data, void *self)
        {
            return static_cast<CTraffic *>(self)->getPlaneData(id, dataType, io_data);
        }

        static void planeLoaded(void *id, bool succeeded, void *self)
        {
            auto *traffic = static_cast<CTraffic *>(self);
            auto  planeIt = traffic->m_planesById.find(id);
            if (planeIt == traffic->m_planesById.end()) { return; }

            if (succeeded) { traffic->emitPlaneAdded(planeIt->second->callsign); }
            else { traffic->emitPlaneAddingFailed(planeIt->second->callsign); }
        }
    };
} // ns

#endif // guard

