// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "initialdataloadcomponent.h"
#include "ui_initialdataloadcomponent.h"

namespace BlackGui::Components
{
    CInitialDataLoadComponent::CInitialDataLoadComponent(QWidget *parent) : QFrame(parent),
                                                                            ui(new Ui::CInitialDataLoadComponent)
    {
        ui->setupUi(this);
        ui->comp_DataUpdates->showVisibleLoadAllButtons(true, false, true);
    }

    CInitialDataLoadComponent::~CInitialDataLoadComponent()
    {}

    void CInitialDataLoadComponent::loadAllFromShared()
    {
        ui->comp_DataUpdates->loadAllFromShared();
    }

    void CInitialDataLoadComponent::loadAllFromDb()
    {
        ui->comp_DataUpdates->loadAllFromDb();
    }

    bool CInitialDataLoadComponent::isLoadInProgress() const
    {
        return ui->comp_DataUpdates->isLoadInProgress();
    }

    void CInitialDataLoadWizardPage::initializePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        QTimer::singleShot(500, m_config, [=] { m_config->loadAllFromShared(); });
    }

    bool CInitialDataLoadWizardPage::validatePage()
    {
        Q_ASSERT_X(m_config, Q_FUNC_INFO, "Missing config");
        return !m_config->isLoadInProgress();
    }
} // ns
