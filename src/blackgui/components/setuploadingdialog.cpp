/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "setuploadingdialog.h"
#include "copyconfigurationdialog.h"
#include "ui_setuploadingdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/setupreader.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/network/urllist.h"

#include <QPushButton>
#include <QDesktopServices>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CSetupLoadingDialog::CSetupLoadingDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CSetupLoadingDialog)
        {
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Need sApp");
            if (this->hasSetupReader())
            {
                // reset if it was temporarily ignored
                sApp->getSetupReader()->setIgnoreCmdLineBootstrapUrl(false);
                connect(sApp, &CGuiApplication::setupHandlingCompleted, this, &CSetupLoadingDialog::onSetupHandlingCompleted);
            }

            ui->setupUi(this);
            connect(ui->pb_IgnoreExplicitBootstrapUrl, &QPushButton::clicked, this, &CSetupLoadingDialog::tryAgainWithoutBootstrapUrl);
            connect(ui->pb_LoadFromDisk, &QPushButton::clicked, this, &CSetupLoadingDialog::prefillSetupCache);
            connect(ui->pb_Help, &QPushButton::clicked, this, &CSetupLoadingDialog::openHelpPage);
            connect(ui->pb_CopyFromSwift, &QPushButton::clicked, this, &CSetupLoadingDialog::copyFromOtherSwiftVersions);
            connect(ui->pb_OpemDirectory, &QPushButton::clicked, this, &CSetupLoadingDialog::openDirectory);

            QPushButton *retry = ui->bb_Dialog->button(QDialogButtonBox::Retry);
            retry->setDefault(true);

            this->displaySetupCacheInfo();
            this->displayCmdBoostrapUrl();
            this->displayBootstrapUrls();
            this->displayGlobalSetup();
            this->displayOtherVersionsInfo();
        }
        CSetupLoadingDialog::CSetupLoadingDialog(const BlackMisc::CStatusMessageList &msgs, QWidget *parent) : CSetupLoadingDialog(parent)
        {
            ui->comp_Messages->appendStatusMessagesToList(msgs);
        }

        CSetupLoadingDialog::~CSetupLoadingDialog()
        { }

        bool CSetupLoadingDialog::hasCachedSetup() const
        {
            return this->hasSetupReader() && sApp->getSetupReader()->hasCachedSetup();
        }

        bool CSetupLoadingDialog::hasSetupReader() const
        {
            return sApp && sApp->hasSetupReader();
        }

        void CSetupLoadingDialog::displayBootstrapUrls()
        {
            const CUrlList bootstrapUrls = sApp->getGlobalSetup().getSwiftBootstrapFileUrls();
            for (const CUrl &url : bootstrapUrls)
            {
                const CStatusMessage msg = CNetworkUtils::canConnect(url) ?
                                           CStatusMessage(this).info("Can connect to '%1'") << url.getFullUrl() :
                                           CStatusMessage(this).warning("Cannot connect to '%1'") << url.getFullUrl();
                ui->comp_Messages->appendStatusMessageToList(msg);
            }
        }

        void CSetupLoadingDialog::displayCmdBoostrapUrl()
        {
            if (!sApp->hasSetupReader()) { return; }
            ui->le_CmdLine->setText(sApp->cmdLineArgumentsAsString());
            ui->le_BootstrapMode->setText(sApp->getSetupReader()->getBootstrapModeAsString());

            const QString bsUrl = sApp->getSetupReader()->getCmdLineBootstrapUrl();
            ui->pb_IgnoreExplicitBootstrapUrl->setVisible(!bsUrl.isEmpty());
            ui->le_BootstrapUrl->setText(bsUrl);
        }

        void CSetupLoadingDialog::displayGlobalSetup()
        {
            const QString gs = sApp->getGlobalSetup().convertToQString("\n", true);
            ui->comp_Messages->appendPlainTextToConsole(gs);
        }

        void CSetupLoadingDialog::openHelpPage()
        {
            const CUrl url = sApp->getGlobalSetup().getHelpPageUrl("bootstrap");
            if (url.isEmpty()) { return; }
            QDesktopServices::openUrl(url);
        }

        void CSetupLoadingDialog::tryAgainWithoutBootstrapUrl()
        {
            if (!sApp->hasSetupReader()) { return; }
            sApp->getSetupReader()->setIgnoreCmdLineBootstrapUrl(true);
            this->accept();
        }

        void CSetupLoadingDialog::prefillSetupCache()
        {
            if (!sApp || sApp->isShuttingDown()) { return; }
            if (!this->hasSetupReader()) { return; }
            sApp->getSetupReader()->prefillCacheWithLocalResourceBootstrapFile();
            this->displaySetupCacheInfo();
        }

        void CSetupLoadingDialog::displaySetupCacheInfo()
        {
            if (this->hasSetupReader())
            {
                // reset if it was temporarily ignored
                const CSetupReader *sr = sApp->getSetupReader();
                const QDateTime setupTs = sr->getSetupCacheTimestamp();
                ui->le_SetupCache->setText(setupTs.isValid() ?
                                           setupTs.toString(Qt::ISODateWithMs) :
                                           "No cache timestamp");
            }
            else
            {
                ui->le_SetupCache->setText("No setup reader");
            }

            const bool hasCachedSetup = this->hasCachedSetup();
            ui->pb_LoadFromDisk->setEnabled(!hasCachedSetup);
            ui->pb_LoadFromDisk->setToolTip(hasCachedSetup ? "Cached setup already available" : "No cached setup");
        }

        void CSetupLoadingDialog::displayOtherVersionsInfo()
        {
            const int other = CDirectoryUtils::applicationDataDirectoriesCount() - 1 ;
            ui->le_OtherSwiftVersions->setText(QString("There is/are %1 other swift version(s) installed").arg(other));
            ui->pb_CopyFromSwift->setEnabled(other > 0);
        }

        void CSetupLoadingDialog::openDirectory()
        {
            const QUrl url = QUrl::fromLocalFile(CDirectoryUtils::normalizedApplicationDataDirectory());
            QDesktopServices::openUrl(url);
        }

        void CSetupLoadingDialog::copyFromOtherSwiftVersions()
        {
            if (!m_copyFromOtherSwiftVersion)
            {
                CCopyConfigurationDialog *d = new CCopyConfigurationDialog(this);
                d->setWithBootstrapFile(true);
                d->setModal(true);
                d->setCacheMode();
                m_copyFromOtherSwiftVersion.reset(d);
            }

            const int r = m_copyFromOtherSwiftVersion->exec();
            Q_UNUSED(r);
        }

        void CSetupLoadingDialog::onSetupHandlingCompleted(bool success)
        {
            Q_UNUSED(success);
            this->displaySetupCacheInfo();
        }
    } // ns
} // ns
