/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "logcomponent.h"
#include "ui_logcomponent.h"

namespace BlackGui
{
    namespace Components
    {

        CLogComponent::CLogComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CLogComponent)
        {
            ui->setupUi(this);
        }

        CLogComponent::~CLogComponent()
        {
            delete ui;
        }
    }
}
