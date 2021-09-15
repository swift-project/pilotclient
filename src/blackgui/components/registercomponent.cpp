/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackgui/components/registercomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/identifierview.h"
#include "ui_registercomponent.h"

#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    CRegisterComponent::CRegisterComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CRegisterComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        Q_ASSERT_X(sGui->getIContextApplication(), Q_FUNC_INFO, "Need application context");

        ui->setupUi(this);
        ui->tvp_RegisteredComponents->menuAddItems(CIdentifierView::MenuRefresh);

        connect(&m_updateTimer, &QTimer::timeout, this, &CRegisterComponent::update);
        connect(sGui->getIContextApplication(), &IContextApplication::registrationChanged, this, &CRegisterComponent::update);
        connect(ui->tvp_RegisteredComponents, &CIdentifierView::requestUpdate, this, &CRegisterComponent::update);

        // timer is there just in case something goes wrong
        m_updateTimer.setInterval(30 * 1000);
        m_updateTimer.start();
    }

    CRegisterComponent::~CRegisterComponent()
    { }

    void CRegisterComponent::update()
    {
        // if not supported, do nothing
        if (!sGui || !sGui->supportsContexts() || sGui->isShuttingDown()) { return; }
        ui->tvp_RegisteredComponents->updateContainer(sGui->getIContextApplication()->getRegisteredApplications());
    }
} // ns
