/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dbstashcomponent.h"
#include "ui_dbstashcomponent.h"

namespace BlackGui
{
    namespace Components
    {
        CDbStashComponent::CDbStashComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbStashComponent)
        {
            ui->setupUi(this);
        }

        CDbStashComponent::~CDbStashComponent()
        { }

        void CDbStashComponent::setProvider(BlackMisc::Network::IWebDataServicesProvider *provider)
        {
            CWebDataServicesAware::setProvider(provider);
        }

        void CDbStashComponent::gracefulShutdown()
        {

        }

    } // ns
} // ns
