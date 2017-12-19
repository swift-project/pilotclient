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

#include "blackgui/blackguiexport.h"
#include "blackgui/models/actionhotkeylistmodel.h"
#include "blackcore/actionbind.h"
#include "blackcore/application/applicationsettings.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/icons.h"

#include <QFrame>
#include <QObject>
#include <QWizardPage>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CSettingsHotkeyComponent; }
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
            virtual ~CSettingsHotkeyComponent();

            //! Save settings
            void saveSettings();

            //! Create dummy/emtpy Ptt entry for wizard
            void registerDummyPttEntry();

        private:
            void addEntry();
            void editEntry();
            void removeEntry();
            void hotkeySlot(bool keyDown);
            void addHotkeytoSettings(const BlackMisc::Input::CActionHotkey &actionHotkey);
            void updateHotkeyInSettings(const BlackMisc::Input::CActionHotkey &oldValue, const BlackMisc::Input::CActionHotkey &newValue);
            void removeHotkeyFromSettings(const BlackMisc::Input::CActionHotkey &actionHotkey);
            bool checkAndConfirmConflicts(const BlackMisc::Input::CActionHotkey &actionHotkey, const BlackMisc::Input::CActionHotkeyList &ignore = {});
            void reloadHotkeysFromSettings();

            QScopedPointer<Ui::CSettingsHotkeyComponent> ui;
            BlackGui::Models::CActionHotkeyListModel m_model;
            BlackMisc::CSetting<BlackCore::Application::TActionHotkeys> m_actionHotkeys { this };
            BlackCore::CActionBind m_action { "/Test/Message", BlackMisc::CIcons::wrench16(), this, &CSettingsHotkeyComponent::hotkeySlot };
        };

        /**
         * Wizard page for CConfigSimulatorComponent
         */
        class CConfigHotkeyWizardPage : public QWizardPage
        {
        public:
            //! Constructors
            using QWizardPage::QWizardPage;

            //! Set config
            void setConfigComponent(CSettingsHotkeyComponent *config) { m_config = config; }

            //! \copydoc QWizardPage::validatePage
            virtual bool validatePage() override;

        private:
            CSettingsHotkeyComponent *m_config = nullptr;
        };
    } // ns
} // ns

#endif // guard
