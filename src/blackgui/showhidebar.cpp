/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/showhidebar.h"
#include "ui_showhidebar.h"

#include <QPushButton>

namespace BlackGui
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
