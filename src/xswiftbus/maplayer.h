// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_MAPLAYER_H
#define SWIFT_SIM_XSWIFTBUS_MAPLAYER_H

//! \file

#include <XPLM/XPLMMap.h>

#include <map>
#include <string>

namespace XSwiftBus
{
    class CTraffic;

    /*!
     * Draws the network aircraft into X-Plane's built-in maps (user interface map
     * and instructor station). A 3x3 png icon sheet is used, the icon per aircraft
     * is selected based on its Doc8643 classification.
     */
    class CMapLayer
    {
    public:
        //! Register map creation hooks and create layers for existing maps
        void init(const std::string &iconSheetPath);

        //! Remove layers and hooks
        void cleanup();

    private:
        //! Called when X-Plane creates a map, creates our layer in it
        static void mapCreatedCallback(const char *mapIdentifier, void *refcon);

        //! Called when a map layer is about to be deleted
        static void mapWillBeDeletedCallback(XPLMMapLayerID inLayer, void *refcon);

        //! Called when the map projection changes, caches meters per map unit
        static void mapPrepareCacheCallback(XPLMMapLayerID inLayer,
                                            const float *inTotalMapBoundsLeftTopRightBottom,
                                            XPLMMapProjectionID projection, void *refcon);

        //! Called per frame to draw the icons
        static void mapIconDrawingCallback(XPLMMapLayerID inLayer,
                                           const float *inMapBoundsLeftTopRightBottom, float zoomRatio,
                                           float mapUnitsPerUserInterfaceUnit, XPLMMapStyle mapStyle,
                                           XPLMMapProjectionID projection, void *inRefcon);

        //! Select the sprite sheet cell for an aircraft based on its classification
        static void mapIconRowCol(const std::string &classification, const std::string &icaoType,
                                  const std::string &wtc, int &row, int &col);

        std::map<std::string, XPLMMapLayerID> m_layers;
        std::string m_iconSheetPath;
        float m_metersPerMapUnit = 0.0f;
        bool m_layersCreated = false;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_MAPLAYER_H
