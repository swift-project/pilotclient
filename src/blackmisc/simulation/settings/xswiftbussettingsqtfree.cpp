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

using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::Simulation::XPlane;

using namespace rapidjson;

//! @cond SWIFT_INTERNAL
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDBusServerAddress[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDrawingLabels[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxPlanes[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxDrawDistance[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonFollowAircraftDistanceM[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonTimestamp[];
//! @endcond

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            CXSwiftBusSettingsQtFree::CXSwiftBusSettingsQtFree()
            {}

            CXSwiftBusSettingsQtFree::CXSwiftBusSettingsQtFree(const std::string &json)
            {
                this->parseXSwiftBusString(json);
            }

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
                if (settingsDoc.HasMember(CXSwiftBusSettingsQtFree::JsonDrawingLabels) && settingsDoc[CXSwiftBusSettingsQtFree::JsonDrawingLabels].IsBool())
                {
                    m_drawingLabels = settingsDoc[CXSwiftBusSettingsQtFree::JsonDrawingLabels].GetBool();  c++;
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
                return c == 6;
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
                document.AddMember(JsonDrawingLabels,     m_drawingLabels, a);
                document.AddMember(JsonMaxPlanes,         m_maxPlanes, a);
                document.AddMember(JsonMaxDrawDistance,   m_maxDrawDistanceNM, a);
                document.AddMember(JsonTimestamp,         m_msSinceEpochQtFree, a);
                document.AddMember(JsonFollowAircraftDistanceM, m_followAircraftDistanceM, a);

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
                       ", drawLabels: " + QtFreeUtils::boolToYesNo(m_drawingLabels) +
                       ", max planes: " + std::to_string(m_maxPlanes) +
                       ", max distance NM: " + std::to_string(m_maxDrawDistanceNM) +
                       ", follow dist m: "   + std::to_string(m_followAircraftDistanceM) +
                       ", ts: " + std::to_string(m_msSinceEpochQtFree);
            }

            int CXSwiftBusSettingsQtFree::update(const CXSwiftBusSettingsQtFree &newValues)
            {
                int changed = 0;
                if (m_dBusServerAddress  != newValues.m_dBusServerAddress)  { m_dBusServerAddress  = newValues.m_dBusServerAddress;  changed++; }
                if (m_drawingLabels      != newValues.m_drawingLabels)      { m_drawingLabels      = newValues.m_drawingLabels;      changed++; }
                if (m_maxPlanes          != newValues.m_maxPlanes)          { m_maxPlanes          = newValues.m_maxPlanes;          changed++; }
                if (m_msSinceEpochQtFree != newValues.m_msSinceEpochQtFree) { m_msSinceEpochQtFree = newValues.m_msSinceEpochQtFree; changed++; }
                if (m_followAircraftDistanceM != newValues.m_followAircraftDistanceM)  { m_followAircraftDistanceM = newValues.m_followAircraftDistanceM; changed++; }
                if (!QtFreeUtils::isFuzzyEqual(m_maxDrawDistanceNM, newValues.m_maxDrawDistanceNM)) { m_maxDrawDistanceNM = newValues.m_maxDrawDistanceNM; changed++; }

                if (changed > 0) { this->objectUpdated(); } // post processing
                return changed;
            }

            void CXSwiftBusSettingsQtFree::setCurrentUtcTime()
            {
                using namespace std::chrono;
                const milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                m_msSinceEpochQtFree = static_cast<int64_t>(ms.count());
            }

            void CXSwiftBusSettingsQtFree::objectUpdated()
            {
                // void
            }
        } // ns
    } // ns
} // ns

