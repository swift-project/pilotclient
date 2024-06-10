// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "xswiftbussettingsqtfree.h"

#include <string>
#include <chrono>

#include "nlohmann/json.hpp"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

//! @cond SWIFT_INTERNAL
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDBusServerAddress[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDrawingLabels[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxPlanes[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxDrawDistance[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonNightTextureMode[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMessageBox[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonTimestamp[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonTcas[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonTerrainProbe[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonLogRenderPhases[];
//! @endcond

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            bool CXSwiftBusSettingsQtFree::parseXSwiftBusString(const std::string &json)
            {
                if (json.empty()) { return false; }

                const nlohmann::basic_json parsed = nlohmann::json::parse(json, nullptr, false);

                if (parsed.is_discarded()) { return false; }

                int c = 0;
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonDBusServerAddress) && parsed[CXSwiftBusSettingsQtFree::JsonDBusServerAddress].is_string())
                {
                    m_dBusServerAddress = parsed[CXSwiftBusSettingsQtFree::JsonDBusServerAddress].get<std::string>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonMessageBox) && parsed[CXSwiftBusSettingsQtFree::JsonMessageBox].is_string())
                {
                    m_msgBox = parsed[CXSwiftBusSettingsQtFree::JsonMessageBox].get<std::string>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonNightTextureMode) && parsed[CXSwiftBusSettingsQtFree::JsonNightTextureMode].is_string())
                {
                    m_nightTextureMode = parsed[CXSwiftBusSettingsQtFree::JsonNightTextureMode].get<std::string>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonDrawingLabels) && parsed[CXSwiftBusSettingsQtFree::JsonDrawingLabels].is_boolean())
                {
                    m_drawingLabels = parsed[CXSwiftBusSettingsQtFree::JsonDrawingLabels].get<bool>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonLabelColor) && parsed[CXSwiftBusSettingsQtFree::JsonLabelColor].is_number_integer())
                {
                    m_labelColor = parsed[CXSwiftBusSettingsQtFree::JsonLabelColor].get<int>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights) && parsed[CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights].is_boolean())
                {
                    m_bundleTaxiLandingLights = parsed[CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights].get<bool>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonTcas) && parsed[CXSwiftBusSettingsQtFree::JsonTcas].is_boolean())
                {
                    m_tcasEnabled = parsed[CXSwiftBusSettingsQtFree::JsonTcas].get<bool>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonTerrainProbe) && parsed[CXSwiftBusSettingsQtFree::JsonTerrainProbe].is_boolean())
                {
                    m_terrainProbeEnabled = parsed[CXSwiftBusSettingsQtFree::JsonTerrainProbe].get<bool>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonLogRenderPhases) && parsed[CXSwiftBusSettingsQtFree::JsonLogRenderPhases].is_boolean())
                {
                    m_logRenderPhases = parsed[CXSwiftBusSettingsQtFree::JsonLogRenderPhases].get<bool>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonMaxPlanes) && parsed[CXSwiftBusSettingsQtFree::JsonMaxPlanes].is_number_integer())
                {
                    m_maxPlanes = parsed[CXSwiftBusSettingsQtFree::JsonMaxPlanes].get<int>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonMaxDrawDistance) && parsed[CXSwiftBusSettingsQtFree::JsonMaxDrawDistance].is_number_float())
                {
                    m_maxDrawDistanceNM = parsed[CXSwiftBusSettingsQtFree::JsonMaxDrawDistance].get<double>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM) && parsed[CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM].is_number_integer())
                {
                    m_followAircraftDistanceM = parsed[CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM].get<int>();  c++;
                }
                if (parsed.contains(CXSwiftBusSettingsQtFree::JsonTimestamp) && parsed[CXSwiftBusSettingsQtFree::JsonTimestamp].is_number_integer())
                {
                    m_msSinceEpochQtFree = parsed[CXSwiftBusSettingsQtFree::JsonTimestamp].get<int64_t>();  c++;
                }
                this->objectUpdated(); // post processing
                return c == 13;
            }

            std::string CXSwiftBusSettingsQtFree::toXSwiftBusJsonString() const
            {
                nlohmann::json json;


                json[JsonDBusServerAddress] = m_dBusServerAddress;
                json[JsonNightTextureMode] = m_nightTextureMode;
                json[JsonMessageBox] = m_msgBox;
                json[JsonMaxPlanes] = m_maxPlanes;
                json[JsonMaxDrawDistance] = m_maxDrawDistanceNM;
                json[JsonTimestamp] = m_msSinceEpochQtFree;
                json[JsonDrawingLabels] = m_drawingLabels;
                json[JsonLabelColor] = m_labelColor;
                json[JsonBundleTaxiLandingLights] = m_bundleTaxiLandingLights;
                json[JsonFollowAircraftDistanceM] = m_followAircraftDistanceM;
                json[JsonLogRenderPhases] = m_logRenderPhases;
                json[JsonTcas] = m_tcasEnabled;
                json[JsonTerrainProbe] = m_terrainProbeEnabled;

                return json.dump(2);
            }

            std::string CXSwiftBusSettingsQtFree::convertToString() const
            {
                return "DBusServer: " + m_dBusServerAddress +
                       ", drawLabels: "      + QtFreeUtils::boolToYesNo(m_drawingLabels) +
                       ", labelColor: "      + std::to_string((m_labelColor >> 16) & 0xff) +
                                   ";"       + std::to_string((m_labelColor >> 8) & 0xff) +
                                   ";"       + std::to_string(m_labelColor & 0xff) +
                       ", bundle lights: "   + QtFreeUtils::boolToYesNo(m_bundleTaxiLandingLights) +
                       ", phases: "          + QtFreeUtils::boolToYesNo(m_logRenderPhases) +
                       ", TCAS: "            + QtFreeUtils::boolToYesNo(m_tcasEnabled) +
                       ", terr.probe: "      + QtFreeUtils::boolToYesNo(m_terrainProbeEnabled) +
                       ", night t.: "        + m_nightTextureMode +
                       ", max planes: "      + std::to_string(m_maxPlanes) +
                       ", max distance NM: " + std::to_string(m_maxDrawDistanceNM) +
                       ", follow dist m: "   + std::to_string(m_followAircraftDistanceM) +
                       ", msg.box: "         + m_msgBox +
                       ", ts: "              + std::to_string(m_msSinceEpochQtFree);
            }

            int CXSwiftBusSettingsQtFree::update(const CXSwiftBusSettingsQtFree &newValues)
            {
                int changed = 0;
                if (m_dBusServerAddress  != newValues.m_dBusServerAddress)  { m_dBusServerAddress  = newValues.m_dBusServerAddress;  changed++; }
                if (m_msgBox             != newValues.m_msgBox)             { m_msgBox             = newValues.m_msgBox;             changed++; }
                if (m_drawingLabels      != newValues.m_drawingLabels)      { m_drawingLabels      = newValues.m_drawingLabels;      changed++; }
                if (m_labelColor         != newValues.m_labelColor)         { m_labelColor         = newValues.m_labelColor;         changed++; }
                if (m_nightTextureMode   != newValues.m_nightTextureMode)   { m_nightTextureMode   = newValues.m_nightTextureMode;   changed++; }
                if (m_logRenderPhases    != newValues.m_logRenderPhases)    { m_logRenderPhases    = newValues.m_logRenderPhases;          changed++; }
                if (m_tcasEnabled        != newValues.m_tcasEnabled)        { m_tcasEnabled        = newValues.m_tcasEnabled;        changed++; }
                if (m_terrainProbeEnabled != newValues.m_terrainProbeEnabled) { m_terrainProbeEnabled = newValues.m_terrainProbeEnabled;   changed++; }
                if (m_maxPlanes          != newValues.m_maxPlanes)          { m_maxPlanes          = newValues.m_maxPlanes;          changed++; }
                if (m_msSinceEpochQtFree != newValues.m_msSinceEpochQtFree) { m_msSinceEpochQtFree = newValues.m_msSinceEpochQtFree; changed++; }
                if (m_bundleTaxiLandingLights != newValues.m_bundleTaxiLandingLights) { m_bundleTaxiLandingLights = newValues.m_bundleTaxiLandingLights;   changed++; }
                if (m_followAircraftDistanceM != newValues.m_followAircraftDistanceM) { m_followAircraftDistanceM = newValues.m_followAircraftDistanceM;   changed++; }
                if (!QtFreeUtils::isFuzzyEqual(m_maxDrawDistanceNM, newValues.m_maxDrawDistanceNM)) { m_maxDrawDistanceNM = newValues.m_maxDrawDistanceNM; changed++; }

                if (changed > 0) { this->objectUpdated(); } // post processing
                return changed;
            }

        } // ns
    } // ns
} // ns

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
