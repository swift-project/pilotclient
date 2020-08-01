/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKMISC_SIMULATION_SETTINGS_CXSWIFTBUSSETTINGSQTFREE_H
#define BLACKMISC_SIMULATION_SETTINGS_CXSWIFTBUSSETTINGSQTFREE_H

#include "blackmisc/simulation/xplane/qtfreeutils.h"

#include <string>
#include <chrono>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            /*!
             * XSwiftBus/swift side settings class, JSON capable, shared among all services
             * \details Used on swift and XSwiftBus side, MUST BE Qt free
             */
            class CXSwiftBusSettingsQtFree
            {
            protected:
                //! Constructor.
                CXSwiftBusSettingsQtFree() {}

                //! Destructor.
                virtual ~CXSwiftBusSettingsQtFree() {}

            public:
                //! DBus server
                const std::string &getDBusServerAddress() const { return m_dBusServerAddress; }

                //! Set DBus server
                void setDBusServerAddress(const std::string &dBusServer) { m_dBusServerAddress = dBusServer; }

                //! Set whether the plugin draws type and callsign labels above aircraft
                void setDrawingLabels(bool drawing) { m_drawingLabels = drawing; }

                //! Get whether the plugin draws type and callsign labels above aircraft
                bool isDrawingLabels() const { return m_drawingLabels; }

                //! Bundle taxi and landing lights
                void setBundlingTaxiAndLandingLights(bool bundle) { m_bundleTaxiLandingLights = bundle; }

                //! Bundle taxi and landing lights
                bool isBundlingTaxiAndLandingLights() const { return m_bundleTaxiLandingLights; }

                //! The the night texture mode
                void setNightTextureMode(const std::string &mode) { m_nightTextureMode = XPlane::QtFreeUtils::toLower(mode); }

                //! The the night texture mode
                const std::string &getNightTextureMode() const { return m_nightTextureMode; }

                //! Left, top, right, bottom, lines, duration
                void setMessageBoxValues(const std::string &positions) { m_msgBox = positions; }

                //! Left, top, right, bottom, lines, duration
                void setMessageBoxValues(int leftPx, int topPx, int rightPx, int bottomPx, int lines, int durationMs)
                {
                    if (topPx >= 0) { bottomPx = -1; }
                    if (lines < 3) { lines = 3; }
                    this->setMessageBoxValues(std::to_string(leftPx) + ";" + std::to_string(topPx) + ";" + std::to_string(rightPx) + ";" + std::to_string(bottomPx) + ";" + std::to_string(lines) + ";" + std::to_string(durationMs));
                }

                //! Left, top, right, bottom, lines, duration
                const std::string &getMessageBoxValues() const { return m_msgBox; }

                //! Left, top, right, bottom, lines, duration
                std::vector<int> getMessageBoxValuesVector() const
                {
                    constexpr int partCount = 6;
                    std::vector<int> partsInt = {20, 20, 20, -1, 5, 5000};
                    const std::vector<std::string> parts = XPlane::QtFreeUtils::split(m_msgBox, partCount, ";");

                    size_t c = 0;
                    for (const std::string &p : parts)
                    {
                        const int i = std::stoi(p);
                        partsInt[c++] = i;
                    }
                    return partsInt;
                }

                //! Set the maximum number of aircraft.
                bool setMaxPlanes(int planes)
                {
                    if (planes == m_maxPlanes) { return false; }
                    m_maxPlanes = planes;
                    return true;
                }

                //! Get the maximum number of aircraft.
                int getMaxPlanes() const { return m_maxPlanes; }

                //! Set follow aircraft distance
                bool setFollowAircraftDistanceM(int meters)
                {
                    if (meters == m_followAircraftDistanceM) { return false; }
                    m_followAircraftDistanceM = meters;
                    return true;
                }

                //! Get follow aircraft distance
                int getFollowAircraftDistanceM() const { return m_followAircraftDistanceM; }

                //! Set the maximum distance at which to draw aircraft (nautical miles).
                double getMaxDrawDistanceNM() const { return m_maxDrawDistanceNM; }

                //! Set the maximum distance at which to draw aircraft (nautical miles).
                bool setMaxDrawDistanceNM(double nauticalMiles)
                {
                    if (BlackMisc::Simulation::XPlane::QtFreeUtils::isFuzzyEqual(nauticalMiles, m_maxDrawDistanceNM)) { return false; }
                    m_maxDrawDistanceNM = nauticalMiles;
                    return true;
                }

                //! Debug messages?
                bool isLogRenderPhases() const { return m_logRenderPhases; }

                //! Debug messages?
                void setLogRenderPhases(bool log) { m_logRenderPhases = log; }

                //! TCAS functionality?
                bool isTcasEnabled() const { return m_tcasEnabled; }

                //! TCAS functionality?
                void setTcasEnabled(bool tcas) { m_tcasEnabled = tcas; }

                //! Terrain probe to query ground elevation enabled?
                bool isTerrainProbeEnabled() const { return m_terrainProbeEnabled; }

                //! Terrain probe to query ground elevation enabled?
                void setTerrainProbeEnabled(bool enabled) { m_terrainProbeEnabled = enabled; }

                //! Load and parse config file
                bool parseXSwiftBusString(const std::string &json);

                //! As JSON string
                std::string toXSwiftBusJsonString() const;

                //! Convert to string
                std::string convertToString() const;

                //! Update only changed values
                int update(const CXSwiftBusSettingsQtFree &newValues);

                //! Sets timestamp to now
                virtual void setCurrentUtcTime()
                {
                    using namespace std::chrono;
                    const milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                    m_msSinceEpochQtFree = static_cast<int64_t>(ms.count());
                }

            protected:
                //! The JSON members
                //! @{
                static constexpr char JsonDBusServerAddress[] = "dbusserveradress";
                static constexpr char JsonDrawingLabels[]     = "drawinglabels";
                static constexpr char JsonLogRenderPhases[]   = "renderPhases";
                static constexpr char JsonTcas[]              = "tcas";
                static constexpr char JsonTerrainProbe[]      = "terrainProbe";
                static constexpr char JsonMaxPlanes[]         = "maxplanes";
                static constexpr char JsonMaxDrawDistance[]   = "maxDrawDistance";
                static constexpr char JsonNightTextureMode[]  = "nighttexture";
                static constexpr char JsonTimestamp[]         = "timestamp";
                static constexpr char JsonMessageBox[]        = "msgbox";
                static constexpr char JsonBundleTaxiLandingLights[] = "bundleLights";
                static constexpr char JsonFollowAircraftDistanceM[] = "followAircraftDistance";
                //! @}

                //! Object has been updated
                virtual void objectUpdated() = 0;

                std::string m_dBusServerAddress { "tcp:host=127.0.0.1,port=45001" }; //!< DBus server (also in class CXSwiftBusConfigWriter)
                std::string m_nightTextureMode  { "auto" }; //!< night texture mode
                std::string m_msgBox            { "20;20;20;-1;5;5000" }; //!< left, top, right, bottom, lines, duration
                int    m_maxPlanes               = 100;     //!< max. planes in XPlane
                int    m_followAircraftDistanceM = 200;     //!< follow aircraft in distance
                bool   m_drawingLabels           = true;    //!< labels in XPlane
                bool   m_bundleTaxiLandingLights = true;    //!< bundle taxi and landing lights
                bool   m_logRenderPhases         = false;   //!< render phases debug messages
                bool   m_tcasEnabled             = true;    //!< TCAS functionality
                bool   m_terrainProbeEnabled     = true;    //!< terrain probe to establish ground elevation
                double m_maxDrawDistanceNM       = 50.0;    //!< distance in XPlane
                int64_t m_msSinceEpochQtFree     = 0;       //!< timestamp
            };
        } // ns
    } // ns
} // ns

#endif
