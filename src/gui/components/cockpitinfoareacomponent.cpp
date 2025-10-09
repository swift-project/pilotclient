// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/cockpitinfoareacomponent.h"

#include <QTabWidget>
#include <QtGlobal>

#include "ui_cockpitinfoareacomponent.h"

#include "gui/infoarea.h"
#include "misc/icons.h"

using namespace swift::misc;

namespace swift::gui::components
{
    CCockpitInfoAreaComponent::CCockpitInfoAreaComponent(QWidget *parent)
        : CInfoArea(parent), ui(new Ui::CCockpitInfoAreaComponent)
    {
        ui->setupUi(this);
        this->initInfoArea();
        this->setTabBarPosition(QTabWidget::North);
        this->toggleTabBarLocked(true);
    }

    CCockpitInfoAreaComponent::~CCockpitInfoAreaComponent() {}

    QSize CCockpitInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        // see also CMainInfoAreaComponent::getPreferredSizeWhenFloating
        Q_UNUSED(areaIndex)
        return QSize(600, 400);
    }

    const QPixmap &CCockpitInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        const auto area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaAudio: return CIcons::appAudio16();
        case InfoAreaNotifications: return CIcons::appVoiceRooms16();
        case InfoAreaAdvanced: return CIcons::appSettings16();
        default: return CIcons::empty();
        }
    }

} // namespace swift::gui::components
