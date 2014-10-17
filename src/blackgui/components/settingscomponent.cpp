/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingscomponent.h"
#include "ui_settingscomponent.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackgui/stylesheetutility.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_audio.h"
#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/settingsblackmiscclasses.h"
#include <QColorDialog>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Hardware;

namespace BlackGui
{
    namespace Components
    {
        CSettingsComponent::CSettingsComponent(QWidget *parent) :
            QTabWidget(parent),
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CSettingsComponent)
        {
            ui->setupUi(this);
            this->tabBar()->setExpanding(false);
        }

        CSettingsComponent::~CSettingsComponent()
        { }

        bool CSettingsComponent::playNotificationSounds() const
        {
            return ui->comp_AudioSetup->playNotificationSounds();
        }

        /*
         * Update own ICAO data from GUI
         */
        void CSettingsComponent::setOwnAircraftIcaoDataFromGui(CAircraftIcao &icao) const
        {
            icao.setAirlineDesignator(this->ui->le_SettingsIcaoAirlineDesignator->text());
            icao.setAircraftDesignator(this->ui->le_SettingsIcaoAircraftDesignator->text());
            icao.setAircraftCombinedType(this->ui->le_SettingsIcaoCombinedType->text());
        }

        void CSettingsComponent::setGuiOpacity(double value)
        {
            this->ui->hs_SettingsGuiOpacity->setValue(value);
        }

        bool CSettingsComponent::loginAsObserver() const
        {
            return this->ui->rb_SettingsLoginStealthMode->isChecked();
        }

        bool CSettingsComponent::loginStealth() const
        {
            return this->ui->rb_SettingsLoginStealthMode->isChecked();
        }

        int CSettingsComponent::getAtcUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAtcRefreshTime->value();
        }

        int CSettingsComponent::getAircraftUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiAircraftRefreshTime->value();
        }

        int CSettingsComponent::getUsersUpdateIntervalSeconds() const
        {
            return this->ui->hs_SettingsGuiUserRefreshTime->value();
        }

        QString CSettingsComponent::getOwnCallsignFromGui() const
        {
            return this->ui->le_SettingsAircraftCallsign->text();
        }

        /*
         * Reload settings
         */
        void CSettingsComponent::reloadSettings()
        {
            // local copy
            CSettingsNetwork nws = this->getIContextSettings()->getNetworkSettings();

            // update servers
            this->ui->tvp_SettingsTnServers->setSelectedServer(nws.getCurrentTrafficNetworkServer());
            this->ui->tvp_SettingsTnServers->updateContainer(nws.getTrafficNetworkServers());

            // update hot keys
            this->ui->tvp_SettingsMiscHotkeys->updateContainer(this->getIContextSettings()->getHotkeys());
        }

        /*
         * Set tab
         */
        void CSettingsComponent::setSettingsTab(CSettingsComponent::SettingTab tab)
        {
            this->setCurrentIndex(static_cast<int>(tab));
        }

        /*
         * Runtime set
         */
        void CSettingsComponent::runtimeHasBeenSet()
        {
            if (!this->getIContextSettings()) qFatal("Settings missing");
            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsComponent::ps_changedSettings);

            // Opacity, intervals
            bool connected = false;
            this->connect(this->ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsComponent::changedWindowsOpacity);
            this->connect(this->ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);

            // Settings server
            this->connect(this->ui->pb_SettingsTnCurrentServer, &QPushButton::released, this, &CSettingsComponent::ps_alterTrafficServer);
            this->connect(this->ui->pb_SettingsTnRemoveServer, &QPushButton::released, this, &CSettingsComponent::ps_alterTrafficServer);
            this->connect(this->ui->pb_SettingsTnSaveServer, &QPushButton::released, this, &CSettingsComponent::ps_alterTrafficServer);
            this->connect(this->ui->tvp_SettingsTnServers, &QTableView::clicked, this, &CSettingsComponent::ps_networkServerSelected);

            // Settings hotkeys
            this->connect(this->ui->pb_SettingsMiscCancel, &QPushButton::clicked, this, &CSettingsComponent::reloadSettings);
            this->connect(this->ui->pb_SettingsMiscSave, &QPushButton::clicked, this, &CSettingsComponent::ps_saveHotkeys);
            this->connect(this->ui->pb_SettingsMiscRemove, &QPushButton::clicked, this, &CSettingsComponent::ps_clearHotkey);

            // Font
            const QFont font = this->font();
            this->ui->cb_SettingsGuiFontStyle->setCurrentText(CStyleSheetUtility::fontAsCombinedWeightStyle(font));
            this->ui->cb_SettingsGuiFont->setCurrentFont(font);
            this->ui->cb_SettingsGuiFontSize->setCurrentText(QString::number(font.pointSize()));
            this->m_fontColor = QColor(CStyleSheetUtility::instance().fontColor());
            this->ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());
            connected = this->connect(this->ui->cb_SettingsGuiFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            this->connect(this->ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsComponent::ps_fontColorDialog);
        }

        /*
         * Network has been selected
         */
        void CSettingsComponent::ps_networkServerSelected(QModelIndex index)
        {
            const CServer clickedServer = this->ui->tvp_SettingsTnServers->at<CServer>(index);
            this->ps_updateGuiSelectedServerTextboxes(clickedServer);
        }

        /*
         * Alter server
         */
        void CSettingsComponent::ps_alterTrafficServer()
        {
            CServer server = this->ps_selectedServerFromTextboxes();
            if (!server.isValidForLogin())
            {
                CLogMessage().validationWarning("Wrong settings for server");
                return;
            }

            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathNetworkSettings(), CSettingsNetwork::ValueTrafficServers());
            QObject *sender = QObject::sender();
            if (sender == this->ui->pb_SettingsTnCurrentServer)
            {
                this->getIContextSettings()->value(path, CSettingsNetwork::CmdSetCurrentServer(), server.toQVariant());
            }
            else if (sender == this->ui->pb_SettingsTnRemoveServer)
            {
                this->getIContextSettings()->value(path, CSettingUtilities::CmdRemove(), server.toQVariant());
            }
            else if (sender == this->ui->pb_SettingsTnSaveServer)
            {
                this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), server.toQVariant());
            }
        }

        /*
         * Settings did change
         */
        void CSettingsComponent::ps_changedSettings(uint typeValue)
        {
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
            this->reloadSettings();
            Q_UNUSED(type);
        }

        /*
         * Textboxes from server
         */
        void CSettingsComponent::ps_updateGuiSelectedServerTextboxes(const CServer &server)
        {
            this->ui->le_SettingsTnCsName->setText(server.getName());
            this->ui->le_SettingsTnCsDescription->setText(server.getDescription());
            this->ui->le_SettingsTnCsAddress->setText(server.getAddress());
            this->ui->le_SettingsTnCsPort->setText(QString::number(server.getPort()));
            this->ui->le_SettingsTnCsRealName->setText(server.getUser().getRealName());
            this->ui->le_SettingsTnCsNetworkId->setText(server.getUser().getId());
            this->ui->le_SettingsTnCsPassword->setText(server.getUser().getPassword());
        }

        /*
         * Server settings from textboxes
         */
        CServer CSettingsComponent::ps_selectedServerFromTextboxes() const
        {
            CServer server;
            bool portOk = false;
            server.setName(this->ui->le_SettingsTnCsName->text());
            server.setDescription(this->ui->le_SettingsTnCsDescription->text());
            server.setAddress(this->ui->le_SettingsTnCsAddress->text());
            server.setPort(this->ui->le_SettingsTnCsPort->text().toInt(&portOk));
            if (!portOk) server.setPort(-1);

            CUser user;
            user.setRealName(this->ui->le_SettingsTnCsRealName->text());
            user.setId(this->ui->le_SettingsTnCsNetworkId->text());
            user.setPassword(this->ui->le_SettingsTnCsPassword->text());
            server.setUser(user);

            return server;
        }

        /*
         * Save the hotkeys
         */
        void CSettingsComponent::ps_saveHotkeys()
        {
            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathRoot(), IContextSettings::PathHotkeys());
            this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), this->ui->tvp_SettingsMiscHotkeys->derivedModel()->getContainer().toQVariant());
        }

        /*
         * Clear particular hotkey
         */
        void CSettingsComponent::ps_clearHotkey()
        {
            QModelIndex i = this->ui->tvp_SettingsMiscHotkeys->currentIndex();
            if (i.row() < 0 || i.row() >= this->ui->tvp_SettingsMiscHotkeys->rowCount()) return;
            CSettingKeyboardHotkey hotkey = this->ui->tvp_SettingsMiscHotkeys->at<CSettingKeyboardHotkey>(i);
            CSettingKeyboardHotkey defaultHotkey;
            defaultHotkey.setFunction(hotkey.getFunction());
            this->ui->tvp_SettingsMiscHotkeys->derivedModel()->update(i, defaultHotkey);
        }

        /*
         * Font has been changed
         */
        void CSettingsComponent::ps_fontChanged()
        {
            QString fontSize = this->ui->cb_SettingsGuiFontSize->currentText().append("pt");
            QString fontFamily = this->ui->cb_SettingsGuiFont->currentFont().family();
            QString fontStyleCombined = this->ui->cb_SettingsGuiFontStyle->currentText();
            QString fontColor = this->m_fontColor.name();
            if (!this->m_fontColor.isValid() || this->m_fontColor.name().isEmpty())
            {
                fontColor = CStyleSheetUtility::instance().fontColor();
            }
            this->ui->le_SettingsGuiFontColor->setText(fontColor);
            bool ok = CStyleSheetUtility::instance().updateFonts(fontFamily, fontSize, CStyleSheetUtility::fontStyle(fontStyleCombined), CStyleSheetUtility::fontWeight(fontStyleCombined), fontColor);
            if (ok)
            {
                CLogMessage(this).info("Updated font style");
            }
            else
            {
                CLogMessage(this).info("Updating style failed");
            }
        }

        /*
         * Font color dialog
         */
        void CSettingsComponent::ps_fontColorDialog()
        {
            QColor c =  QColorDialog::getColor(this->m_fontColor, this, "Font color");
            if (c == this->m_fontColor) return;
            this->m_fontColor = c;
            this->ui->le_SettingsGuiFontColor->setText(this->m_fontColor.name());
            this->ps_fontChanged();
        }
    }
} // namespace
