// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/categorymatcher.h"

#include <QStringBuilder>

#include "misc/simulation/aircraftmatchersetup.h"
#include "misc/simulation/matchingutils.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/statusmessagelist.h"

using namespace swift::misc::aviation;

namespace swift::misc::simulation
{
    const QStringList &CCategoryMatcher::getLogCategories()
    {
        static const QStringList cats { CLogCategories::matching() };
        return cats;
    }

    void CCategoryMatcher::setCategories(const CAircraftCategoryList &categories)
    {
        m_all = categories;
        m_all.sortByLevel();

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

    CAircraftModelList CCategoryMatcher::reduceByCategories(const CAircraftModelList &alreadyMatchedModels,
                                                            const CAircraftModelList &modelSet,
                                                            const CAircraftMatcherSetup &setup,
                                                            const CSimulatedAircraft &remoteAircraft, bool &reduced,
                                                            bool shortLog, CStatusMessageList *log) const
    {
        Q_UNUSED(shortLog)

        reduced = false;
        if (!setup.useCategoryMatching())
        {
            if (log)
            {
                CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Disabled category matching"),
                                                    getLogCategories());
            }
            return alreadyMatchedModels;
        }
        if (m_all.isEmpty())
        {
            // no categories?
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("Disabled category matching"),
                                                getLogCategories());
            return alreadyMatchedModels;
        }
        if (!remoteAircraft.getAircraftIcaoCode().hasCategory())
        {
            CMatchingUtils::addLogDetailsToList(log, remoteAircraft, QStringLiteral("No category in remote aircraft"),
                                                getLogCategories());
            return alreadyMatchedModels;
        }

        if (log)
        {
            CMatchingUtils::addLogDetailsToList(
                log, remoteAircraft,
                QStringLiteral("Remote aircraft has category '%1'")
                    .arg(remoteAircraft.getAircraftIcaoCode().getCategory().getNameDbKey()),
                getLogCategories());
        }
        if (!m_gliders.isEmpty() && setup.getMatchingMode().testFlag(CAircraftMatcherSetup::ByCategoryGlider) &&
            this->isGlider(remoteAircraft.getAircraftIcaoCode()))
        {
            // we have a glider
            // and we search in the whole set: this is a special case
            const int firstLevel = this->gliderFirstLevel();
            const CAircraftModelList gliders =
                modelSet.findByCategoryFirstLevel(firstLevel); // all gliders from model set
            if (!gliders.isEmpty())
            {
                const CAircraftCategory category = remoteAircraft.getAircraftIcaoCode().getCategory();
                reduced = true; // in any case reduced (to gliders)

                // find same category
                const CAircraftModelList sameGliders = gliders.findByCategory(category);
                if (!sameGliders.isEmpty())
                {
                    if (log)
                    {
                        CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                            QStringLiteral("Reduced to %1 models by category: '%2'")
                                                                .arg(sameGliders.size())
                                                                .arg(category.toQString(true)),
                                                            getLogCategories());
                    }
                    return sameGliders;
                }

                // find parallel branch category
                const CAircraftCategoryList otherBranches = m_gliders.findInParallelBranch(category);
                if (!otherBranches.isEmpty())
                {
                    const CAircraftModelList otherBranchGliders = gliders.findByCategories(otherBranches);
                    if (!otherBranchGliders.isEmpty())
                    {
                        if (log)
                        {
                            CMatchingUtils::addLogDetailsToList(
                                log, remoteAircraft,
                                QStringLiteral("Reduced to %1 parallel branch models of '%2' by categories: '%3'")
                                    .arg(otherBranchGliders.size())
                                    .arg(category.getLevelAndName(), otherBranches.getLevelsString()),
                                getLogCategories());
                        }
                        return otherBranchGliders;
                    }
                }

                const CAircraftCategoryList siblings = m_gliders.findSiblings(category);
                if (!siblings.isEmpty())
                {
                    const CAircraftModelList siblingGliders = modelSet.findByCategories(siblings);
                    if (!siblings.isEmpty() && !siblingGliders.isEmpty())
                    {
                        if (log)
                        {
                            CMatchingUtils::addLogDetailsToList(
                                log, remoteAircraft,
                                QStringLiteral("Reduced to %1 sibling models of '%2' by categories: '%3'")
                                    .arg(siblingGliders.size())
                                    .arg(category.getLevelAndName(), siblings.getLevelsString()),
                                getLogCategories());
                        }
                        return siblingGliders;
                    }
                }

                CMatchingUtils::addLogDetailsToList(
                    log, remoteAircraft,
                    QStringLiteral("Reduced to %1 models by 'GLIDER' category").arg(sameGliders.size()),
                    getLogCategories());
                return gliders;
            }
            else
            {
                if (log)
                {
                    CMatchingUtils::addLogDetailsToList(
                        log, remoteAircraft,
                        QStringLiteral("No glider category '%1' in set").arg(m_gliders.front().getLevelAndName()),
                        getLogCategories());
                }
                static const QStringList substituteIcaos({ "UHEL", "GLID", "ULAC" }); // maybe also GYRO
                static const QString substituteIcaosStr = substituteIcaos.join(", ");

                CAircraftModelList substitutes =
                    alreadyMatchedModels.findByDesignatorsOrFamilyWithColorLivery(substituteIcaos);
                if (substitutes.isEmpty())
                {
                    substitutes = alreadyMatchedModels.findByCombinedType(QStringLiteral("L1P"));
                    if (!substitutes.isEmpty())
                    {
                        reduced = true;
                        if (log)
                        {
                            CMatchingUtils::addLogDetailsToList(
                                log, remoteAircraft,
                                QStringLiteral("No gliders, reduced to 'L1P' models: %1' (avoid absurd matchings)")
                                    .arg(substitutes.size()),
                                getLogCategories());
                        }
                        return substitutes;
                    }
                }
                else
                {
                    reduced = true;
                    if (log)
                    {
                        CMatchingUtils::addLogDetailsToList(log, remoteAircraft,
                                                            QStringLiteral("Reduced to %1 models by '%2'")
                                                                .arg(substitutes.size())
                                                                .arg(substituteIcaosStr),
                                                            getLogCategories());
                    }
                    return substitutes;
                }
            }
        }

        return alreadyMatchedModels;
    }

    bool CCategoryMatcher::isGlider(const CAircraftIcaoCode &icao) const
    {
        if (icao.getDesignator() == CAircraftIcaoCode::getGliderDesignator()) { return true; }
        const int glider1st = this->gliderFirstLevel();
        if (glider1st >= 0 && icao.hasCategory()) { return icao.getCategory().getFirstLevel() == glider1st; }
        return false;
    }

    int CCategoryMatcher::gliderFirstLevel() const
    {
        if (m_gliders.isEmpty()) { return -1; }
        return m_gliders.front().getFirstLevel();
    }
} // namespace swift::misc::simulation
