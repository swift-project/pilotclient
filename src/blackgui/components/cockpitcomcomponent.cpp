/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cockpitcomcomponent.h"
#include "ui_cockpitcomcomponent.h"
#include "../stylesheetutility.h"

using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {
        CCockpitComComponent::CCockpitComComponent(QWidget *parent) :
            QFrame(parent),
            CEnableForDockWidgetInfoArea(this),
            ui(new Ui::CCockpitMainComponent)
        {
            ui->setupUi(this);
        }

        CCockpitComComponent::~CCockpitComComponent()
        { }

        void CCockpitComComponent::paintEvent(QPaintEvent *event)
        {
            Q_UNUSED(event);
            CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
        }

    } // namespace
} // namespace
