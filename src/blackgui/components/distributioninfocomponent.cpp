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
#include "blackconfig/buildconfig.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/db/distributionlist.h"
#include "blackmisc/logmessage.h"

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
            connect(sGui, &CGuiApplication::distributionInfoAvailable, this, &CDistributionInfoComponent::ps_loadedDistributionInfo);
            QTimer::singleShot(10 * 1000, this, [ = ]
            {
                // use this as timeout failover with cached data
                if (m_distributionsLoaded) { return; }
                this->ps_loadedDistributionInfo(true);
            });

            connect(ui->pb_CheckForUpdates, &QPushButton::pressed, this, &CDistributionInfoComponent::ps_loadSetup);
        }

        CDistributionInfoComponent::~CDistributionInfoComponent()
        { }

        void CDistributionInfoComponent::ps_loadSetup()
        {
            if (!ui->le_LatestVersion->text().isEmpty())
            {
                ui->le_LatestVersion->setText("");
                const CStatusMessageList msgs(sApp->requestReloadOfSetupAndVersion());
                CLogMessage::preformatted(msgs);
            }
        }

        void CDistributionInfoComponent::ps_loadedDistributionInfo(bool success)
        {
            ui->pb_CheckForUpdates->setToolTip("");
            if (!success)
            {
                CLogMessage(this).warning("Loading setup or distribution information failed");
                return;
            }

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

        void CDistributionInfoComponent::saveSettings()
        {
            const QString channel = ui->cb_Channels->currentText();
            const QString currentPlatform = ui->cb_Platforms->currentText();
            const QStringList settings({ channel, currentPlatform });
            const CStatusMessage m = this->m_distributionSettings.setAndSave(settings);
            if (m.isFailure())
            {
                CLogMessage(this).preformatted(m);
            }
        }

        void CDistributionInfoComponent::ps_channelChanged()
        {
            const CDistributionList distributions(m_distributionInfo.get());
            const QStringList channels = distributions.getChannels();
            const QStringList settings = m_distributionSettings.get(); // channel / platform

            // default value
            QString channel = ui->cb_Channels->currentText();
            if (channel.isEmpty()) { channel = settings.front(); }
            if (channel.isEmpty() && !channels.isEmpty()) { channel = channels.front(); }

            // channels
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
            if (platform.isEmpty()) { platform = settings.last(); }
            if (platform.isEmpty()) { platform = currentDistribution.guessPlatform(); }

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
            this->m_newVersionAvailable.clear();

            const QString currentPlatform = ui->cb_Platforms->currentText();
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
                    this->m_newVersionAvailable = latestVersionStr;
                }

                if (!downloadUrl.isEmpty())
                {
                    const QString urlStr(downloadUrl.toQString());
                    const QString hl("<a href=\"%1\"><img src=\":/own/icons/own/drophere16.png\"></a> %2");
                    ui->lbl_NewVersionUrl->setText(hl.arg(urlStr, currentPlatform));
                    ui->lbl_NewVersionUrl->setToolTip("Download '" + latestVersionStr + "' " + m_currentDistribution.getFilename(currentPlatform));
                }
            }
        }
    } // ns
} // ns
