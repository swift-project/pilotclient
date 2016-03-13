/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "registercomponent.h"
#include "ui_registercomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextapplication.h"

using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CRegisterComponent::CRegisterComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CRegisterComponent),
            m_updateTimer(new CUpdateTimer("CRegisterComponent", &CRegisterComponent::ps_update, this))
        {
            ui->setupUi(this);
            m_updateTimer->setUpdateIntervalSeconds(20);
            connect(sGui->getIContextApplication(), &IContextApplication::registrationChanged, this, &CRegisterComponent::ps_update);
        }

        CRegisterComponent::~CRegisterComponent()
        { }

        void CRegisterComponent::ps_update()
        {
            if (!sGui) { return; }
            Q_ASSERT_X(sGui->supportsContexts(), Q_FUNC_INFO, "Application does not support contexts");
            this->ui->tvp_RegisteredComponents->updateContainer(sGui->getIContextApplication()->getRegisteredApplications());
        }
    } // ns
} // ns
