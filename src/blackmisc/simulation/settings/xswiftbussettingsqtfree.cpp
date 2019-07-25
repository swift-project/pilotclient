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

using namespace rapidjson;
using namespace BlackMisc::Simulation::Settings;

//! @cond SWIFT_INTERNAL
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDBusServerAddress[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonDrawingLabels[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxPlanes[];
constexpr char BlackMisc::Simulation::Settings::CXSwiftBusSettingsQtFree::JsonMaxDrawDistance[];
//! @endcond

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Settings
        {
            CXSwiftBusSettingsQtFree::CXSwiftBusSettingsQtFree()
            {}

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
                return c == 4;
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
        } // ns
    } // ns
} // ns

