// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_WAKEWRITER_H
#define SWIFT_SIM_XSWIFTBUS_WAKEWRITER_H

//! \file

#include <XPLM/XPLMDataAccess.h>

#include <string>
#include <unordered_map>

namespace XSwiftBus
{
    class CTraffic;

    /*!
     * Writes XP12 wake turbulence data for the network aircraft into the official
     * dataref arrays (sim/cockpit2/tcas/targets/wake/*). The feature silently
     * disables itself on simulators without those datarefs (XP11 and older).
     *
     * The TCAS slot of each aircraft is determined by matching the aircraft's
     * transponder id (low 24 bits of the plane id, as written by libxplanemp) against
     * the slots populated by the TCAS override, so wake data always corresponds to
     * the aircraft actually visible in the TCAS.
     */
    class CWakeWriter
    {
    public:
        //! Find the wake datarefs, disables the writer if they cannot be found (XP11)
        void init();

        //! Write wake data for all planes with a TCAS slot. Called once per frame after
        //! the TCAS override has written its targets.
        void update(const std::unordered_map<std::string, class CTraffic::Plane *> &planesByCallsign);

        //! Is the wake feature enabled (XP12 datarefs present)?
        bool isEnabled() const { return m_enabled; }

    private:
        struct WakeDefaults
        {
            float wingSpanM;
            float wingAreaM2;
            float massKg;
        };

        //! Defaults per WTC letter, mass is OEW + 80% of (MTOW - OEW)
        static WakeDefaults defaultsForWtc(const std::string &wtc);

        XPLMDataRef m_wingSpanRef = nullptr;
        XPLMDataRef m_wingAreaRef = nullptr;
        XPLMDataRef m_wakeCatRef = nullptr;
        XPLMDataRef m_massRef = nullptr;
        XPLMDataRef m_aoaRef = nullptr;
        XPLMDataRef m_liftRef = nullptr;
        XPLMDataRef m_modeSRef = nullptr;
        bool m_enabled = false;
    };
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_WAKEWRITER_H
