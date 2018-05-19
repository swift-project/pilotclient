/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "otherswiftversionscomponent.h"
#include "ui_otherswiftversionscomponent.h"
#include "blackmisc/directoryutils.h"
#include "guiapplication.h"
#include <QUrl>
#include <QDesktopServices>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        COtherSwiftVersionsComponent::COtherSwiftVersionsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::COtherSwiftVersionsComponent)
        {
            ui->setupUi(this);
            connect(ui->tb_DataDir, &QToolButton::clicked, this, &COtherSwiftVersionsComponent::openDataDirectory);
            ui->tvp_ApplicationInfo->otherSwiftVersionsFromDataDirectories();
            ui->le_ThisVersion->setText(sGui->getApplicationInfo().asOtherSwiftVersionString());
            ui->le_ThisVersion->home(false);
        }

        COtherSwiftVersionsComponent::~COtherSwiftVersionsComponent()
        { }

        bool COtherSwiftVersionsComponent::hasSelection() const
        {
            return (ui->tvp_ApplicationInfo->hasSelection());
        }

        BlackMisc::CApplicationInfo COtherSwiftVersionsComponent::selectedOtherVersion() const
        {
            if (!this->hasSelection()) { return CApplicationInfo::null(); }
            return ui->tvp_ApplicationInfo->selectedObject();
        }

        void COtherSwiftVersionsComponent::openDataDirectory()
        {
            const QString dir = CDirectoryUtils::applicationDataDirectory();
            const QUrl url = QUrl::fromLocalFile(dir);
            QDesktopServices::openUrl(url);
        }
    } // ns
} // ns
