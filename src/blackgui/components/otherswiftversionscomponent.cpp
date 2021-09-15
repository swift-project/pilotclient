/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "otherswiftversionscomponent.h"
#include "ui_otherswiftversionscomponent.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "guiapplication.h"
#include <QUrl>
#include <QDesktopServices>

using namespace BlackMisc;
using namespace BlackGui::Views;

namespace BlackGui::Components
{
    COtherSwiftVersionsComponent::COtherSwiftVersionsComponent(QWidget *parent) :
        QFrame(parent),
        ui(new Ui::COtherSwiftVersionsComponent)
    {
        ui->setupUi(this);

        ui->tvp_ApplicationInfo->menuRemoveItems(CApplicationInfoView::MenuClear);
        ui->tvp_ApplicationInfo->menuAddItems(CApplicationInfoView::MenuRefresh);
        ui->tvp_ApplicationInfo->otherSwiftVersionsFromDataDirectories();

        ui->le_ThisVersion->setText(sGui->getApplicationInfo().asOtherSwiftVersionString());
        ui->le_ThisVersion->home(false);

        connect(ui->tb_DataDir, &QToolButton::clicked, this, &COtherSwiftVersionsComponent::openDataDirectory);
        connect(ui->tvp_ApplicationInfo, &CApplicationInfoView::objectSelected, this, &COtherSwiftVersionsComponent::onObjectSelected);
        connect(ui->tvp_ApplicationInfo, &CApplicationInfoView::requestUpdate, this, &COtherSwiftVersionsComponent::reloadOtherVersions);
    }

    COtherSwiftVersionsComponent::~COtherSwiftVersionsComponent()
    { }

    bool COtherSwiftVersionsComponent::hasSelection() const
    {
        return (ui->tvp_ApplicationInfo->hasSelection());
    }

    CApplicationInfo COtherSwiftVersionsComponent::selectedOtherVersion() const
    {
        if (!this->hasSelection()) { return CApplicationInfo::null(); }
        return ui->tvp_ApplicationInfo->selectedObject();
    }

    void COtherSwiftVersionsComponent::reloadOtherVersionsDeferred(int deferMs)
    {
        if (deferMs <= 0)
        {
            ui->tvp_ApplicationInfo->otherSwiftVersionsFromDataDiretoriesAndResize(true);
        }
        else
        {
            QPointer<COtherSwiftVersionsComponent> myself(this);
            QTimer::singleShot(deferMs, this, [ = ]
            {
                if (myself) { myself->reloadOtherVersionsDeferred(-1); }
            });
        }
    }

    void COtherSwiftVersionsComponent::openDataDirectory()
    {
        const QString dir = CSwiftDirectories::applicationDataDirectory();
        const QUrl url = QUrl::fromLocalFile(dir);
        QDesktopServices::openUrl(url);
    }

    void COtherSwiftVersionsComponent::onObjectSelected(const CVariant &object)
    {
        if (!object.canConvert<CApplicationInfo>()) { return; }
        const CApplicationInfo info(object.value<CApplicationInfo>());
        emit this->versionChanged(info);
    }
} // ns
