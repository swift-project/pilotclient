// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/coreinfoareacomponent.h"
#include "blackgui/infoarea.h"
#include "blackmisc/icons.h"
#include "ui_coreinfoareacomponent.h"

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui::Components
{
    CCoreInfoAreaComponent::CCoreInfoAreaComponent(QWidget *parent) : CInfoArea(parent),
                                                                      ui(new Ui::CCoreInfoAreaComponent)
    {
        ui->setupUi(this);
        initInfoArea();
        toggleTabBarLocked(true);
    }

    CCoreInfoAreaComponent::~CCoreInfoAreaComponent()
    {}

    CLogComponent *CCoreInfoAreaComponent::getLogComponent()
    {
        return ui->comp_Log;
    }

    CCoreStatusComponent *CCoreInfoAreaComponent::getStatusComponent()
    {
        return ui->comp_Status;
    }

    QSize CCoreInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaLog:
            return QSize(400, 300);
        default:
            return QSize(400, 300);
        }
    }

    const QPixmap &CCoreInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaLog:
            return CIcons::appLog16();
        default:
            return CIcons::statusBar16();
        }
    }
} // namespace
