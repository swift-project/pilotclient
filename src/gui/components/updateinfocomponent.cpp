// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/updateinfocomponent.h"

#include <QDesktopServices>
#include <QMessageBox>

#include "ui_updateinfocomponent.h"

#include "config/buildconfig.h"
#include "gui/components/downloaddialog.h"
#include "gui/components/installxswiftbusdialog.h"
#include "gui/guiapplication.h"
#include "misc/db/distributionlist.h"
#include "misc/logmessage.h"
#include "misc/network/networkutils.h"
#include "misc/stringutils.h"

using namespace swift::config;
using namespace swift::core::application;
using namespace swift::misc;
using namespace swift::misc::db;
using namespace swift::misc::network;

namespace swift::gui::components
{
    CUpdateInfoComponent::CUpdateInfoComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CUpdateInfoComponent)
    {
        ui->setupUi(this);
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

        connect(sGui, &CGuiApplication::updateInfoAvailable, this, &CUpdateInfoComponent::changedUpdateInfo,
                Qt::QueuedConnection);
        connect(ui->pb_CheckForUpdates, &QPushButton::pressed, this, &CUpdateInfoComponent::requestLoadOfSetup,
                Qt::QueuedConnection);
        connect(ui->pb_DownloadXSwiftBus, &QPushButton::pressed, this, &CUpdateInfoComponent::downloadXSwiftBusDialog,
                Qt::QueuedConnection);
        connect(ui->pb_DownloadInstaller, &QPushButton::pressed, this, &CUpdateInfoComponent::downloadInstallerDialog,
                Qt::QueuedConnection);

        // use version signal as trigger for completion
        if (!m_updateInfo.get().isEmpty()) { this->changedUpdateInfo(); }
    }

    CUpdateInfoComponent::~CUpdateInfoComponent() = default;

    CArtifact CUpdateInfoComponent::getLatestAvailablePilotClientArtifactForSelection() const
    {
        const CUpdateInfo info = m_updateInfo.get();
        const CPlatform p = this->getSelectedOrDefaultPlatform();
        const CDistribution d = this->getSelectedOrDefaultDistribution();
        const CArtifact a =
            info.getArtifactsPilotClient().findByDistributionAndPlatform(d, p, true).getLatestArtifactOrDefault();
        return a;
    }

    bool CUpdateInfoComponent::isNewPilotClientVersionAvailable() const
    {
        const QStringList settings = m_updateSettings.get();
        Q_ASSERT_X(settings.size() == 2, Q_FUNC_INFO, "wrong setting");
        const QString channel = settings.front();
        if (channel.isEmpty()) { return false; }

        const CUpdateInfo info = m_updateInfo.get();
        const CDistribution d = info.getDistributions().findFirstByChannelOrDefault(channel);
        const QVersionNumber vCurrentChannelPlatform =
            info.getArtifactsPilotClient().findByDistribution(d).getLatestQVersion();
        const QVersionNumber vCurrent = CBuildConfig::getVersion();
        if (vCurrentChannelPlatform.isNull()) { return false; }
        return (vCurrentChannelPlatform > vCurrent);
    }

    void CUpdateInfoComponent::triggerDownload()
    {
        QPointer<CUpdateInfoComponent> myself(this);
        QTimer::singleShot(10, this, [=] {
            if (!myself) { return; }
            ui->pb_DownloadInstaller->click();
        });
    }

    void CUpdateInfoComponent::requestLoadOfSetup()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        sGui->reloadUpdateInfo();
    }

    void CUpdateInfoComponent::changedUpdateInfo()
    {
        ui->lbl_CurrentVersionDisplay->setText(CBuildConfig::getVersionString());

        const CUpdateInfo updateInfo(m_updateInfo.get());
        const QStringList settings = m_updateSettings.get();
        Q_ASSERT_X(settings.size() == 2, Q_FUNC_INFO, "Settings");

        const CPlatformSet platforms = updateInfo.getArtifactsPilotClient().getPlatforms();
        CDistributionList distributions = updateInfo.getArtifactsPilotClient().getDistributions();
        distributions.sortByStability(Qt::DescendingOrder);

        int i = 0;
        ui->cb_Platforms->disconnect();
        ui->cb_Platforms->clear();
        for (const CPlatform &platform : platforms)
        {
            ui->cb_Platforms->insertItem(i++, CIcon(platform.toIcon()).toPixmap(), platform.getPlatformName());
        }
        if (platforms.contains(settings.last())) { ui->cb_Platforms->setCurrentText(settings.last()); }
        else if (platforms.contains(CPlatform::currentPlatform().getPlatformName()))
        {
            ui->cb_Platforms->setCurrentText(CPlatform::currentPlatform().getPlatformName());
        }

        i = 0;
        ui->cb_Channels->disconnect();
        ui->cb_Channels->clear();
        for (const CDistribution &distribution : distributions)
        {
            ui->cb_Channels->insertItem(i++, CIcon(distribution.getRestrictionIcon()).toPixmap(),
                                        distribution.getChannel());
        }
        if (distributions.containsChannel(settings.front())) { ui->cb_Channels->setCurrentText(settings.front()); }

        this->uiSelectionChanged();
        connect(ui->cb_Platforms, &QComboBox::currentTextChanged, this, &CUpdateInfoComponent::platformChanged,
                Qt::QueuedConnection);
        connect(ui->cb_Channels, &QComboBox::currentTextChanged, this, &CUpdateInfoComponent::channelChanged,
                Qt::QueuedConnection);

        // emit via digest signal
        m_dsDistributionAvailable.inputSignal();
    }

    void CUpdateInfoComponent::downloadXSwiftBusDialog()
    {
        const QString currentXsb = ui->cb_ArtifactsXsb->currentText();
        const QString currentSwift = ui->cb_ArtifactsPilotClient->currentText();

        if (!stringCompare(currentXsb, currentSwift, Qt::CaseInsensitive))
        {
            const QString msg = QStringLiteral("xswiftbus '%1' does NOT match swift version, download anyway?")
                                    .arg(currentXsb, currentSwift);
            const QMessageBox::StandardButton reply = QMessageBox::question(this, QStringLiteral("Download xswiftbus"),
                                                                            msg, QMessageBox::Yes | QMessageBox::No);
            if (reply != QMessageBox::Yes) { return; }
        }

        if (!m_installXSwiftBusDialog)
        {
            m_installXSwiftBusDialog.reset(new CInstallXSwiftBusDialog(this));
            m_installXSwiftBusDialog->setModal(true);
        }

        m_installXSwiftBusDialog->setDefaultDownloadName(currentXsb);
        m_installXSwiftBusDialog->show();
    }

    void CUpdateInfoComponent::downloadInstallerDialog()
    {
        const CUpdateInfo update(m_updateInfo.get());
        const QString currentVersion = ui->cb_ArtifactsPilotClient->currentText();
        const QString platform = ui->cb_Platforms->currentText();
        if (!CPlatform::isCurrentPlatform(platform))
        {
            const QMessageBox::StandardButton ret =
                QMessageBox::warning(this, "Download installer",
                                     QStringLiteral("The platform '%1' does not match your current platform '%2'.\n"
                                                    "Do you want to continue?")
                                         .arg(platform, CPlatform::currentPlatform().getPlatformName()),
                                     QMessageBox::Yes | QMessageBox::No);
            if (ret != QMessageBox::Yes) { return; }
        }

        // find artifcat
        const CArtifact artifact =
            update.getArtifactsPilotClient().findByMatchingPlatform(platform).findFirstByVersionOrDefault(
                currentVersion);

        if (!m_downloadDialog)
        {
            m_downloadDialog.reset(new CDownloadDialog(this));
            m_downloadDialog->setModal(true);
        }

        const CRemoteFile rf = artifact.asRemoteFile();
        if (rf.getUrl().isHavingHtmlSuffix()) { QDesktopServices::openUrl(rf.getUrl()); }
        else
        {
            m_downloadDialog->setMode(CDownloadComponent::SwiftInstaller);
            m_downloadDialog->setDownloadFile(artifact.asRemoteFile());
            m_downloadDialog->showAndStartDownloading();
        }
    }

    void CUpdateInfoComponent::saveSettings()
    {
        const QString channel = this->getSelectedOrDefaultDistribution().getChannel();
        const QString platform = this->getSelectedOrDefaultPlatform().getPlatformName();
        const QStringList settings({ channel, platform });
        const CStatusMessage m = m_updateSettings.setAndSave(settings);
        if (m.isFailure()) { CLogMessage::preformatted(m); }
    }

    void CUpdateInfoComponent::channelChanged() { this->uiSelectionChanged(); }

    void CUpdateInfoComponent::platformChanged() { this->uiSelectionChanged(); }

    void CUpdateInfoComponent::uiSelectionChanged()
    {
        const CDistribution selectedDistribution(this->getSelectedOrDefaultDistribution());
        const CPlatform selectedPlatform(this->getSelectedOrDefaultPlatform());

        // for xswiftbus we only show public (unrestricted) ones, as the follow up dialog will only show unrestricted
        const CUpdateInfo updateInfo(m_updateInfo.get());
        const CArtifactList artifactsPilotClient = updateInfo.getArtifactsPilotClient().findByDistributionAndPlatform(
            selectedDistribution, selectedPlatform, true);
        const CArtifactList artifactsXsb =
            updateInfo.getArtifactsXSwiftBus().findWithUnrestrictedDistributions().findByDistributionAndPlatform(
                selectedDistribution, selectedPlatform, true);
        const CArtifact latestPilotClient = artifactsPilotClient.getLatestArtifactOrDefault();

        const QStringList sortedPilotClientVersions = artifactsPilotClient.getSortedVersions();
        ui->cb_ArtifactsPilotClient->clear();
        ui->cb_ArtifactsPilotClient->insertItems(0, sortedPilotClientVersions);
        ui->pb_DownloadInstaller->setEnabled(!artifactsPilotClient.isEmpty());

        const QStringList sortedXsbVersions = artifactsXsb.getSortedVersions();
        ui->cb_ArtifactsXsb->clear();
        ui->cb_ArtifactsXsb->insertItems(0, sortedXsbVersions);
        ui->pb_DownloadXSwiftBus->setEnabled(!artifactsXsb.isEmpty());

        // save the settings as this is needed afterwards
        this->saveSettings();

        //! \fixme hardcoded stylesheet color
        const bool newer = this->isNewPilotClientVersionAvailable();
        ui->lbl_StatusInfo->setText(newer ? "New version available" : "Nothing new");
        ui->lbl_StatusInfo->setStyleSheet(newer ? "background-color: green; color: white;" :
                                                  "background-color: red; color: white;");

        emit this->selectionChanged();

        if (newer && latestPilotClient.isLoadedFromDb()) { emit this->newerPilotClientAvailable(latestPilotClient); }
    }

    const CPlatform &CUpdateInfoComponent::getSelectedOrDefaultPlatform() const
    {
        const QStringList settings = m_updateSettings.get(); // channel / platform
        QString p = ui->cb_Platforms->currentText();
        if (p.isEmpty()) { p = settings.last(); }
        if (p.isEmpty()) { p = CPlatform::currentPlatform().getPlatformName(); }
        return CPlatform::stringToPlatformObject(p);
    }

    CDistribution CUpdateInfoComponent::getSelectedOrDefaultDistribution() const
    {
        const QStringList settings = m_updateSettings.get(); // channel / platform
        QString c = ui->cb_Channels->currentText();
        if (c.isEmpty()) { c = settings.first(); }
        const CUpdateInfo updateInfo(m_updateInfo.get());
        return c.isEmpty() ? updateInfo.getDistributions().getMostStableOrDefault() :
                             updateInfo.getDistributions().findFirstByChannelOrDefault(c);
    }
} // namespace swift::gui::components
