// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/registercomponent.h"

#include <QtGlobal>

#include "ui_registercomponent.h"

#include "core/context/contextapplication.h"
#include "gui/guiapplication.h"
#include "gui/views/identifierview.h"

using namespace swift::core;
using namespace swift::core::context;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CRegisterComponent::CRegisterComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CRegisterComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        Q_ASSERT_X(sGui->getIContextApplication(), Q_FUNC_INFO, "Need application context");

        ui->setupUi(this);
        ui->tvp_RegisteredComponents->menuAddItems(CIdentifierView::MenuRefresh);

        connect(&m_updateTimer, &QTimer::timeout, this, &CRegisterComponent::update);
        connect(sGui->getIContextApplication(), &IContextApplication::registrationChanged, this,
                &CRegisterComponent::update);
        connect(ui->tvp_RegisteredComponents, &CIdentifierView::requestUpdate, this, &CRegisterComponent::update);

        // timer is there just in case something goes wrong
        m_updateTimer.setInterval(30 * 1000);
        m_updateTimer.start();
    }

    CRegisterComponent::~CRegisterComponent() {}

    void CRegisterComponent::update()
    {
        // if not supported, do nothing
        if (!sGui || !sGui->supportsContexts() || sGui->isShuttingDown()) { return; }
        ui->tvp_RegisteredComponents->updateContainer(sGui->getIContextApplication()->getRegisteredApplications());
    }
} // namespace swift::gui::components
