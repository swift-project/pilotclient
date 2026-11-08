// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftcategory.h"

#include <fstream>
#include <mutex>

namespace XSwiftBus
{
    CDoc8643Database &CDoc8643Database::instance()
    {
        static CDoc8643Database db;
        return db;
    }

    bool CDoc8643Database::load(const std::string &doc8643FilePath)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (m_loaded) { return true; }

        std::ifstream file(doc8643FilePath);
        if (!file) { return false; }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#' || line[0] == ';') { continue; }
            std::vector<std::string> cols;
            std::string::size_type pos = 0;
            while (pos <= line.size())
            {
                const auto next = line.find('\t', pos);
                cols.push_back(line.substr(pos, next == std::string::npos ? next : next - pos));
                if (next == std::string::npos) { break; }
                pos = next + 1;
            }
            // columns: manufacturer, description, model name, type code, classification, WTC
            if (cols.size() < 6 || cols.at(3).empty()) { continue; }
            const std::string type = cols.at(3);
            m_entries[type] = { cols.at(4), cols.at(5) };
        }

        m_loaded = !m_entries.empty();
        return m_loaded;
    }

    AircraftCategory CDoc8643Database::classify(const std::string &icaoType) const
    {
        AircraftCategory category;
        const auto it = m_entries.find(icaoType);
        if (it != m_entries.end())
        {
            category.classification = it->second.classification;
            category.wtc = it->second.wtc;
        }
        else
        {
            // unknown type, use the class from the designator if it looks like a classification
            category.classification.clear();
            category.wtc = "-";
        }

        // engine count / type from the classification, e.g. "L2J" -> 2 / 'J', "H1T" -> 1 / 'T'
        for (const char c : category.classification)
        {
            if (c >= '1' && c <= '9') { category.engineCount = c - '0'; }
            else if (c == 'J' || c == 'T' || c == 'P' || c == 'E') { category.engineType = c; }
        }
        category.isHelicopter = !category.classification.empty() && category.classification[0] == 'H';
        category.isGlider = !category.classification.empty() && category.classification[0] == 'G';

        category.category = categorize(category.wtc, category.classification, icaoType);
        return category;
    }
} // namespace XSwiftBus
