// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/fscommon/aircraftcfgentrieslist.h"

#include "misc/predicates.h"
#include "misc/range.h"
#include "misc/setbuilder.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/statusmessagelist.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::simulation;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::simulation::fscommon, CAircraftCfgEntries, CAircraftCfgEntriesList)

namespace swift::misc::simulation::fscommon
{
    CAircraftCfgEntriesList::CAircraftCfgEntriesList() = default;

    CAircraftCfgEntriesList::CAircraftCfgEntriesList(const CSequence<CAircraftCfgEntries> &other) : CSequence(other) {}

    bool CAircraftCfgEntriesList::containsModelWithTitle(const QString &title, Qt::CaseSensitivity caseSensitivity)
    {
        if (title.isEmpty()) { return false; }
        return this->containsBy([=](const CAircraftCfgEntries &entries) { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
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
        QStringList titles = this->transform(predicates::MemberTransform(&CAircraftCfgEntries::getTitle));
        if (sorted) { titles.sort(Qt::CaseInsensitive); }
        return titles;
    }

    QSet<QString> CAircraftCfgEntriesList::getTitleSetUpperCase() const
    {
        CSetBuilder<QString> titlesUc;
        for (const CAircraftCfgEntries &entries : *this)
        {
            titlesUc.insert(entries.getTitle().toUpper());
        }
        return titlesUc;
    }

    QString CAircraftCfgEntriesList::getTitlesAsString(bool sorted, const QString &separator) const
    {
        return this->getTitles(sorted).join(separator);
    }

    bool CAircraftCfgEntriesList::containsTitle(const QString &title) const
    {
        if (title.isEmpty()) { return false; }
        for (const CAircraftCfgEntries &entries : (*this))
        {
            if (stringCompare(entries.getTitle(), title, Qt::CaseInsensitive)) { return true; }
        }
        return false;
    }

    CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList(bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const
    {
        CAircraftModelList ml;
        QSet<QString> keys;
        for (const CAircraftCfgEntries &entries : (*this))
        {
            if (ignoreDuplicatesAndEmptyModelStrings)
            {
                const QString key = entries.getTitle().toUpper();
                if (key.isEmpty()) { continue; }
                if (keys.contains(key))
                {
                    CStatusMessage msg(this);
                    msg.warning(u"Duplicate model string %1 in %2 %3")
                        << entries.getTitle() << entries.getFileDirectory() << entries.getFileName();
                    msgs.push_back(msg);
                    continue;
                }
                keys.insert(key);
            }
            ml.push_back(entries.toAircraftModel());
        }
        return ml;
    }

    CAircraftModelList CAircraftCfgEntriesList::toAircraftModelList(const CSimulatorInfo &simInfo, bool ignoreDuplicatesAndEmptyModelStrings, CStatusMessageList &msgs) const
    {
        CAircraftModelList ml = this->toAircraftModelList(ignoreDuplicatesAndEmptyModelStrings, msgs);
        ml.setSimulatorInfo(simInfo);
        return ml;
    }

    CAircraftCfgEntriesList CAircraftCfgEntriesList::findByTitle(const QString &title, Qt::CaseSensitivity caseSensitivity) const
    {
        return this->findBy([=](const CAircraftCfgEntries &entries) { return title.compare(entries.getTitle(), caseSensitivity) == 0; });
    }

} // namespace swift::misc::simulation::fscommon
