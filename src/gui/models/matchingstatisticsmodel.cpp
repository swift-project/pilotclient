// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "matchingstatisticsmodel.h"

#include <QtGlobal>

#include "gui/models/columns.h"

using namespace swift::misc::simulation;

namespace swift::gui::models
{
    CMatchingStatisticsModel::CMatchingStatisticsModel(MatchingStatisticsMode mode, QObject *parent)
        : CListModelBase("MatchingStatisticsModel", parent)
    {
        this->setMode(mode);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "session");
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "model set");
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "combination");
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "type");
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "aircraft");
        (void)QT_TRANSLATE_NOOP("MatchingStatisticsModel", "airline");
    }

    void CMatchingStatisticsModel::setMode(CMatchingStatisticsModel::MatchingStatisticsMode mode)
    {
        if (m_mode == mode) { return; }
        m_mode = mode;
        m_columns.clear();
        switch (mode)
        {
        case ForMultiSessions:
            m_columns.addColumn(CColumn::standardString("session", CMatchingStatisticsEntry::IndexSessionId));
            m_columns.addColumn(CColumn::standardString("model set", CMatchingStatisticsEntry::IndexModelSetId));
        // fall thru
        case ForSingleSession:
            m_columns.addColumn(CColumn("type", CMatchingStatisticsEntry::IndexEntryTypeAsIcon));
            m_columns.addColumn(CColumn::standardString("aircraft", CMatchingStatisticsEntry::IndexAircraftDesignator));
            m_columns.addColumn(CColumn::standardString("airline", CMatchingStatisticsEntry::IndexAirlineDesignator));
            m_columns.addColumn(CColumn::standardInteger("#", "count", CMatchingStatisticsEntry::IndexCount));
            m_columns.addColumn(CColumn::standardString("description", CMatchingStatisticsEntry::IndexDescription));
            break;
        default: break;
        }
        this->setSortColumnByPropertyIndex(CMatchingStatisticsEntry::IndexAircraftDesignator);
    }
} // namespace swift::gui::models
