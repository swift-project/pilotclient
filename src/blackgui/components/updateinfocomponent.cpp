/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "updateinfocomponent.h"
#include "ui_updateinfocomponent.h"
#include "installxswiftbusdialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

#include<QDesktopServices>

using namespace BlackConfig;
using namespace BlackCore::Application;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CUpdateInfoComponent::CUpdateInfoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CUpdateInfoComponent)
        {
            ui->setupUi(this);
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

            connect(sGui, &CGuiApplication::updateInfoAvailable, this, &CUpdateInfoComponent::changedUpdateInfo);
            connect(ui->pb_CheckForUpdates, &QPushButton::pressed, this, &CUpdateInfoComponent::requestLoadOfSetup);
            connect(ui->pb_DownloadXSwiftBus, &QPushButton::pressed, this, &CUpdateInfoComponent::downloadXSwiftBusDialog);
            connect(ui->pb_DownloadInstaller, &QPushButton::pressed, this, &CUpdateInfoComponent::downloadInstallerDialog);

            // use version signal as trigger for completion
            if (!m_updateInfo.get().isEmpty()) { this->changedUpdateInfo(); }
        }

        CUpdateInfoComponent::~CUpdateInfoComponent()
        { }

        CArtifact CUpdateInfoComponent::getLatestAvailablePilotClientArtifactForSelection() const
        {
            const CUpdateInfo info = m_updateInfo.get();
            const CPlatform p = this->getSelectedOrDefaultPlatform();
            const CDistribution d = this->getSelectedOrDefaultDistribution();
            const CArtifact a = info.getArtifactsPilotClient().findByDistributionAndPlatform(d, p, true).getLatestArtifactOrDefault();
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
            const QVersionNumber vCurrentChannelPlatform = info.getArtifactsPilotClient().findByDistribution(d).getLatestQVersion();
            const QVersionNumber vCurrent = CBuildConfig::getVersion();
            if (vCurrentChannelPlatform.isNull()) { return false; }
            return (vCurrentChannelPlatform > vCurrent);
        }

        void CUpdateInfoComponent::requestLoadOfSetup()
        {
            if (!sGui) { return; }
            const CStatusMessageList msgs(sGui->requestReloadOfSetupAndVersion());
            CLogMessage::preformatted(msgs);
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
                ui->cb_Platforms->insertItem(i++, platform.toPixmap(), platform.getPlatformName());
            }
            if (platforms.contains(settings.last())) { ui->cb_Platforms->setCurrentText(settings.last()); }
            else if (platforms.contains(CPlatform::currentPlatform().getPlatformName())) { ui->cb_Platforms->setCurrentText(CPlatform::currentPlatform().getPlatformName()); }

            i = 0;
            ui->cb_Channels->disconnect();
            ui->cb_Channels->clear();
            for (const CDistribution &distribution : distributions)
            {
                ui->cb_Channels->insertItem(i++, distribution.getRestrictionIcon().toPixmap(), distribution.getChannel());
            }
            if (distributions.containsChannel(settings.front())) { ui->cb_Channels->setCurrentText(settings.front()); }

            this->uiSelectionChanged();
            connect(ui->cb_Platforms, &QComboBox::currentTextChanged, this, &CUpdateInfoComponent::platformChanged);
            connect(ui->cb_Channels, &QComboBox::currentTextChanged, this, &CUpdateInfoComponent::channelChanged);

            // emit via digest signal
            m_dsDistributionAvailable.inputSignal();
        }

        void CUpdateInfoComponent::downloadXSwiftBusDialog()
        {
            if (!m_installXSwiftBusDialog)
            {
                m_installXSwiftBusDialog.reset(new CInstallXSwiftBusDialog(this));
                m_installXSwiftBusDialog->setModal(true);
            }

            const QString current = ui->cb_ArtifactsXsb->currentText();
            m_installXSwiftBusDialog->setDefaultDownloadName(current);
            m_installXSwiftBusDialog->show();
        }

        void CUpdateInfoComponent::saveSettings()
        {
            const QString channel = this->getSelectedOrDefaultDistribution().getChannel();
            const QString platform = this->getSelectedOrDefaultPlatform().getPlatformName();
            const QStringList settings({ channel, platform });
            const CStatusMessage m = m_updateSettings.setAndSave(settings);
            if (m.isFailure())
            {
                CLogMessage(this).preformatted(m);
            }
        }

        void CUpdateInfoComponent::channelChanged()
        {
            this->uiSelectionChanged();
        }

        void CUpdateInfoComponent::platformChanged()
        {
            this->uiSelectionChanged();
        }

        void CUpdateInfoComponent::uiSelectionChanged()
        {
            const CDistribution selectedDistribution(this->getSelectedOrDefaultDistribution());
            const CPlatform selectedPlatform(this->getSelectedOrDefaultPlatform());

            const CUpdateInfo updateInfo(m_updateInfo.get());
            const CArtifactList artifactsPilotClient = updateInfo.getArtifactsPilotClient().findByDistributionAndPlatform(selectedDistribution, selectedPlatform, true);
            const CArtifactList artifactsXsb = updateInfo.getArtifactsXsb().findByDistributionAndPlatform(selectedDistribution, selectedPlatform, true);

            const QStringList sortedPilotClientVersions = artifactsPilotClient.getSortedVersions();
            ui->cb_ArtifactsPilotClient->clear();
            ui->cb_ArtifactsPilotClient->insertItems(0, sortedPilotClientVersions);
            ui->pb_DownloadInstaller->setEnabled(!artifactsPilotClient.isEmpty());

            const QStringList sortedXsbVersions = artifactsXsb.getSortedVersions();
            ui->cb_ArtifactsXsb->clear();
            ui->cb_ArtifactsXsb->insertItems(0, sortedXsbVersions);
            ui->pb_DownloadXSwiftBus->setEnabled(!artifactsXsb.isEmpty());

            const bool newer = this->isNewPilotClientVersionAvailable();
            ui->lbl_StatusInfo->setText(newer ? "New version available" : "Nothing new");
            ui->lbl_StatusInfo->setStyleSheet(newer ?
                                              "background-color: green; color: white;" :
                                              "background-color: red; color: white;");

            this->saveSettings();
            emit this->selectionChanged();
        }

        const CPlatform &CUpdateInfoComponent::getSelectedOrDefaultPlatform() const
        {
            const QStringList settings = m_updateSettings.get(); // channel / platform
            QString p = ui->cb_Platforms->currentText();
            if (p.isEmpty()) { p = settings.last(); }
            if (p.isEmpty()) { p = CPlatform::currentPlatform().getPlatformName(); }
            return CPlatform::stringToPlatformObject(p);
        }

        const CDistribution CUpdateInfoComponent::getSelectedOrDefaultDistribution() const
        {
            const QStringList settings = m_updateSettings.get(); // channel / platform
            QString c = ui->cb_Channels->currentText();
            if (c.isEmpty()) { c = settings.first(); }
            const CUpdateInfo updateInfo(m_updateInfo.get());
            return c.isEmpty() ?
                   updateInfo.getDistributions().getMostStableOrDefault() :
                   updateInfo.getDistributions().findFirstByChannelOrDefault(c);
        }
    } // ns
} // ns
