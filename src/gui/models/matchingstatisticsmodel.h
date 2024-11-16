// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MATCHINGSTATISTICSMODEL_H
#define SWIFT_GUI_MATCHINGSTATISTICSMODEL_H

#include "gui/models/listmodelbase.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/matchingstatistics.h"

class QObject;

namespace swift::gui::models
{
    //! Matching statistics entry
    class SWIFT_GUI_EXPORT CMatchingStatisticsModel :
        public CListModelBase<swift::misc::simulation::CMatchingStatistics, true>
    {
        Q_OBJECT

    public:
        //! How to display
        enum MatchingStatisticsMode
        {
            NoSet,
            ForSingleSession,
            ForMultiSessions
        };

        //! Constructor
        explicit CMatchingStatisticsModel(MatchingStatisticsMode mode, QObject *parent = nullptr);

        //! Destructor
        virtual ~CMatchingStatisticsModel() {}

        //! Set mode
        void setMode(MatchingStatisticsMode mode);

    private:
        MatchingStatisticsMode m_mode = NoSet;
    };
} // namespace swift::gui::models
#endif // guard
