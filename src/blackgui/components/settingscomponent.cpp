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

        void CSettingsComponent::setGuiOpacity(double value)
        {
            this->ui->hs_SettingsGuiOpacity->setValue(value);
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

        /*
         * Reload settings
         */
        void CSettingsComponent::reloadSettings()
        {
            // reload components
            this->ui->comp_AudioSetup->reloadSettings();
            this->ui->comp_SettingsServersComponent->reloadSettings();

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
            Q_ASSERT_X(this->getIContextSettings(), "runtimeHasBeenSet", "Missing settings");
            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsComponent::ps_changedSettings);

            // Opacity, intervals
            this->connect(this->ui->hs_SettingsGuiOpacity, &QSlider::valueChanged, this, &CSettingsComponent::changedWindowsOpacity);
            this->connect(this->ui->hs_SettingsGuiAircraftRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAircraftUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiAtcRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedAtcStationsUpdateInterval);
            this->connect(this->ui->hs_SettingsGuiUserRefreshTime, &QSlider::valueChanged, this, &CSettingsComponent::changedUsersUpdateInterval);

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
            bool connected = this->connect(this->ui->cb_SettingsGuiFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontSize, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            connected = this->connect(this->ui->cb_SettingsGuiFontStyle, SIGNAL(currentIndexChanged(QString)), this, SLOT(ps_fontChanged()));
            Q_ASSERT(connected);
            this->connect(this->ui->tb_SettingsGuiFontColor, &QToolButton::clicked, this, &CSettingsComponent::ps_fontColorDialog);
            Q_UNUSED(connected);
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
         * Save the hotkeys
         */
        void CSettingsComponent::ps_saveHotkeys()
        {
            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathRoot(), IContextSettings::PathHotkeys());
            this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), this->ui->tvp_SettingsMiscHotkeys->derivedModel()->getContainer().toCVariant());
        }

        /*
         * Clear particular hotkey
         */
        void CSettingsComponent::ps_clearHotkey()
        {
            QModelIndex i = this->ui->tvp_SettingsMiscHotkeys->currentIndex();
            if (i.row() < 0 || i.row() >= this->ui->tvp_SettingsMiscHotkeys->rowCount()) return;
            CSettingKeyboardHotkey hotkey = this->ui->tvp_SettingsMiscHotkeys->at(i);
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
