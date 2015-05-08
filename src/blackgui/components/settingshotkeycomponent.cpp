/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingshotkeycomponent.h"
#include "ui_settingshotkeycomponent.h"
#include "blackcore/context_settings.h"
#include "blackmisc/settingutilities.h"

using namespace BlackCore;
using namespace BlackMisc::Settings;

namespace BlackGui
{
    namespace Components
    {

        CSettingsHotkeyComponent::CSettingsHotkeyComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsHotkeyComponent)
        {
            ui->setupUi(this);
        }

        CSettingsHotkeyComponent::~CSettingsHotkeyComponent() { }

        void CSettingsHotkeyComponent::runtimeHasBeenSet()
        {
            Q_ASSERT_X(this->getIContextSettings(), Q_FUNC_INFO, "Missing settings");
            this->connect(this->getIContextSettings(), &IContextSettings::changedSettings, this, &CSettingsHotkeyComponent::ps_changedSettings);

            // Settings hotkeys
            this->connect(this->ui->pb_SettingsCancel, &QPushButton::clicked, this, &CSettingsHotkeyComponent::reloadSettings);
            this->connect(this->ui->pb_SettingsSave, &QPushButton::clicked, this, &CSettingsHotkeyComponent::ps_saveHotkeys);
            this->connect(this->ui->pb_SettingsRemove, &QPushButton::clicked, this, &CSettingsHotkeyComponent::ps_clearHotkey);
        }

        void CSettingsHotkeyComponent::reloadSettings()
        {
            // update hot keys
            this->ui->tvp_SettingsMiscHotkeys->updateContainer(this->getIContextSettings()->getHotkeys());
        }

        void CSettingsHotkeyComponent::ps_changedSettings(uint typeValue)
        {
            IContextSettings::SettingsType type = static_cast<IContextSettings::SettingsType>(typeValue);
            this->reloadSettings();
            Q_UNUSED(type);
        }

        void CSettingsHotkeyComponent::ps_saveHotkeys()
        {
            const QString path = CSettingUtilities::appendPaths(IContextSettings::PathRoot(), IContextSettings::PathHotkeys());
            this->getIContextSettings()->value(path, CSettingUtilities::CmdUpdate(), this->ui->tvp_SettingsMiscHotkeys->derivedModel()->getContainer().toCVariant());
        }

        void CSettingsHotkeyComponent::ps_clearHotkey()
        {
            QModelIndex i = this->ui->tvp_SettingsMiscHotkeys->currentIndex();
            if (i.row() < 0 || i.row() >= this->ui->tvp_SettingsMiscHotkeys->rowCount()) return;
            CSettingKeyboardHotkey hotkey = this->ui->tvp_SettingsMiscHotkeys->at(i);
            CSettingKeyboardHotkey defaultHotkey;
            defaultHotkey.setFunction(hotkey.getFunction());
            this->ui->tvp_SettingsMiscHotkeys->derivedModel()->update(i, defaultHotkey);
        }

    } // ns
} // ns
