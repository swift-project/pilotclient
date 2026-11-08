// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_SIM_XSWIFTBUS_AIRCRAFTCATEGORY_H
#define SWIFT_SIM_XSWIFTBUS_AIRCRAFTCATEGORY_H

//! \file

#include <map>
#include <regex>
#include <string>
#include <vector>

namespace XSwiftBus
{
    /*!
     * Per-category animation parameters for network aircraft.
     * Values are ported 1:1 from the reference implementation.
     */
    struct FlightModel
    {
        std::string modelCategory;
        float flapsDurationMs = 5000;
        float gearDurationMs = 10000;
        float gearDeflectionM = 0.5;
    };

    /*!
     * Derives wake/animation data from the ICAO type code using the Doc8643 table
     * shipped with the plugin (LegacyData/Doc8643.txt).
     */
    struct AircraftCategory
    {
        std::string wtc;             //!< "L" "M" "H" "J" "-"
        std::string classification;  //!< e.g. "L2J", "H1T", "GLID"
        std::string category;        //!< matched flight model category, e.g. "MediumJets"
        int engineCount = 0;
        char engineType = '\0';
        bool isHelicopter = false;
        bool isGlider = false;

        //! Wake turbulence category as used by XP12 (0=L 1=M 2=H 3=Super). "S" treated as Super as well.
        int wakeCategory() const
        {
            if (wtc == "L") { return 0; }
            if (wtc == "M") { return 1; }
            if (wtc == "H") { return 2; }
            if (wtc == "J" || wtc == "S") { return 3; }
            return 1;
        }

        //! Flight model parameters for this category
        FlightModel flightModel() const
        {
            FlightModel fm;
            fm.modelCategory = category;
            if (category == "HugeJets")
            {
                fm.flapsDurationMs = 10000;
                fm.gearDurationMs = 10000;
                fm.gearDeflectionM = 1.4f;
            }
            else if (category == "BizJet")
            {
                // the reference implementation uses 0.25ms (≈ instant) which looks like a typo;
                // we use 2500ms as a sane value
                fm.flapsDurationMs = 5000;
                fm.gearDurationMs = 2500;
                fm.gearDeflectionM = 0.5f;
            }
            else if (category == "GA" || category == "LightAC" || category == "Heli")
            {
                fm.flapsDurationMs = 5000;
                fm.gearDurationMs = 10000;
                fm.gearDeflectionM = 0.25f;
            }
            else
            {
                fm.flapsDurationMs = 5000;
                fm.gearDurationMs = 10000;
                fm.gearDeflectionM = 0.5f;
            }
            return fm;
        }
    };

    /*!
     * Database of ICAO type codes based on the Doc8643 table. Singleton as the
     * table is parsed once and then only read.
     */
    class CDoc8643Database
    {
    public:
        //! Singleton access
        static CDoc8643Database &instance();

        //! Load the Doc8643 table, returns false if the file cannot be parsed
        bool load(const std::string &doc8643FilePath);

        //! Classification for a type code, empty string if unknown
        AircraftCategory classify(const std::string &icaoType) const;

    private:
        CDoc8643Database() = default;

        struct Entry
        {
            std::string classification;
            std::string wtc;
        };

        std::map<std::string, Entry> m_entries;
        bool m_loaded = false;
    };

    /*!
     * Categorize an aircraft into its flight model based on the classification string.
     * Matching table ported 1:1 from the reference implementation.
     */
    inline std::string categorize(const std::string &wtc, const std::string &classification,
                                  const std::string &icaoType)
    {
        static const std::vector<std::pair<std::string, std::string>> modelMatches = {
            { "HugeJets", "^(H|J);L\\dJ;" },
            { "HugeJets", "^M;L4J;" },
            { "BizJet", "^M;L\\dJ;*BEECH*" },
            { "BizJet", "^M;L\\dJ;GLF" },
            { "BizJet", "^M;L\\dJ;LJ" },
            { "BizJet", "^M;L\\dJ;.*;CESSNA" },
            { "BizJet", "^M;L\\dJ;.*;DASSAULT" },
            { "MediumJets", "^M;L\\dJ;" },
            { "MediumProps", "^M;L\\dT;" },
            { "BizJet", "^L;L\\dJ;" },
            { "Glider",
              ";(GLID|A20J|A33P|A33E|A34E|ARCE|ARCP|AS14|AS16|AS20|AS21|AS22|AS24|AS25|AS26|AS28|AS29|AS30|AS31|DG1T|"
              "DG40|DG50|DG60|DG80|DIMO|DISC|DUOD|G103|G109|HU1|HU2|JANU|L13M|LAE1|LK17|LK19|LK20|LS8|LS9|NIMB|PISI|"
              "PITE|PITA|PIT4|PK15|PK20|S10S|S32M|S32E|SF24|SF25|SF27|SF28|SF31|SZ45|SZ9M|TS1J|VENT);" },
            { "LightAC", "^-" },
            { "TurboProps", "^L;L\\dT;" },
            { "GA", "^L;L\\dP;" },
            { "Heli", "^.;H" },
            // Fallback
            { "MediumJets", ".*" }
        };

        const std::string s = wtc + ";" + classification + ";" + icaoType + ";";
        for (const auto &match : modelMatches)
        {
            const std::regex re(match.second);
            std::smatch m;
            std::regex_search(s, m, re);
            if (m.size() > 0) { return match.first; }
        }
        return "MediumJets";
    }
} // namespace XSwiftBus

#endif // SWIFT_SIM_XSWIFTBUS_AIRCRAFTCATEGORY_H
