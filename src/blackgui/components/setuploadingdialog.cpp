/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "setuploadingdialog.h"
#include "copymodelsfromotherswiftversionsdialog.h"
#include "ui_setuploadingdialog.h"
#include "blackgui/guiapplication.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/setupreader.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/network/urllist.h"

#include <QPushButton>
#include <QDesktopServices>
#include <QTimer>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore;
using namespace BlackCore::Data;

namespace BlackGui::Components
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
        connect(ui->pb_TryToFix, &QPushButton::clicked, this, &CSetupLoadingDialog::tryToFix);

        QPushButton *retry = ui->bb_Dialog->button(QDialogButtonBox::Retry);
        retry->setDefault(true);

        this->displaySetupCacheInfo();
        this->displayCmdBoostrapUrl();
        this->displayBootstrapUrls();
        this->displayGlobalSetup();
        this->displayOtherVersionsInfo();

        ui->comp_Messages->hideFilterBar(); // saves space, we only expect aview messages
    }
    CSetupLoadingDialog::CSetupLoadingDialog(const CStatusMessageList &msgs, QWidget *parent) : CSetupLoadingDialog(parent)
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
        if (!sApp || sApp->isShuttingDown()) { return; }
        const CGlobalSetup setup = sApp->getGlobalSetup();
        if (setup.wasLoadedFromWeb())
        {
            QPointer<CSetupLoadingDialog> myself(this);
            QTimer::singleShot(250, this, [ = ]
            {
                if (!myself) { return; }
                const CUrlList bootstrapUrls = setup.getSwiftBootstrapFileUrls();
                for (const CUrl &url : bootstrapUrls)
                {
                    const bool cc = CNetworkUtils::canConnect(url);
                    if (!myself) { return; } // because of CEventLoop::processEventsUntil
                    const CStatusMessage msg = cc ?
                                                CStatusMessage(this).info(u"Can connect to '%1'") << url.getFullUrl() :
                                                CStatusMessage(this).warning(u"Cannot connect to '%1'") << url.getFullUrl();
                    ui->comp_Messages->appendStatusMessageToList(msg);
                }
            });
        }
        else
        {
            const CStatusMessage msg = CStatusMessage(this).warning(u"No loaded bootstrap setup available, skipping URL test");
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
        Q_UNUSED(gs)
        //ui->comp_Messages->appendPlainTextToConsole(gs);
        //! \fixme create plain text console for this (used to be part of the log component, changed by issue T36)
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

    void CSetupLoadingDialog::tryToFix()
    {
        this->prefillSetupCache();
        QPushButton *retry = ui->bb_Dialog->button(QDialogButtonBox::Retry);
        if (!retry) { return; }

        QPointer<CSetupLoadingDialog> myself(this);
        QTimer::singleShot(2000, this, [ = ]
        {
            if (!sApp || !myself) { return; }
            retry->click();
        });
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
            static const QDateTime zeroTime = QDateTime::fromMSecsSinceEpoch(0);
            ui->le_SetupCache->setText(setupTs.isValid() && setupTs > zeroTime ?
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
        ui->pb_TryToFix->setEnabled(!hasCachedSetup);
    }

    void CSetupLoadingDialog::displayOtherVersionsInfo()
    {
        const int other = CSwiftDirectories::applicationDataDirectoriesCount() - 1 ;
        ui->le_OtherSwiftVersions->setText(QStringLiteral("There is/are %1 other swift version(s) installed").arg(other));
        ui->pb_CopyFromSwift->setEnabled(other > 0);
    }

    void CSetupLoadingDialog::openDirectory()
    {
        const QUrl url = QUrl::fromLocalFile(CSwiftDirectories::normalizedApplicationDataDirectory());
        QDesktopServices::openUrl(url);
    }

    void CSetupLoadingDialog::copyFromOtherSwiftVersions()
    {
        if (!m_copyFromOtherSwiftVersion)
        {
            CCopyModelsFromOtherSwiftVersionsDialog *d = new CCopyModelsFromOtherSwiftVersionsDialog(this);
            d->setModal(true);
            m_copyFromOtherSwiftVersion.reset(d);
        }

        const int r = m_copyFromOtherSwiftVersion->exec();
        Q_UNUSED(r);
        this->displaySetupCacheInfo();
    }

    void CSetupLoadingDialog::onSetupHandlingCompleted(bool success)
    {
        Q_UNUSED(success);
        this->displaySetupCacheInfo();
    }
} // ns
