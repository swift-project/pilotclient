// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/showhidebar.h"
#include "ui_showhidebar.h"

#include <QPushButton>

namespace swift::gui
{

    CShowHideBar::CShowHideBar(QWidget *parent) : QFrame(parent),
                                                  ui(new Ui::CShowHideBar)
    {
        ui->setupUi(this);
        connect(ui->pb_ShowHidePushButton, &QPushButton::clicked, this, &CShowHideBar::toggleShowHide);
    }

    CShowHideBar::~CShowHideBar()
    {}

    bool CShowHideBar::isShown() const
    {
        return ui->pb_ShowHidePushButton->isChecked();
    }
} // namespacee
