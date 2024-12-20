// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "interpolationcomponent.h"

#include "ui_interpolationcomponent.h"

#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/views/statusmessageview.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CInterpolationComponent::CInterpolationComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CInterpolationComponent)
    {
        ui->setupUi(this);
        ui->tw_InterpolationSetup->setCurrentIndex(0);

        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validChangedCallsignEntered, this,
                &CInterpolationComponent::displayInterpolationMessages);
        connect(ui->pb_ReloadInterpolationMessages, &QPushButton::released, this,
                &CInterpolationComponent::displayInterpolationMessages);
    }

    CInterpolationComponent::~CInterpolationComponent() {}

    void CInterpolationComponent::displayInterpolationMessages()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        const CCallsign cs = ui->comp_CallsignCompleter->getCallsign();
        if (!cs.isValid()) { return; }

        const CStatusMessageList messages = sGui->getIContextSimulator()->getInterpolationMessages(cs);
        ui->tvp_InterpolationMessages->updateContainerMaybeAsync(messages);
    }
} // namespace swift::gui::components
