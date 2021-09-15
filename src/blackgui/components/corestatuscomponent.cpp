/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/corestatuscomponent.h"
#include "ui_corestatuscomponent.h"

class QWidget;

namespace BlackGui::Components
{
    CCoreStatusComponent::CCoreStatusComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::CCoreStatusComponent)
    {
        ui->setupUi(this);
    }

    CCoreStatusComponent::~CCoreStatusComponent() { }

} // namespacee
