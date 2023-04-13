/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "interpolationcomponent.h"
#include "ui_interpolationcomponent.h"

#include "blackgui/guiapplication.h"
#include "blackgui/views/statusmessageview.h"
#include "blackcore/context/contextsimulator.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CInterpolationComponent::CInterpolationComponent(QWidget *parent) : QFrame(parent),
                                                                        ui(new Ui::CInterpolationComponent)
    {
        ui->setupUi(this);
        ui->tw_InterpolationSetup->setCurrentIndex(0);

        connect(ui->comp_InterpolationSetup, &CInterpolationSetupComponent::requestRenderingRestrictionsWidget, this, &CInterpolationComponent::requestRenderingRestrictionsWidget);
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validChangedCallsignEntered, this, &CInterpolationComponent::displayInterpolationMessages);
        connect(ui->pb_ReloadInterpolationMessages, &QPushButton::released, this, &CInterpolationComponent::displayInterpolationMessages);
    }

    CInterpolationComponent::~CInterpolationComponent()
    {}

    void CInterpolationComponent::displayInterpolationMessages()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        const CCallsign cs = ui->comp_CallsignCompleter->getCallsign();
        if (!cs.isValid()) { return; }

        const CStatusMessageList messages = sGui->getIContextSimulator()->getInterpolationMessages(cs);
        ui->tvp_InterpolationMessages->updateContainerMaybeAsync(messages);
    }
} // ns
