/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distributioninfocomponent.h"
#include "ui_distributioninfocomponent.h"
#include "installxswiftbusdialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

using namespace BlackConfig;
using namespace BlackCore::Application;
using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDistributionInfoComponent::CDistributionInfoComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDistributionInfoComponent)
        {
            ui->setupUi(this);
            ui->lbl_NewVersionUrl->setTextFormat(Qt::RichText);
            ui->lbl_NewVersionUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_NewVersionUrl->setOpenExternalLinks(true);

            // use version signal as trigger for completion
            if (!m_distributionsInfo.get().isEmpty())
            {
                // we have at least cached data:
                // in case CGuiApplication::distributionInfoAvailable never comes/was already sent
                QTimer::singleShot(15 * 1000, this, [ = ]
                {
                    // use this as timeout failover with cached data
                    if (m_distributionsLoaded) { return; }
                    this->ps_loadedDistributionInfo(true);
                });
            }

            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
            connect(sGui, &CGuiApplication::distributionInfoAvailable, this, &CDistributionInfoComponent::ps_loadedDistributionInfo);
            connect(ui->pb_CheckForUpdates, &QPushButton::pressed, this, &CDistributionInfoComponent::ps_requestLoadOfSetup);
            connect(ui->pb_InstallXSwiftBus, &QPushButton::pressed, this, &CDistributionInfoComponent::ps_installXSwiftBusDialog);
        }

        CDistributionInfoComponent::~CDistributionInfoComponent()
        { }

        bool CDistributionInfoComponent::isNewVersionAvailable() const
        {
            const QStringList channelPlatform = m_distributionSetting.get();
            Q_ASSERT_X(channelPlatform.size() == 2, Q_FUNC_INFO, "wrong setting");
            const QVersionNumber vCurrentChannelPlatform = m_distributionsInfo.get().getQVersionForChannelAndPlatform(channelPlatform);
            if (vCurrentChannelPlatform.isNull() || vCurrentChannelPlatform.segmentCount() < 4) return false;
            const QVersionNumber vCurrent = CBuildConfig::getVersion();
            return (vCurrentChannelPlatform > vCurrent);
        }

        void CDistributionInfoComponent::ps_requestLoadOfSetup()
        {
            if (sGui && !ui->le_LatestVersion->text().isEmpty())
            {
                const CStatusMessageList msgs(sGui->requestReloadOfSetupAndVersion());
                CLogMessage::preformatted(msgs);
                if (msgs.isSuccess())
                {
                    ui->le_LatestVersion->setText("");
                    m_distributionsLoaded = false; // reset
                }
            }
        }

        void CDistributionInfoComponent::ps_loadedDistributionInfo(bool success)
        {
            if (!success)
            {
                m_distributionsLoaded = false;
                ui->pb_CheckForUpdates->setToolTip("");
                CLogMessage(this).warning("Loading setup or distribution information failed");
                return;
            }

            // only emit once
            if (m_distributionsLoaded) { return; }
            m_distributionsLoaded = true;
            this->ps_channelChanged();
            ui->pb_CheckForUpdates->setToolTip(sApp->getLastSuccesfulDistributionUrl());

            // emit only after all has been set
            emit this->distributionInfoAvailable(success);
        }

        void CDistributionInfoComponent::ps_changedDistributionCache()
        {
            this->ps_loadedDistributionInfo(true);
        }

        void CDistributionInfoComponent::ps_installXSwiftBusDialog()
        {
            if (!m_installXSwiftBusDialog)
            {
                m_installXSwiftBusDialog.reset(new CInstallXSwiftBusDialog(this));
                m_installXSwiftBusDialog->setModal(true);
            }
            m_installXSwiftBusDialog->show();
        }

        void CDistributionInfoComponent::saveSettings()
        {
            const QString channel = ui->cb_Channels->currentText();
            const QString currentPlatform = ui->cb_Platforms->currentText();
            const QStringList settings({ channel, currentPlatform });
            const CStatusMessage m = m_distributionSetting.setAndSave(settings);
            if (m.isFailure())
            {
                CLogMessage(this).preformatted(m);
            }
        }

        void CDistributionInfoComponent::ps_channelChanged()
        {
            const CDistributionList distributions(m_distributionsInfo.get());
            const QStringList channels = distributions.getChannels().toList();
            const QStringList channelPlatformSetting = m_distributionSetting.get(); // channel / platform
            Q_ASSERT_X(channelPlatformSetting.size() == 2, Q_FUNC_INFO, "Settings");

            // default value
            QString channel = ui->cb_Channels->currentText();
            if (channel.isEmpty()) { channel = channelPlatformSetting.front(); }
            if (channel.isEmpty() && !channels.isEmpty()) { channel = channels.front(); }

            // channels (will be connected below)
            ui->cb_Channels->disconnect();
            ui->cb_Platforms->disconnect();
            ui->cb_Channels->clear();
            ui->cb_Channels->insertItems(0, channels);
            if (!channel.isEmpty()) { ui->cb_Channels->setCurrentText(channel); }

            // current distribution
            const CDistribution currentDistribution = distributions.findByChannelOrDefault(channel);
            const QStringList platforms = currentDistribution.getPlatforms();
            m_currentDistribution = currentDistribution;
            ui->le_CurrentVersion->setText(CBuildConfig::getVersionString());

            // platforms
            QString platform = ui->cb_Platforms->currentText();
            if (platform.isEmpty()) { platform = channelPlatformSetting.last(); }
            if (platform.isEmpty() || !platforms.contains(platform)) { platform = currentDistribution.guessMyPlatform(); }

            ui->cb_Platforms->clear();
            ui->cb_Platforms->insertItems(0, platforms);
            if (!platform.isEmpty()) { ui->cb_Platforms->setCurrentText(platform); }

            // platform dependent stuff
            this->ps_platformChanged();
            connect(ui->cb_Channels, &QComboBox::currentTextChanged, this, &CDistributionInfoComponent::ps_channelChanged);
            connect(ui->cb_Platforms, &QComboBox::currentTextChanged, this, &CDistributionInfoComponent::ps_platformChanged);
        }

        void CDistributionInfoComponent::ps_platformChanged()
        {
            this->saveSettings();

            // defaults
            ui->le_LatestVersion->clear();
            ui->lbl_NewVersionInfo->setText("Nothing new");
            ui->lbl_NewVersionInfo->setStyleSheet("background-color: green");
            ui->lbl_NewVersionUrl->clear();
            m_newVersionAvailable.clear();

            const QString currentPlatform = this->getSelectedOrGuessedPlatform();
            if (!currentPlatform.isEmpty())
            {
                const QVersionNumber latestVersion = m_currentDistribution.getQVersion(currentPlatform);
                const QString latestVersionStr = m_currentDistribution.getVersionString(currentPlatform);
                ui->le_LatestVersion->setText(latestVersionStr);
                ui->le_LatestVersion->setToolTip("");

                CFailoverUrlList downloadUrls(m_currentDistribution.getDownloadUrls());
                const CUrl downloadUrl(downloadUrls.obtainNextUrl());
                const bool newer = CBuildConfig::getVersion() < latestVersion;
                if (newer)
                {
                    ui->lbl_NewVersionInfo->setText("New version!");
                    ui->lbl_NewVersionInfo->setToolTip("New version '" + latestVersionStr + "'");
                    ui->lbl_NewVersionInfo->setStyleSheet("background-color: red");
                    m_newVersionAvailable = latestVersionStr;
                }

                if (!downloadUrl.isEmpty())
                {
                    const QString urlStr(downloadUrl.toQString());
                    const QString hl("<a href=\"%1\"><img src=\":/own/icons/own/drophere16.png\"></a> %2");
                    ui->lbl_NewVersionUrl->setText(hl.arg(urlStr, currentPlatform));
                    ui->lbl_NewVersionUrl->setToolTip("Download '" + latestVersionStr + "' " + m_currentDistribution.getFilename(currentPlatform));
                }

                emit selectionChanged();
            }
        }

        QString CDistributionInfoComponent::getSelectedOrGuessedPlatform() const
        {
            QString p = ui->cb_Platforms->currentText();
            if (p.isEmpty())
            {
                const CDistributionList distributions = m_distributionsInfo.get();
                p = distributions.findByChannelOrDefault(ui->cb_Channels->currentText()).guessMyPlatform();
            }
            return p;
        }
    } // ns
} // ns
