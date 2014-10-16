/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cockpitinfoareacomponent.h"
#include "ui_cockpitinfoareacomponent.h"
#include "blackmisc/icons.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {

        CCockpitInfoAreaComponent::CCockpitInfoAreaComponent(QWidget *parent) :
            CInfoArea(parent),
            ui(new Ui::CCockpitInfoAreaComponent)
        {
            ui->setupUi(this);
            initInfoArea();
            this->ps_setTabBarPosition(QTabWidget::North);
        }

        CCockpitInfoAreaComponent::~CCockpitInfoAreaComponent()
        { }

        QSize CCockpitInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
        {
            Q_UNUSED(areaIndex);
            return QSize(600, 400);
        }

        const QPixmap &CCockpitInfoAreaComponent::indexToPixmap(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaAudio:
                return CIcons::appAudio16();
            case InfoAreaVoiceRooms:
                return CIcons::appVoiceRooms16();
            default:
                return CIcons::empty();
            }
        }

    } // namespace
} // namespace
