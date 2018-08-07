/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsmatchingcomponent.h"
#include "ui_settingsmatchingcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/aircraftmatchersetup.h"
#include <QPointer>

using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CSettingsMatchingComponent::CSettingsMatchingComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsMatchingComponent)
        {
            ui->setupUi(this);
            connect(ui->pb_Save, &QPushButton::released, this, &CSettingsMatchingComponent::onSavePressed);
            connect(ui->pb_Reload, &QPushButton::released, this, &CSettingsMatchingComponent::onReloadPressed);
            this->deferredReload(5000);
        }

        CSettingsMatchingComponent::~CSettingsMatchingComponent()
        { }

        void CSettingsMatchingComponent::onSavePressed() const
        {
            IContextSimulator *simContext = simulatorContext();
            if (!simContext) { return; }
            const CAircraftMatcherSetup setup = ui->editor_MatchingForm->value();
            simContext->setMatchingSetup(setup);
        }

        void CSettingsMatchingComponent::onReloadPressed()
        {
            this->deferredReload(0);
        }

        void CSettingsMatchingComponent::deferredReload(int deferMs)
        {
            if (deferMs < 1)
            {
                IContextSimulator *simContext = simulatorContext();
                if (!simContext) { return; }
                const CAircraftMatcherSetup setup = simContext->getMatchingSetup();
                ui->editor_MatchingForm->setValue(setup);
            }
            else
            {
                QPointer<CSettingsMatchingComponent> myself(this);
                QTimer::singleShot(deferMs, this, [ = ]
                {
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
    } // ns
} // ns
