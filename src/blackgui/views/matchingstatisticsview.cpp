// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "matchingstatisticsview.h"

using namespace BlackGui::Models;

namespace BlackGui::Views
{
    CMatchingStatisticsView::CMatchingStatisticsView(QWidget *parent) : CViewBase(parent)
    {
        this->standardInit(new CMatchingStatisticsModel(CMatchingStatisticsModel::ForSingleSession, this));
    }

    void CMatchingStatisticsView::setStatisticsModelMode(CMatchingStatisticsModel::MatchingStatisticsMode mode)
    {
        this->m_model->setMode(mode);
    }
} // namespace
