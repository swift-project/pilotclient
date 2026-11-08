// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_CONTRAIL_H
#define SWIFT_SIM_XSWIFTBUS_CONTRAIL_H

//! \file

#include <XPLM/XPLMInstance.h>
#include <XPLM/XPLMObjects.h>

#include <map>
#include <string>

namespace XSwiftBus
{
    class CTraffic;

    /*!
     * Contrail particle instances for high-altitude jet aircraft. Uses X-Plane's
     * built-in particle system (obj + pss + png assets). Instances are created
     * lazily for jets that the altitude gate qualifies.
     */
    class CContrails
    {
    public:
        //! Load the contrail object, initialize the altitude gate
        void init(const std::string &resourcesPath);

        //! Destroy all instances and unload the object
        void cleanup();

        //! Destroy the instance for a removed plane
        void removePlane(const std::string &callsign);

        //! Once per second: check which jets qualify for a contrail based on altitude
        void updateGating(double ownAltitudeFt);

        //! Once per frame: position all active contrail instances
        void updatePositions();

    private:
        //! Per-aircraft contrail state
        struct ContrailInstance
        {
            XPLMInstanceRef instance = nullptr;
            bool visible = false;
        };

        //! Create or destroy the instance for a given plane
        void updateInstance(const std::string &callsign, bool shouldHaveContrail);

        XPLMObjectRef m_contrailObject = nullptr;
        bool m_objectLoaded = false;
        bool m_altitudeSampled = false;
        double m_ownAltitudeFt = 0.0;
        std::map<std::string, ContrailInstance> m_instances;
        std::string m_resourcesPath;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_CONTRAIL_H
