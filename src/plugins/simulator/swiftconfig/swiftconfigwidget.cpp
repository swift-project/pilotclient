/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftconfigwidget.h"
#include "ui_swiftconfigwidget.h"

using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace Swift
    {
        CSwiftConfigWidget::CSwiftConfigWidget(QWidget *parent) :
            BlackGui::CPluginConfigWindow(parent),
            ui(new Ui::CSwiftConfigWidget)
        {
            ui->setupUi(this);
        }

        CSwiftConfigWidget::~CSwiftConfigWidget()
        { }
    } // ns
} // ns
