/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftcfgentrieslist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        namespace FsCommon
        {
            bool CAircraftCfgEntriesList::containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
            {
                if (title.isEmpty()) { return false; }
                return this->containsBy(
                           [ = ](const CAircraftCfgEntries & entries) { return title.compare(entries.getTitle(), caseSensitivity) == 0; }
                       );
            }

            QStringList CAircraftCfgEntriesList::detectAmbiguousTitles() const
            {
                QStringList titles = this->getTitles(true);
                QStringList ambiguousTitles;
                QString last;
                for (const QString &title : titles)
                {
                    if (title.isEmpty()) { continue; }
                    if (title.compare(last, Qt::CaseInsensitive) == 0)
                    {
                        if (!ambiguousTitles.contains(title, Qt::CaseInsensitive))
                        {
                            ambiguousTitles.append(title);
                        }
                    }
                    last = title;
                }
                return ambiguousTitles;
            }

            QStringList CAircraftCfgEntriesList::getTitles(bool sorted) const
            {
                QStringList titles = this->transform(Predicates::MemberTransform(&CAircraftCfgEntries::getTitle));
                if (sorted) { titles.sort(Qt::CaseInsensitive); }
                return titles;
            }

            CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList() const
            {
                CAircraftModelList ml;
                for (const CAircraftCfgEntries &entries : (*this))
                {
                    ml.push_back(entries.toAircraftModel());
                }
                return ml;
            }

            CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList(const CSimulatorInfo &simInfo) const
            {
                CAircraftModelList ml;
                for (const CAircraftCfgEntries &entries : (*this))
                {
                    CAircraftModel m(entries.toAircraftModel());
                    m.setSimulatorInfo(simInfo);
                    ml.push_back(m);
                }
                return ml;
            }

            CAircraftCfgEntriesList CAircraftCfgEntriesList::findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity) const
            {
                return this->findBy([ = ](const CAircraftCfgEntries & entries)
                { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
            }

        } // namespace
    } // namespace
} // namespace
