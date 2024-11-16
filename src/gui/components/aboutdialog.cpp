// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aboutdialog.h"

#include "ui_aboutdialog.h"

#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/fileutils.h"

namespace swift::gui::components
{
    CAboutDialog::CAboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CAboutDialog)
    {
        ui->setupUi(this);
        const QPointer<CAboutDialog> myself(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->init();

        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            myself->loadSwiftLicense();
        });

        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            myself->loadThirdPartyLicenses();
        });
    }

    CAboutDialog::~CAboutDialog()
    {}

    void CAboutDialog::init()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        ui->lbl_VersionValue->setText(sGui->getApplicationNameVersionDetailed());
        ui->pte_Info->setPlainText(sGui->getInfoString("\n"));
        ui->pte_Info->appendPlainText("\nSetup follows:\n----");
        ui->pte_Info->appendPlainText(sGui->getGlobalSetup().convertToQString("\n", true));
        ui->pte_Info->appendPlainText("\nScreen info follows:\n----");
        ui->pte_Info->appendPlainText(CGuiUtility::screenInformation("\n"));
    }

    void CAboutDialog::loadSwiftLicense()
    {
        const QString html = swift::misc::CFileUtils::readFileToString(swift::misc::CSwiftDirectories::legalDirectory() + "/LicenseRef-swift-pilot-client-1.html");
        ui->tbr_swiftLicense->setHtml(html);
    }

    void CAboutDialog::loadThirdPartyLicenses()
    {
        const QString html = swift::misc::CFileUtils::readFileToString(swift::misc::CSwiftDirectories::legalDirectory() + "/3rdparty.html");
        ui->tbr_ThirdPartyLicenses->setHtml(html);
    }
} // namespace swift::gui::components
