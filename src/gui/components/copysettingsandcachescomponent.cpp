// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "copysettingsandcachescomponent.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QStringBuilder>

#include "ui_copysettingsandcachescomponent.h"

#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/cachesettingsutils.h"
#include "misc/fileutils.h"
#include "misc/icons.h"
#include "misc/json.h"

using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::misc::input;
using namespace swift::misc::network;
using namespace swift::misc::network::data;
using namespace swift::misc::network::settings;
using namespace swift::misc::settings;
using namespace swift::misc::simulation::settings;
using namespace swift::core::audio;
using namespace swift::core::application;
using namespace swift::core::data;
using namespace swift::gui::settings;

namespace swift::gui::components
{
    CCopySettingsAndCachesComponent::CCopySettingsAndCachesComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CCopySettingsAndCachesComponent)
    {
        ui->setupUi(this);
        this->initAll();
        this->allCheckBoxesReadOnly();
        connect(ui->pb_SelectAll, &QPushButton::clicked, this, &CCopySettingsAndCachesComponent::selectAll);
        connect(ui->pb_DeselectAll, &QPushButton::clicked, this, &CCopySettingsAndCachesComponent::deselectAll);
        connect(ui->pb_Copy, &QPushButton::clicked, this, &CCopySettingsAndCachesComponent::copy);
        connect(ui->comp_OtherSwiftVersions, &COtherSwiftVersionsComponent::versionChanged, this,
                &CCopySettingsAndCachesComponent::onOtherVersionChanged);
    }

    CCopySettingsAndCachesComponent::~CCopySettingsAndCachesComponent() {}

    void CCopySettingsAndCachesComponent::reloadOtherVersions(int deferMs)
    {
        ui->comp_OtherSwiftVersions->reloadOtherVersionsDeferred(deferMs);
    }

    void CCopySettingsAndCachesComponent::onOtherVersionChanged(const CApplicationInfo &info)
    {
        readOnlyCheckbox(ui->cb_SettingsAudio,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsAudio.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsAudioInputDevice,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsAudio.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsAudioOutputDevice, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                               info, m_settingsAudioOutputDevice.getFilename()));

        readOnlyCheckbox(ui->cb_SettingsNetworkTrafficServers, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                                   info, m_settingsNetworkServers.getFilename()));
        readOnlyCheckbox(ui->cb_CacheLastNetworkServer,
                         !CCacheSettingsUtils::hasOtherVersionCacheFile(info, m_cacheLastNetworkServer.getFilename()));
        readOnlyCheckbox(ui->cb_CacheLastVatsimServer,
                         !CCacheSettingsUtils::hasOtherVersionCacheFile(info, m_cacheLastVatsimServer.getFilename()));

        readOnlyCheckbox(ui->cb_SettingsGuiGeneral,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsGuiGeneral.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsDockWidget,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsDockWidget.getFilename()));

        readOnlyCheckbox(ui->cb_SettingsEnabledSimulators, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                               info, m_settingsEnabledSimulators.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsSimulatorFSX,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsSimulatorFsx.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsSimulatorP3D,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsSimulatorP3D.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsSimulatorXPlane, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                             info, m_settingsSimulatorXPlane.getFilename()));

        readOnlyCheckbox(ui->cb_SettingsActionHotkeys, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                           info, m_settingsActionHotkeys.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsTextMessages,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsTextMessage.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsDirectories,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsDirectories.getFilename()));

        readOnlyCheckbox(ui->cb_SettingsConsolidation, !CCacheSettingsUtils::hasOtherVersionSettingsFile(
                                                           info, m_settingsConsolidation.getFilename()));
        readOnlyCheckbox(ui->cb_SettingsModel,
                         !CCacheSettingsUtils::hasOtherVersionSettingsFile(info, m_settingsModel.getFilename()));
    }

    void CCopySettingsAndCachesComponent::initAll()
    {
        this->initAudio();
        this->initNetwork();
        this->initUi();
        this->initSimulator();
        this->initMisc();
        this->initModel();
    }

    void CCopySettingsAndCachesComponent::initAudio()
    {
        ui->cb_SettingsAudio->setText(checkBoxText(TSettings::humanReadable(), true));
        ui->cb_SettingsAudioInputDevice->setText(checkBoxText(TInputDevice::humanReadable(), true));
        ui->cb_SettingsAudioOutputDevice->setText(checkBoxText(TOutputDevice::humanReadable(), true));
    }

    void CCopySettingsAndCachesComponent::initNetwork()
    {
        ui->cb_SettingsNetworkTrafficServers->setText(checkBoxText(TTrafficServers::humanReadable(), true));
        ui->cb_CacheLastNetworkServer->setText(checkBoxText(TLastServer::humanReadable(), false));
        ui->cb_CacheLastVatsimServer->setText(checkBoxText(TVatsimLastServer::humanReadable(), false));
    }

    void CCopySettingsAndCachesComponent::initUi()
    {
        ui->cb_SettingsGuiGeneral->setText(checkBoxText(TGeneralGui::humanReadable(), true));
        ui->cb_SettingsDockWidget->setText(checkBoxText(TDockWidget::humanReadable(), true));
    }

    void CCopySettingsAndCachesComponent::initSimulator()
    {
        ui->cb_SettingsEnabledSimulators->setText(checkBoxText(TEnabledSimulators::humanReadable(), true));
        ui->cb_SettingsSimulatorFSX->setText(checkBoxText(TSimulatorFsx::humanReadable(), true));
        ui->cb_SettingsSimulatorP3D->setText(checkBoxText(TSimulatorP3D::humanReadable(), true));
        ui->cb_SettingsSimulatorXPlane->setText(checkBoxText(TSimulatorXP::humanReadable(), true));
    }

    void CCopySettingsAndCachesComponent::initMisc()
    {
        ui->cb_SettingsActionHotkeys->setText(checkBoxText(TActionHotkeys::humanReadable(), true));
        ui->cb_SettingsTextMessages->setText(checkBoxText(TextMessageSettings::humanReadable(), true));
        ui->cb_SettingsDirectories->setText(checkBoxText(TDirectorySettings::humanReadable(), true));
    }

    void CCopySettingsAndCachesComponent::initModel()
    {
        ui->cb_SettingsModel->setText(checkBoxText(TModel::humanReadable(), true));
        ui->cb_SettingsConsolidation->setText(checkBoxText(TBackgroundConsolidation::humanReadable(), true));
    }

    int CCopySettingsAndCachesComponent::copy()
    {
        ui->le_Status->clear();

        const CApplicationInfo otherVersionInfo = ui->comp_OtherSwiftVersions->selectedOtherVersion();
        if (otherVersionInfo.isNull()) { return 0; }

        bool success = false;
        int copied = 0;
        QString errMsg;
        ui->le_Status->setText("Starting to copy from '" + otherVersionInfo.toQString(true) + "'");

        // ------- audio -------
        if (ui->cb_SettingsAudio->isChecked())
        {
            const QString joStr =
                CCacheSettingsUtils::otherVersionSettingsFileContent(otherVersionInfo, m_settingsAudio.getFilename());
            if (!joStr.isEmpty())
            {
                const audio::CSettings audioSettings = audio::CSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsAudio.getKey()))
                {
                    this->displayStatusMessage(m_settingsAudio.setAndSave(audioSettings),
                                               audioSettings.toQString(true));
                    copied++;
                }
            }
        }

        if (ui->cb_SettingsAudioInputDevice->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsAudioInputDevice.getFilename());
            const QString audioInputSettings = json::firstJsonValueAsString(joStr);
            if (!audioInputSettings.isEmpty())
            {
                this->displayStatusMessage(m_settingsAudioInputDevice.setAndSave(audioInputSettings),
                                           audioInputSettings);
                copied++;
            }
        }

        if (ui->cb_SettingsAudioOutputDevice->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsAudioOutputDevice.getFilename());
            const QString audioOutputSettings = json::firstJsonValueAsString(joStr);
            if (!audioOutputSettings.isEmpty())
            {
                this->displayStatusMessage(m_settingsAudioOutputDevice.setAndSave(audioOutputSettings),
                                           audioOutputSettings);
                copied++;
            }
        }

        // ------- directories -------
        if (ui->cb_SettingsDirectories->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsDirectories.getFilename());
            if (!joStr.isEmpty())
            {
                const CDirectories directories = CDirectories::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsDirectories.getKey()))
                {
                    this->displayStatusMessage(m_settingsDirectories.setAndSave(directories),
                                               directories.toQString(true));
                }
                copied++;
            }
        }

        // ------- network -------
        if (ui->cb_SettingsNetworkTrafficServers->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsNetworkServers.getFilename());
            if (!joStr.isEmpty())
            {
                const CServerList networkServers = CServerList::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsNetworkServers.getKey()))
                {
                    this->displayStatusMessage(m_settingsNetworkServers.setAndSave(networkServers),
                                               networkServers.toQString(true));
                    copied++;
                }
            }
        }

        if (ui->cb_CacheLastNetworkServer->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionCacheFileContent(
                otherVersionInfo, m_cacheLastNetworkServer.getFilename());
            if (!joStr.isEmpty())
            {
                const CServer server = CServer::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_cacheLastNetworkServer.getKey()))
                {
                    this->displayStatusMessage(m_cacheLastNetworkServer.set(server), server.toQString(true));
                    copied++;
                }
            }
        }

        if (ui->cb_CacheLastVatsimServer->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionCacheFileContent(
                otherVersionInfo, m_cacheLastVatsimServer.getFilename());
            if (!joStr.isEmpty())
            {
                const CServer server = CServer::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_cacheLastVatsimServer.getKey()))
                {
                    this->displayStatusMessage(m_cacheLastVatsimServer.set(server), server.toQString(true));
                    copied++;
                }
            }
        }

        // ------- GUI -------
        if (ui->cb_SettingsGuiGeneral->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsGuiGeneral.getFilename());
            if (!joStr.isEmpty())
            {
                const CGeneralGuiSettings guiGeneral =
                    CGeneralGuiSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                this->displayStatusMessage(m_settingsGuiGeneral.setAndSave(guiGeneral), guiGeneral.toQString(true));
                copied++;
            }
        }

        if (ui->cb_SettingsDockWidget->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsDockWidget.getFilename());
            if (!joStr.isEmpty())
            {
                const CDockWidgetSettings dwSettings =
                    CDockWidgetSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                this->displayStatusMessage(m_settingsDockWidget.setAndSave(dwSettings), dwSettings.toQString(true));
                copied++;
            }
        }

        // ------- sims -------
        if (ui->cb_SettingsEnabledSimulators->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsEnabledSimulators.getFilename());
            if (!joStr.isEmpty())
            {
                const QStringList enabledSims = json::firstJsonValueAsStringList(joStr);
                if (!enabledSims.isEmpty())
                {
                    this->displayStatusMessage(m_settingsEnabledSimulators.setAndSave(enabledSims),
                                               enabledSims.join(", "));
                    copied++;
                }
            }
        }

        if (ui->cb_SettingsSimulatorFSX->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsSimulatorFsx.getFilename());
            if (!joStr.isEmpty())
            {
                const CSimulatorSettings settings = CSimulatorSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsSimulatorFsx.getKey()))
                {
                    this->displayStatusMessage(m_settingsSimulatorFsx.setAndSave(settings), settings.toQString(true));
                }
                copied++;
            }
        }

        if (ui->cb_SettingsSimulatorP3D->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsSimulatorP3D.getFilename());
            if (!joStr.isEmpty())
            {
                const CSimulatorSettings settings = CSimulatorSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsSimulatorP3D.getKey()))
                {
                    this->displayStatusMessage(m_settingsSimulatorP3D.setAndSave(settings), settings.toQString(true));
                }
                copied++;
            }
        }

        if (ui->cb_SettingsSimulatorXPlane->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsSimulatorXPlane.getFilename());
            if (!joStr.isEmpty())
            {
                const CSimulatorSettings settings = CSimulatorSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsSimulatorXPlane.getKey()))
                {
                    this->displayStatusMessage(m_settingsSimulatorXPlane.setAndSave(settings),
                                               settings.toQString(true));
                }
                copied++;
            }
        }

        // ------ model ------
        if (ui->cb_SettingsModel->isChecked())
        {
            const QString joStr =
                CCacheSettingsUtils::otherVersionSettingsFileContent(otherVersionInfo, m_settingsModel.getFilename());
            if (!joStr.isEmpty())
            {
                const CModelSettings settings = CModelSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsModel.getKey()))
                {
                    this->displayStatusMessage(m_settingsModel.setAndSave(settings), settings.toQString(true));
                    copied++;
                }
            }
        }

        if (ui->cb_SettingsConsolidation->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsConsolidation.getFilename());
            if (!joStr.isEmpty())
            {
                bool ok = false;
                const int consolidation = json::firstJsonValueAsInt(joStr, -1, &ok);
                if (ok)
                {
                    this->displayStatusMessage(m_settingsConsolidation.setAndSave(consolidation),
                                               QString::number(consolidation));
                    copied++;
                }
            }
        }

        // ------ misc -------
        if (ui->cb_SettingsActionHotkeys->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsActionHotkeys.getFilename());
            if (!joStr.isEmpty())
            {
                CActionHotkeyList hotkeys = CActionHotkeyList::fromJsonNoThrow(joStr, true, success, errMsg);
                hotkeys.updateToCurrentMachine();
                if (this->parsingMessage(success, errMsg, m_settingsActionHotkeys.getKey()))
                {
                    this->displayStatusMessage(m_settingsActionHotkeys.setAndSave(hotkeys), hotkeys.toQString(true));
                    copied++;
                }
            }
        }

        if (ui->cb_SettingsTextMessages->isChecked())
        {
            const QString joStr = CCacheSettingsUtils::otherVersionSettingsFileContent(
                otherVersionInfo, m_settingsTextMessage.getFilename());
            if (!joStr.isEmpty())
            {
                const CTextMessageSettings settings =
                    CTextMessageSettings::fromJsonNoThrow(joStr, true, success, errMsg);
                if (this->parsingMessage(success, errMsg, m_settingsTextMessage.getKey()))
                {
                    this->displayStatusMessage(m_settingsTextMessage.setAndSave(settings), settings.toQString(true));
                    copied++;
                }
            }
        }

        if (copied > 0)
        {
            const CStatusMessage m = CStatusMessage(this).validationInfo(u"Copied %1 settings") << copied;
            this->showOverlayHTMLMessage(m);
        }

        return copied;
    }

    void CCopySettingsAndCachesComponent::selectAll()
    {
        for (QCheckBox *cb : this->checkBoxes()) { cb->setChecked(cb->isEnabled()); }
    }

    void CCopySettingsAndCachesComponent::deselectAll()
    {
        for (QCheckBox *cb : this->checkBoxes()) { cb->setChecked(false); }
    }

    QList<QCheckBox *> CCopySettingsAndCachesComponent::checkBoxes() const { return this->findChildren<QCheckBox *>(); }

    bool CCopySettingsAndCachesComponent::parsingMessage(bool success, const QString &msg, const QString &value)
    {
        if (success) { return true; }
        const CStatusMessage sm(this, msg);
        this->displayStatusMessage(sm, value);
        return false;
    }

    void CCopySettingsAndCachesComponent::displayStatusMessage(const CStatusMessage &msg, const QString &value)
    {
        if (msg.isEmpty()) { return; }
        if (value.isEmpty()) { ui->le_Status->setText(msg.getMessage()); }
        else { ui->le_Status->setText(msg.getMessage() % u' ' % value); }
        if (sGui) { sGui->processEventsToRefreshGui(); }
    }

    void CCopySettingsAndCachesComponent::allCheckBoxesReadOnly()
    {
        for (QCheckBox *cb : checkBoxes()) { readOnlyCheckbox(cb, true); }
    }

    void CCopySettingsAndCachesComponent::readOnlyCheckbox(QCheckBox *cb, bool readOnly)
    {
        Q_ASSERT_X(cb, Q_FUNC_INFO, "need checkbox");
        CGuiUtility::checkBoxReadOnly(cb, readOnly);
        if (readOnly) { cb->setChecked(false); }
        cb->setEnabled(!readOnly);
        // cb->setIcon(readOnly ? CIcons::cross16() : CIcons::tick16());
    }

    QString CCopySettingsAndCachesComponent::checkBoxText(const QString &text, bool setting)
    {
        return setting ? QStringLiteral("%1 [setting]").arg(text) : QStringLiteral("%1 [cache]").arg(text);
    }

    void CCopySettingsAndCachesWizardPage::initializePage()
    {
        // re-init other versions
        if (m_copyCachesAndSettings) { m_copyCachesAndSettings->reloadOtherVersions(1000); }
    }

    bool CCopySettingsAndCachesWizardPage::validatePage() { return true; }

} // namespace swift::gui::components
