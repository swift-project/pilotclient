/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
