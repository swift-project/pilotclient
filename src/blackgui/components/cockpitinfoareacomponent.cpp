/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/cockpitinfoareacomponent.h"
#include "blackgui/infoarea.h"
#include "blackmisc/icons.h"
#include "ui_cockpitinfoareacomponent.h"

#include <QTabWidget>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui::Components
{
    CCockpitInfoAreaComponent::CCockpitInfoAreaComponent(QWidget *parent) :
        CInfoArea(parent),
        ui(new Ui::CCockpitInfoAreaComponent)
    {
        ui->setupUi(this);
        this->initInfoArea();
        this->setTabBarPosition(QTabWidget::North);
        this->toggleTabBarLocked(true);
    }

    CCockpitInfoAreaComponent::~CCockpitInfoAreaComponent()
    { }

    QSize CCockpitInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        // see also CMainInfoAreaComponent::getPreferredSizeWhenFloating
        Q_UNUSED(areaIndex)
        return QSize(600, 400);
    }

    const QPixmap &CCockpitInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        const InfoArea area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaAudio:         return CIcons::appAudio16();
        case InfoAreaNotifications: return CIcons::appVoiceRooms16();
        case InfoAreaAdvanced:      return CIcons::appSettings16();
        default:                    return CIcons::empty();
        }
    }

} // namespace
