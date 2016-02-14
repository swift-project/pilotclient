/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H
#define BLACKGUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H

#include "blackgui/models/actionhotkeylistmodel.h"
#include "blackcore/settings/application.h"
#include "blackcore/actionbind.h"
#include <QFrame>

namespace Ui {
    class CSettingsHotkeyComponent;
}

namespace BlackGui
{
    namespace Components
    {

        //! Configure hotkeys
        class BLACKGUI_EXPORT CSettingsHotkeyComponent :
            public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            CSettingsHotkeyComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CSettingsHotkeyComponent();

        private slots:
            void ps_addEntry();
            void ps_editEntry();
            void ps_removeEntry();

        private:
            void addHotkeytoSettings(const BlackMisc::Input::CActionHotkey &actionHotkey);
            void updateHotkeyInSettings(const BlackMisc::Input::CActionHotkey &oldValue, const BlackMisc::Input::CActionHotkey &newValue);
            void removeHotkeyFromSettings(const BlackMisc::Input::CActionHotkey &actionHotkey);
            bool checkAndConfirmConflicts(const BlackMisc::Input::CActionHotkey &actionHotkey, const BlackMisc::Input::CActionHotkeyList &ignore = {});

            QScopedPointer<Ui::CSettingsHotkeyComponent> ui;
            BlackGui::Models::CActionHotkeyListModel m_model;
            BlackMisc::CSetting<BlackCore::Settings::Application::ActionHotkeys> m_actionHotkeys { this };
            BlackCore::CActionBind m_action { "/Test/Message", this, &CSettingsHotkeyComponent::ps_hotkeySlot };

            void ps_hotkeySlot(bool keyDown);
        };

    } // ns
} // ns

#endif // guard
