/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextapplication.h"
#include "blackgui/components/registercomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/identifierview.h"
#include "ui_registercomponent.h"

#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CRegisterComponent::CRegisterComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CRegisterComponent)
        {
            ui->setupUi(this);
            connect(sGui->getIContextApplication(), &IContextApplication::registrationChanged, this, &CRegisterComponent::ps_update);

            // timer is there just in case something goes wrong
            connect(&m_updateTimer, &QTimer::timeout, this, &CRegisterComponent::ps_update);
            m_updateTimer.setInterval(30 * 1000);
            m_updateTimer.start();
        }

        CRegisterComponent::~CRegisterComponent()
        { }

        void CRegisterComponent::ps_update()
        {
            // if not supported, do nothing
            if (!sGui || !sGui->supportsContexts()) { return; }
            ui->tvp_RegisteredComponents->updateContainer(sGui->getIContextApplication()->getRegisteredApplications());
        }
    } // ns
} // ns
