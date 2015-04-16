/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "invisibleinfoareacomponent.h"
#include "ui_invisibleinfoareacomponent.h"
#include "blackmisc/icons.h"

using namespace BlackGui;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {

        CInvisibleInfoAreaComponent::CInvisibleInfoAreaComponent(QWidget *parent) :
            CInfoArea(parent),
            ui(new Ui::CInvisibleInfoAreaComponent)
        {
            ui->setupUi(this);
            this->ui->comp_Navigator->allowStatusBar(false);
            this->initInfoArea();
        }

        CInvisibleInfoAreaComponent::~CInvisibleInfoAreaComponent()
        { }

        QSize CInvisibleInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaHorizontalNavigator:
                return QSize(400, 50);
            case InfoAreaVerticalNavigator:
                return QSize(50, 400);
            default:
                return QSize(0, 0);
            }
        }

        const QPixmap &CInvisibleInfoAreaComponent::indexToPixmap(int areaIndex) const
        {
            InfoArea area = static_cast<InfoArea>(areaIndex);
            switch (area)
            {
            case InfoAreaHorizontalNavigator:
            case InfoAreaVerticalNavigator:
            default:
                return CIcons::empty();
            }
        }

        CNavigatorDockWidget *CInvisibleInfoAreaComponent::getNavigatorComponent()
        {
            return this->ui->comp_Navigator;
        }

        void CInvisibleInfoAreaComponent::toggleNavigator()
        {
            this->ui->comp_Navigator->toggleFloating();
        }

    } // ns
} // ns
