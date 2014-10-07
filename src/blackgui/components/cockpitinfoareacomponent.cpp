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
            if (this->statusBar())
            {
                this->statusBar()->hide();
            }
        }

        CCockpitInfoAreaComponent::~CCockpitInfoAreaComponent()
        { }

        void CCockpitInfoAreaComponent::toggleFloating()
        {
            CInfoArea::toggleFloating();
        }

        QSize CCockpitInfoAreaComponent::getPreferredSizeWhenFloating(int areaIndex) const
        {
            Q_UNUSED(areaIndex);
            return QSize(600, 400);
        }

        const QPixmap &CCockpitInfoAreaComponent::indexToPixmap(int areaIndex) const
        {
            Q_UNUSED(areaIndex);
            return CIcons::empty16();
        }

    } // namespace
} // namespace
