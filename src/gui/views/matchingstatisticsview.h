// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_MATCHINGSTATISTICSVIEW_H
#define SWIFT_GUI_VIEWS_MATCHINGSTATISTICSVIEW_H

#include "gui/models/matchingstatisticsmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewbase.h"
#include "misc/network/serverlist.h"

class QWidget;

namespace swift::gui::views
{
    //! Matching statistics
    class SWIFT_GUI_EXPORT CMatchingStatisticsView : public CViewBase<models::CMatchingStatisticsModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMatchingStatisticsView(QWidget *parent = nullptr);

        //! Set vie mode
        void setStatisticsModelMode(models::CMatchingStatisticsModel::MatchingStatisticsMode mode);
    };
} // namespace swift::gui::views
#endif // guard
