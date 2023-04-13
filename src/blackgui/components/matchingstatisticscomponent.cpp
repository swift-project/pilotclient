/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingstatisticscomponent.h"
#include "ui_matchingstatisticscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"

using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CMatchingStatisticsComponent::CMatchingStatisticsComponent(QWidget *parent) : QFrame(parent),
                                                                                  ui(new Ui::CMatchingStatisticsComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Generate, &QPushButton::clicked, this, &CMatchingStatisticsComponent::onGenerateClicked);
        connect(ui->tvp_MatchingStatistics, &CMatchingStatisticsView::requestNewBackendData, this, &CMatchingStatisticsComponent::onGenerateClicked);
        connect(ui->tvp_MatchingStatistics, &CMatchingStatisticsView::requestUpdate, this, &CMatchingStatisticsComponent::onGenerateClicked);
    }

    CMatchingStatisticsComponent::~CMatchingStatisticsComponent()
    {}

    void CMatchingStatisticsComponent::onGenerateClicked()
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "Missing context");

        const bool missingOnly = ui->rb_MissingOnly->isChecked();
        const CMatchingStatistics stats = sGui->getIContextSimulator()->getCurrentMatchingStatistics(missingOnly);
        ui->tvp_MatchingStatistics->updateContainerMaybeAsync(stats);
    }
} // ns
