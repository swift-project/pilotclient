// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "matchingstatisticscomponent.h"
#include "ui_matchingstatisticscomponent.h"
#include "blackgui/guiapplication.h"
#include "core/context/contextsimulator.h"

using namespace swift::misc::simulation;
using namespace swift::core::context;
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
