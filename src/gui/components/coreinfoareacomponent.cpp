// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/coreinfoareacomponent.h"

#include "ui_coreinfoareacomponent.h"

#include "gui/infoarea.h"
#include "misc/icons.h"

using namespace swift::misc;
using namespace swift::gui;

namespace swift::gui::components
{
    CCoreInfoAreaComponent::CCoreInfoAreaComponent(QWidget *parent)
        : CInfoArea(parent), ui(new Ui::CCoreInfoAreaComponent)
    {
        ui->setupUi(this);
        initInfoArea();
        toggleTabBarLocked(true);
    }

    CCoreInfoAreaComponent::~CCoreInfoAreaComponent() = default;

    CLogComponent *CCoreInfoAreaComponent::getLogComponent() { return ui->comp_Log; }

    CCoreStatusComponent *CCoreInfoAreaComponent::getStatusComponent() { return ui->comp_Status; }

    QSize CCoreInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
    {
        auto area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaLog: return QSize(400, 300);
        default: return QSize(400, 300);
        }
    }

    const QPixmap &CCoreInfoAreaComponent::indexToPixmap(int areaIndex) const
    {
        auto area = static_cast<InfoArea>(areaIndex);
        switch (area)
        {
        case InfoAreaLog: return CIcons::appLog16();
        default: return CIcons::statusBar16();
        }
    }
} // namespace swift::gui::components
