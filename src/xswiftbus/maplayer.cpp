// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "maplayer.h"

#include "traffic.h"

#include <XPLM/XPLMUtilities.h>

#include <cmath>
#include <utility>

namespace XSwiftBus
{
    namespace
    {
        constexpr int c_mapIconWidth = 3;   // 3x3 sprite sheet
        constexpr float c_aircraftSizeM = 30.0f;  // assumed aircraft fuselage length
        constexpr int c_minIconSizeUiUnits = 40;  // minimum recognizable icon size in UI units
    } // namespace

    void CMapLayer::init(const std::string &iconSheetPath)
    {
        if (m_iconSheetPath.empty()) { m_iconSheetPath = iconSheetPath; }

        // layers need to be created for maps that already exist
        static const char *maps[] = { XPLM_MAP_USER_INTERFACE, XPLM_MAP_IOS };
        for (const char *map : maps)
        {
            if (XPLMMapExists(map)) { mapCreatedCallback(map, this); }
            XPLMRegisterMapCreationHook(mapCreatedCallback, this);
        }
    }

    void CMapLayer::cleanup()
    {
        for (const auto &pair : m_layers) { XPLMDestroyMapLayer(pair.second); }
        m_layers.clear();
        m_layersCreated = false;
    }

    void CMapLayer::mapCreatedCallback(const char *mapIdentifier, void *refcon)
    {
        if (!mapIdentifier) { return; }
        auto *self = static_cast<CMapLayer *>(refcon);
        if (!self || self->m_layers.count(mapIdentifier) > 0) { return; }

        XPLMCreateMapLayer_t params = {};
        params.structSize = sizeof(params);
        params.mapToCreateLayerIn = mapIdentifier;
        params.layerType = xplm_MapLayer_Markings;
        params.willBeDeletedCallback = mapWillBeDeletedCallback;
        params.prepCacheCallback = mapPrepareCacheCallback;
        params.drawCallback = nullptr;
        params.iconCallback = mapIconDrawingCallback;
        params.labelCallback = nullptr;
        params.showUiToggle = 1;
        params.layerName = "swift network aircraft";
        params.refcon = self;

        const XPLMMapLayerID layerId = XPLMCreateMapLayer(&params);
        if (layerId) { self->m_layers.emplace(mapIdentifier, layerId); }
    }

    void CMapLayer::mapWillBeDeletedCallback(XPLMMapLayerID inLayer, void *refcon)
    {
        auto *self = static_cast<CMapLayer *>(refcon);
        if (!self) { return; }
        for (auto it = self->m_layers.begin(); it != self->m_layers.end(); ++it)
        {
            if (it->second == inLayer)
            {
                self->m_layers.erase(it);
                break;
            }
        }
    }

    void CMapLayer::mapPrepareCacheCallback(XPLMMapLayerID, const float *inTotalMapBoundsLeftTopRightBottom,
                                            XPLMMapProjectionID projection, void *refcon)
    {
        // XPLMMapScaleMeter only works from within a map callback, otherwise it can crash the sim
        auto *self = static_cast<CMapLayer *>(refcon);
        if (!self) { return; }
        self->m_metersPerMapUnit = XPLMMapScaleMeter(
            projection, (inTotalMapBoundsLeftTopRightBottom[2] + inTotalMapBoundsLeftTopRightBottom[0]) / 2.0f,
            (inTotalMapBoundsLeftTopRightBottom[1] + inTotalMapBoundsLeftTopRightBottom[3]) / 2.0f);
    }

    void CMapLayer::mapIconRowCol(const std::string &classification, const std::string &, const std::string &,
                                  int &row, int &col)
    {
        // MapIcons.png layout: row 0: default / L2J / L4J, row 1: L1P / L2P / L4P,
        // row 2: H1T / car / glider
        const char wingClass = classification.empty() ? '\0' : classification[0];
        const int engines = classification.size() > 1 && classification[1] >= '1' && classification[1] <= '9'
                                ? classification[1] - '0'
                                : 0;
        const char engineType = classification.size() > 2 ? classification[2] : '\0';

        if (wingClass == 'H' || wingClass == 'G')
        {
            row = 2;
            col = 0;
            return;
        }
        if (classification == "GLID" || classification == "PARA" || classification == "ULAC")
        {
            row = 2;
            col = 2;
            return;
        }
        if (wingClass == 'A' || wingClass == 'L' || wingClass == 'S' || wingClass == 'T')
        {
            row = engineType == 'J' ? 0 : 1;
            col = engines >= 4 ? 2 : engines >= 2 ? 1 : 0;
            return;
        }
        row = 0;
        col = 0;
    }

    void CMapLayer::mapIconDrawingCallback(XPLMMapLayerID inLayer,
                                           const float *inMapBoundsLeftTopRightBottom, float,
                                           float mapUnitsPerUserInterfaceUnit, XPLMMapStyle,
                                           XPLMMapProjectionID projection, void *inRefcon)
    {
        auto *self = static_cast<CMapLayer *>(inRefcon);
        if (!self || self->m_iconSheetPath.empty()) { return; }

        if (std::isnan(self->m_metersPerMapUnit) || self->m_metersPerMapUnit <= 0.0f)
        {
            mapPrepareCacheCallback(inLayer, inMapBoundsLeftTopRightBottom, projection, inRefcon);
            if (std::isnan(self->m_metersPerMapUnit) || self->m_metersPerMapUnit <= 0.0f) { return; }
        }

        // icon size: 30m fuselage in map units, but at least 40 UI units so it stays identifiable
        const float mapWidth = std::max(c_aircraftSizeM * self->m_metersPerMapUnit,
                                        static_cast<float>(c_minIconSizeUiUnits) * mapUnitsPerUserInterfaceUnit);

        // 3x3 sheet of 100x100px cells (300x300px sheet)
        constexpr int c_cellPx = 100;

        CTraffic *traffic = CTraffic::instance();
        if (!traffic) { return; }

        for (const auto &pair : traffic->getPlanesById())
        {
            const CTraffic::Plane *plane = pair.second;
            if (!plane) { continue; }
            if (!plane->flightModelFilled) { continue; }

            float mapX = 0.0f;
            float mapY = 0.0f;
            XPLMMapProject(projection, plane->positions[3].lat, plane->positions[3].lon, &mapX, &mapY);

            // skip aircraft outside the current map bounds
            if (mapX < inMapBoundsLeftTopRightBottom[0] || mapX > inMapBoundsLeftTopRightBottom[2] ||
                mapY < inMapBoundsLeftTopRightBottom[1] || mapY > inMapBoundsLeftTopRightBottom[3])
            {
                continue;
            }

            int row = 0;
            int col = 0;
            mapIconRowCol(plane->classification, plane->aircraftIcao, plane->wtcLetter, row, col);
            XPLMDrawMapIconFromSheet(inLayer, self->m_iconSheetPath.c_str(), col * c_cellPx, row * c_cellPx,
                                     c_cellPx, c_cellPx, mapX, mapY, xplm_MapOrientation_Map, 0.0f, mapWidth);
        }
    }
} // namespace XSwiftBus
