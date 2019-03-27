/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "categorymatcher.h"
#include "simulatedaircraft.h"
#include "aircraftmatchersetup.h"
#include "matchingutils.h"
#include "blackmisc/statusmessagelist.h"

#include <QStringBuilder>

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        const CLogCategoryList &CCategoryMatcher::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::matching() };
            return cats;
        }

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

        CAircraftModelList CCategoryMatcher::reduceByCategories(const CAircraftModelList &modelSet, const CAircraftMatcherSetup &setup, const CSimulatedAircraft &remoteAircraft, bool &reduced, bool shortLog, CStatusMessageList *log) const
        {
            Q_UNUSED(shortLog);

            reduced = false;
            if (!setup.useCategoryMatching())
            {
                if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Disabled category matching"), getLogCategories()); }
                return modelSet;
            }
            if (m_all.isEmpty())
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Disabled category matching"), getLogCategories());
                return modelSet;
            }

            if (!m_gliders.isEmpty() && setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByCategoryGlider) && this->isGlider(remoteAircraft.getAircraftIcaoCode()))
            {
                // we have a glider
                const int firstLevel = this->gliderFirstLevel();
                const CAircraftModelList gliders = modelSet.findByCategoryFirstLevel(firstLevel);
                if (!gliders.isEmpty())
                {
                    const CAircraftCategory category = remoteAircraft.getAircraftIcaoCode().getCategory();
                    reduced = true; // in any case reduced

                    const CAircraftModelList sameGliders = modelSet.findByCategory(category);
                    if (!sameGliders.isEmpty())
                    {
                        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Reduced to %1 models by category : '%2'").arg(sameGliders.size()).arg(category.toQString(true)), getLogCategories()); }
                        return sameGliders;
                    }

                    const CAircraftCategoryList siblings = m_gliders.findSiblings(category);
                    const CAircraftModelList siblingGliders = modelSet.findByCategories(siblings);
                    if (!siblings.isEmpty() && !siblingGliders.isEmpty())
                    {
                        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Reduced to %1 sibling models by categories : '%2'").arg(siblingGliders.size()).arg(siblings.getLevelsString()), getLogCategories()); }
                        return sameGliders;
                    }

                    CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Reduced to %1 models by 'GLIDER' category").arg(sameGliders.size()), getLogCategories());
                    return gliders;
                }
                else
                {
                    if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No glider categories of level id %1 in set").arg(firstLevel), getLogCategories()); }
                    static const QStringList substituteIcaos({ "UHEL", "GLID", "ULAC" }); // maybe also GYRO
                    static const QString substituteIcaosStr = substituteIcaos.join(", ");

                    CAircraftModelList substitutes = modelSet.findByDesignatorsOrFamilyWithColorLivery(substituteIcaos);
                    if (substitutes.isEmpty())
                    {
                        substitutes = modelSet.findByCombinedType(QStringLiteral("L1P"));
                        if (!substitutes.isEmpty())
                        {
                            reduced = true;
                            if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No gliders, reduced to 'L1P' models: %1' (avoid absurd matchings)").arg(substitutes.size()), getLogCategories()); }
                            return substitutes;
                        }
                    }
                    else
                    {
                        reduced = true;
                        if (log) { CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Reduced to %1 models by '%2'").arg(substitutes.size()).arg(substituteIcaosStr), getLogCategories()); }
                        return substitutes;
                    }
                }
            }

            return modelSet;
        }

        bool CCategoryMatcher::isGlider(const CAircraftIcaoCode &icao) const
        {
            if (icao.getDesignator() == CAircraftIcaoCode::getGliderDesignator()) { return true; }
            const int glider1st = this->gliderFirstLevel();
            if (glider1st >= 0 && icao.hasCategory())
            {
                return icao.getCategory().getFirstLevel() == glider1st;
            }
            return false;
        }

        int CCategoryMatcher::gliderFirstLevel() const
        {
            if (m_gliders.isEmpty()) { return -1; }
            return m_gliders.front().getFirstLevel();
        }
    } // namespace
} // namespace
