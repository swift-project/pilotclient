/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "categorymatcher.h"
#include <QStringBuilder>

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        void CCategoryMatcher::setCategories(const CAircraftCategoryList &categories)
        {
            m_all = categories;

            CAircraftCategoryList gliders = categories.findByName("glider").findFirstLevels();
            if (!gliders.isEmpty())
            {
                const int fl = gliders.front().getFirstLevel();
                gliders = categories.findByFirstLevel(fl);
                gliders.sortByLevel();
                m_gliders = gliders;
            }

            CAircraftCategoryList militaryWing = categories.findByName("wing military").findFirstLevels();
            if (!militaryWing.isEmpty())
            {
                const int fl = militaryWing.front().getFirstLevel();
                militaryWing = categories.findByFirstLevel(fl);
                militaryWing.sortByLevel();
                m_militaryWingAircraft = militaryWing;
            }

            CAircraftCategoryList militaryRotor = categories.findByName("rotor military").findFirstLevels();
            if (!militaryRotor.isEmpty())
            {
                const int fl = militaryRotor.front().getFirstLevel();
                militaryRotor = categories.findByFirstLevel(fl);
                militaryRotor.sortByLevel();
                m_militaryRotorAircraft = militaryRotor;
            }
        }
    } // namespace
} // namespace
