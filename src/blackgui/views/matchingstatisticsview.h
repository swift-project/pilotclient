// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_MATCHINGSTATISTICSVIEW_H
#define BLACKGUI_VIEWS_MATCHINGSTATISTICSVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/matchingstatisticsmodel.h"
#include "blackgui/views/viewbase.h"
#include "misc/network/serverlist.h"

class QWidget;

namespace BlackGui::Views
{
    //! Matching statistics
    class BLACKGUI_EXPORT CMatchingStatisticsView : public CViewBase<Models::CMatchingStatisticsModel>
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CMatchingStatisticsView(QWidget *parent = nullptr);

        //! Set vie mode
        void setStatisticsModelMode(Models::CMatchingStatisticsModel::MatchingStatisticsMode mode);
    };
} // ns
#endif // guard
