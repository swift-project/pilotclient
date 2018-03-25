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
#include "datarefs.h"
#include "terrainprobe.h"
#include "XPMPMultiplayer.h"
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
        CTraffic(CDBusConnection *dbusConnection);

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

        //! Set the position of a traffic aircraft
        void setPlanePosition(const std::string &callsign, double latitude, double longitude, double altitude, double pitch, double roll, double heading);

        //! Set the flight control surfaces and lights of a traffic aircraft
        void setPlaneSurfaces(const std::string &callsign, double gear, double flap, double spoiler, double speedBrake, double slat, double wingSweep, double thrust,
            double elevator, double rudder, double aileron, bool landLight, bool beaconLight, bool strobeLight, bool navLight, int lightPattern, bool onGround);

        //! Set the transponder of a traffic aircraft
        void setPlaneTransponder(const std::string &callsign, int code, bool modeC, bool ident);

        //! Request traffic plane data. A signal remoteAircraftData will be emitted for each known plane
        void requestRemoteAircraftData();

        int processDBus() override;

    protected:
         DBusHandlerResult dbusMessageHandler(const CDBusMessage &message) override;

    private:
        bool m_initialized = false;
        bool m_enabled = false;

        void emitSimFrame();
        void emitRemoteAircraftData(const std::string &callsign, double latitude, double longitude, double elevation, double modelVerticalOffset);

        static int preferences(const char *section, const char *name, int def);
        static float preferences(const char *section, const char *name, float def);

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
        std::chrono::system_clock::time_point m_timestampLastSimFrame = std::chrono::system_clock::now();

        int getPlaneData(void *id, int dataType, void *io_data);
        static int getPlaneData(void *id, int dataType, void *io_data, void *self)
        {
            return static_cast<CTraffic *>(self)->getPlaneData(id, dataType, io_data);
        }
    };
}

#endif // guard

