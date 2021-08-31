/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "xswiftbussettingsqtfree.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

#include <string>
#include <chrono>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

using namespace rapidjson;

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
                const char *jsonCStr = json.c_str();

                Document settingsDoc;
                // "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
                if (settingsDoc.Parse(jsonCStr).HasParseError()) { return false; }

                int c = 0;
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonDBusServerAddress) && settingsDoc[CXSwiftBusSettingsQtFree::JsonDBusServerAddress].IsString())
                {
                    m_dBusServerAddress = settingsDoc[CXSwiftBusSettingsQtFree::JsonDBusServerAddress].GetString();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonMessageBox) && settingsDoc[CXSwiftBusSettingsQtFree::JsonMessageBox].IsString())
                {
                    m_msgBox = settingsDoc[CXSwiftBusSettingsQtFree::JsonMessageBox].GetString();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonNightTextureMode) && settingsDoc[CXSwiftBusSettingsQtFree::JsonNightTextureMode].IsString())
                {
                    m_nightTextureMode = settingsDoc[CXSwiftBusSettingsQtFree::JsonNightTextureMode].GetString();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonDrawingLabels) && settingsDoc[CXSwiftBusSettingsQtFree::JsonDrawingLabels].IsBool())
                {
                    m_drawingLabels = settingsDoc[CXSwiftBusSettingsQtFree::JsonDrawingLabels].GetBool();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonLabelColor) && settingsDoc[CXSwiftBusSettingsQtFree::JsonLabelColor].IsInt())
                {
                    m_labelColor = settingsDoc[CXSwiftBusSettingsQtFree::JsonLabelColor].GetInt();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights) && settingsDoc[CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights].IsBool())
                {
                    m_bundleTaxiLandingLights = settingsDoc[CXSwiftBusSettingsQtFree::JsonBundleTaxiLandingLights].GetBool();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonTcas) && settingsDoc[CXSwiftBusSettingsQtFree::JsonTcas].IsBool())
                {
                    m_tcasEnabled = settingsDoc[CXSwiftBusSettingsQtFree::JsonTcas].GetBool();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonTerrainProbe) && settingsDoc[CXSwiftBusSettingsQtFree::JsonTerrainProbe].IsBool())
                {
                    m_terrainProbeEnabled = settingsDoc[CXSwiftBusSettingsQtFree::JsonTerrainProbe].GetBool();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonLogRenderPhases) && settingsDoc[CXSwiftBusSettingsQtFree::JsonLogRenderPhases].IsBool())
                {
                    m_logRenderPhases = settingsDoc[CXSwiftBusSettingsQtFree::JsonLogRenderPhases].GetBool();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonMaxPlanes) && settingsDoc[CXSwiftBusSettingsQtFree::JsonMaxPlanes].IsInt())
                {
                    m_maxPlanes = settingsDoc[CXSwiftBusSettingsQtFree::JsonMaxPlanes].GetInt();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonMaxDrawDistance) && settingsDoc[CXSwiftBusSettingsQtFree::JsonMaxDrawDistance].IsDouble())
                {
                    m_maxDrawDistanceNM = settingsDoc[CXSwiftBusSettingsQtFree::JsonMaxDrawDistance].GetDouble();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM) && settingsDoc[CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM].IsInt())
                {
                    m_followAircraftDistanceM = settingsDoc[CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM].GetInt();  c++;
                }
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonTimestamp) && settingsDoc[CXSwiftBusSettingsQtFree::JsonTimestamp].IsInt64())
                {
                    m_msSinceEpochQtFree = settingsDoc[CXSwiftBusSettingsQtFree::JsonTimestamp].GetInt64();  c++;
                }
                this->objectUpdated(); // post processing
                return c == 13;
            }

            std::string CXSwiftBusSettingsQtFree::toXSwiftBusJsonString() const
            {
                Document document;
                document.SetObject();

                // 1st version, we could also just concat the JSON string
                // Explicit key/value
                Document::AllocatorType &a = document.GetAllocator();
                // Value k1(JsonDBusServerAddress, a);
                // Value v1(m_dBusServerAddress, a);
                document.AddMember(JsonDBusServerAddress, StringRef(m_dBusServerAddress.c_str()), a);
                document.AddMember(JsonNightTextureMode,  StringRef(m_nightTextureMode.c_str()), a);
                document.AddMember(JsonMessageBox,        StringRef(m_msgBox.c_str()), a);
                document.AddMember(JsonMaxPlanes,         m_maxPlanes, a);
                document.AddMember(JsonMaxDrawDistance,   m_maxDrawDistanceNM,  a);
                document.AddMember(JsonTimestamp,         m_msSinceEpochQtFree, a);
                document.AddMember(JsonDrawingLabels,     m_drawingLabels, a);
                document.AddMember(JsonLabelColor,        m_labelColor,    a);
                document.AddMember(JsonBundleTaxiLandingLights, m_bundleTaxiLandingLights, a);
                document.AddMember(JsonFollowAircraftDistanceM, m_followAircraftDistanceM, a);
                document.AddMember(JsonLogRenderPhases,   m_logRenderPhases,     a);
                document.AddMember(JsonTcas,              m_tcasEnabled,         a);
                document.AddMember(JsonTerrainProbe,      m_terrainProbeEnabled, a);

                // document[CXSwiftBusSettingsQtFree::JsonDBusServerAddress].SetString(StringRef(m_dBusServerAddress.c_str(), m_dBusServerAddress.size()));
                // document[CXSwiftBusSettingsQtFree::JsonDrawingLabels].SetBool(m_drawingLabels);
                // document[CXSwiftBusSettingsQtFree::JsonMaxPlanes].SetInt(m_maxPlanes);
                // document[CXSwiftBusSettingsQtFree::JsonMaxDrawDistance].SetDouble(m_maxDrawDistanceNM);

                StringBuffer sb;
                PrettyWriter<StringBuffer> writer(sb);
                document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
                const std::string json = sb.GetString();
                return json;
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
