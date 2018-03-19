/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationcomponent.h"
#include "ui_interpolationcomponent.h"

namespace BlackGui
{
    namespace Components
    {
        CInterpolationComponent::CInterpolationComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CInterpolationComponent)
        {
            ui->setupUi(this);
            ui->tw_InterpolationSetup->setCurrentIndex(0);

            connect(ui->comp_InterpolationSetup, &CInterpolationSetupComponent::requestRenderingRestrictionsWidget, this, &CInterpolationComponent::requestRenderingRestrictionsWidget);
        }

        CInterpolationComponent::~CInterpolationComponent()
        { }
    } // ns
} // ns
