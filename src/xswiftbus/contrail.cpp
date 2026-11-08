// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "contrail.h"

#include "traffic.h"

#include <XPLM/XPLMObjects.h>
#include <XPLM/XPLMUtilities.h>

#include <cmath>

namespace XSwiftBus
{
    namespace
    {
        constexpr double c_altMinFt = 25000.0;
        constexpr double c_altMaxFt = 45000.0;
    } // namespace

    void CContrails::init(const std::string &resourcesPath)
    {
        m_resourcesPath = resourcesPath;
        m_contrailObject = XPLMLoadObject(
            (resourcesPath + "Contrail" + XPLM_DIRSEP + "Contrail.obj").c_str());
        m_objectLoaded = m_contrailObject != nullptr;
    }

    void CContrails::cleanup()
    {
        for (const auto &pair : m_instances)
        {
            if (pair.second.instance) { XPLMDestroyInstance(pair.second.instance); }
        }
        m_instances.clear();
        if (m_contrailObject) { XPLMUnloadObject(m_contrailObject); }
        m_contrailObject = nullptr;
        m_objectLoaded = false;
    }

    void CContrails::removePlane(const std::string &callsign)
    {
        this->updateInstance(callsign, false);
    }

    void CContrails::updateInstance(const std::string &callsign, bool shouldHaveContrail)
    {
        if (!m_objectLoaded) { return; }

        const auto it = m_instances.find(callsign);
        const bool hasInstance = it != m_instances.end() && it->second.instance;

        if (shouldHaveContrail && !hasInstance)
        {
            static const char *drefs[] = { "", "libxplanemp/controls/thrust_ratio",
                                           "libxplanemp/contrail/lifetime", nullptr };
            ContrailInstance inst;
            inst.instance = XPLMCreateInstance(m_contrailObject, drefs);
            inst.visible = true;
            m_instances[callsign] = inst;
        }
        else if (!shouldHaveContrail && hasInstance)
        {
            XPLMDestroyInstance(it->second.instance);
            m_instances.erase(it);
        }
    }

    void CContrails::updatePositions()
    {
        if (!m_objectLoaded) { return; }
        CTraffic *traffic = CTraffic::instance();
        if (!traffic) { return; }

        for (const auto &pair : traffic->getPlanesById())
        {
            const CTraffic::Plane *plane = pair.second;
            if (!plane || !plane->flightModelFilled) { continue; }

            const bool isJet = plane->classification.size() > 2 && plane->classification[2] == 'J';
            const bool inAltitudeRange = plane->positions[3].elevation >= c_altMinFt &&
                                         plane->positions[3].elevation <= c_altMaxFt;
            const bool enginesRunning = plane->surfaces.thrust > 0.1f;
            const bool shouldHave = isJet && inAltitudeRange && enginesRunning;
            this->updateInstance(plane->callsign, shouldHave);

            const auto instIt = m_instances.find(plane->callsign);
            if (instIt == m_instances.end() || !instIt->second.instance) { continue; }

            XPLMDrawInfo_t drawInfo = {};
            drawInfo.structSize = sizeof(drawInfo);
            drawInfo.lat = plane->positions[3].lat;
            drawInfo.lon = plane->positions[3].lon;
            drawInfo.alt = plane->positions[3].elevation * 0.3048f;
            drawInfo.pitch = plane->positions[3].pitch;
            drawInfo.roll = plane->positions[3].roll;
            drawInfo.heading = plane->positions[3].heading;

            const float data[2] = { plane->surfaces.thrust, 120.0f };
            XPLMInstanceSetPosition(instIt->second.instance, &drawInfo, data);
        }
    }
} // namespace XSwiftBus
