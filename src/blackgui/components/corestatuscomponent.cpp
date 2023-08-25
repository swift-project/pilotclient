// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/corestatuscomponent.h"
#include "ui_corestatuscomponent.h"

class QWidget;

namespace BlackGui::Components
{
    CCoreStatusComponent::CCoreStatusComponent(QWidget *parent) : QFrame(parent),
                                                                  ui(new Ui::CCoreStatusComponent)
    {
        ui->setupUi(this);
    }

    CCoreStatusComponent::~CCoreStatusComponent() {}

} // namespacee
