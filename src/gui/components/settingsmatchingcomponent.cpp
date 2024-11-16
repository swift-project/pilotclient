// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsmatchingcomponent.h"

#include <QPointer>

#include "ui_settingsmatchingcomponent.h"

#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "misc/logmessage.h"
#include "misc/simulation/aircraftmatchersetup.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core::context;

namespace swift::gui::components
{
    CSettingsMatchingComponent::CSettingsMatchingComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CSettingsMatchingComponent)
    {
        ui->setupUi(this);
        connect(ui->pb_Save, &QPushButton::released, this, &CSettingsMatchingComponent::onSavePressed);
        connect(ui->pb_Reload, &QPushButton::released, this, &CSettingsMatchingComponent::onReloadPressed);
        connect(ui->pb_MatchingAgain, &QPushButton::released, this,
                &CSettingsMatchingComponent::onMatchingsAgainPressed);

        // also used in mapping tool, must also work without contexts
        IContextSimulator *simContext = simulatorContext();
        if (simContext)
        {
            connect(simContext, &IContextSimulator::matchingSetupChanged, this,
                    &CSettingsMatchingComponent::onSetupChanged, Qt::QueuedConnection);
            this->deferredReload(5000);
        }
        else { this->showButtons(false); }
    }

    CSettingsMatchingComponent::~CSettingsMatchingComponent() {}

    CAircraftMatcherSetup CSettingsMatchingComponent::getMatchingSetup() const { return ui->form_Matching->value(); }

    void CSettingsMatchingComponent::setMatchingSetup(const CAircraftMatcherSetup &setup)
    {
        ui->form_Matching->setValue(setup);
    }

    void CSettingsMatchingComponent::showButtons(bool show) { ui->fr_Buttons->setVisible(show); }

    void CSettingsMatchingComponent::onSavePressed() const
    {
        IContextSimulator *simContext = simulatorContext();
        if (!simContext) { return; }
        const CAircraftMatcherSetup setup = ui->form_Matching->value();
        simContext->setMatchingSetup(setup);
    }

    void CSettingsMatchingComponent::onReloadPressed() { this->deferredReload(0); }

    void CSettingsMatchingComponent::onMatchingsAgainPressed()
    {
        if (!sGui || !sGui->getISimulator() || !sGui->getISimulator()->isConnected()) { return; }
        const int reMatchedNo = sGui->getIContextSimulator()->doMatchingsAgain();
        CLogMessage(this).info(u"Triggered re-apping of %1 aircraft") << reMatchedNo;
    }

    void CSettingsMatchingComponent::onSetupChanged()
    {
        const IContextSimulator *simContext = simulatorContext();
        if (!simContext) { return; }
        this->deferredReload(0);
    }

    void CSettingsMatchingComponent::deferredReload(int deferMs)
    {
        IContextSimulator *simContext = simulatorContext();
        if (!simContext) { return; }
        if (deferMs < 1)
        {
            const CAircraftMatcherSetup setup = simContext->getMatchingSetup();
            ui->form_Matching->setValue(setup);
        }
        else
        {
            QPointer<CSettingsMatchingComponent> myself(this);
            QTimer::singleShot(deferMs, this, [=] {
                if (!myself) { return; }
                this->deferredReload(0);
            });
        }
    }

    IContextSimulator *CSettingsMatchingComponent::simulatorContext()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return nullptr; }
        return sGui->getIContextSimulator();
    }
} // namespace swift::gui::components
