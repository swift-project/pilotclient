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
#include "blackcore/context_application.h"

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
        }

        CRegisterComponent::~CRegisterComponent()
        { }

        void CRegisterComponent::ps_update()
        {
            this->ui->tvp_RegisteredComponents->updateContainer(getIContextApplication()->getRegisteredApplications());
        }

        void CRegisterComponent::runtimeHasBeenSet()
        {
            Q_ASSERT_X(getIContextApplication(), Q_FUNC_INFO, "Missing context");
            m_updateTimer->setUpdateIntervalSeconds(20);
            QObject::connect(getIContextApplication(), &IContextApplication::registrationChanged, this, &CRegisterComponent::ps_update);
        }

    } // ns
} // ns
