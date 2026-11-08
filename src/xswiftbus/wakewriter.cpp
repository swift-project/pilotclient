// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "wakewriter.h"

#include "traffic.h"

#include <XPLM/XPLMUtilities.h>

#include <cmath>
#include <cstring>

namespace XSwiftBus
{
    namespace
    {
        constexpr std::size_t c_tcasSlotCount = 64; // slot 0 is the user aircraft, 1..63 are remote aircraft
        constexpr float g_earth = 9.80665f;
    } // namespace

    CWakeWriter::WakeDefaults CWakeWriter::defaultsForWtc(const std::string &wtc)
    {
        // masses are OEW + 80% of (MTOW - OEW), representative aircraft:
        // L: C172, L/M: B350, M: A320, H: B744, J: A388
        if (wtc == "L") { return { 11.00f, 16.2f, 1037.6f }; }
        if (wtc == "L/M") { return { 17.65f, 28.8f, 6301.0f }; }
        if (wtc == "J") { return { 79.80f, 845.0f, 510560.0f }; }
        if (wtc == "H") { return { 64.40f, 541.2f, 367129.4f }; }
        return { 34.09f, 122.6f, 74500.0f }; // M and unknown
    }

    void CWakeWriter::init()
    {
        m_wingSpanRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/wing_span_m");
        m_wingAreaRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/wing_area_m2");
        m_wakeCatRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/wake_cat");
        m_massRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/mass_kg");
        m_aoaRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/aoa");
        m_liftRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/wake/lift_N");
        m_modeSRef = XPLMFindDataRef("sim/cockpit2/tcas/targets/modeS_id");

        // all datarefs must exist (XP12+), otherwise the feature silently disables itself
        m_enabled = m_wingSpanRef && m_wingAreaRef && m_wakeCatRef && m_massRef && m_aoaRef && m_liftRef &&
                    m_modeSRef;
    }

    void CWakeWriter::update(const std::unordered_map<std::string, CTraffic::Plane *> &planesByCallsign)
    {
        if (!m_enabled) { return; }

        // map TCAS slots by transponder id, the slots 1..63 have been written by libxplanemp
        // this very frame (Render_PrepLists -> TCAS::pushPlanes)
        int slotByModeS[c_tcasSlotCount] = {};
        std::memset(slotByModeS, -1, sizeof(slotByModeS));
        int modeSValues[c_tcasSlotCount] = {};
        XPLMGetDatavi(m_modeSRef, modeSValues, 0, c_tcasSlotCount);
        for (int slot = 1; slot < static_cast<int>(c_tcasSlotCount); ++slot)
        {
            const int modeS = modeSValues[slot];
            if (modeS > 0) { slotByModeS[modeS] = slot; }
        }

        for (const auto &pair : planesByCallsign)
        {
            const CTraffic::Plane *plane = pair.second;
            if (!plane || !plane->flightModelFilled) { continue; }

            const auto modeS = static_cast<int>(reinterpret_cast<std::uintptr_t>(plane->id) & 0xffffffu);
            if (modeS <= 0 || modeS >= static_cast<int>(c_tcasSlotCount)) { continue; }
            const int slot = slotByModeS[modeS];
            if (slot <= 0) { continue; } // aircraft has no TCAS slot this frame

            const CWakeWriter::WakeDefaults defaults = defaultsForWtc(plane->wtcLetter);
            const float aoaValue = plane->positions[3].pitch; // approximation, like the reference
            const float liftValue =
                plane->isOnGround ? 0.0f : defaults.massKg * g_earth; // no wake for reported-on-ground aircraft
            const int catValue = plane->wakeCategory;

            XPLMSetDatavf(m_wingSpanRef, &defaults.wingSpanM, slot, 1);
            XPLMSetDatavf(m_wingAreaRef, &defaults.wingAreaM2, slot, 1);
            XPLMSetDatavi(m_wakeCatRef, &catValue, slot, 1);
            XPLMSetDatavf(m_massRef, &defaults.massKg, slot, 1);
            XPLMSetDatavf(m_aoaRef, &aoaValue, slot, 1);
            XPLMSetDatavf(m_liftRef, &liftValue, slot, 1);
        }
    }
} // namespace XSwiftBus
