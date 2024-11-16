// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SETTINGSHOTKEYCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>
#include <QWizardPage>

#include "core/actionbind.h"
#include "core/application/applicationsettings.h"
#include "gui/models/actionhotkeylistmodel.h"
#include "gui/swiftguiexport.h"
#include "misc/icons.h"
#include "misc/identifierlist.h"
#include "misc/settingscache.h"

namespace Ui
{
    class CSettingsHotkeyComponent;
}
namespace swift::gui::components
{
    //! Configure hotkeys
    class SWIFT_GUI_EXPORT CSettingsHotkeyComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        CSettingsHotkeyComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSettingsHotkeyComponent() override;

        //! Save settings
        void saveSettings();

        //! Create dummy/emtpy Ptt entry for wizard
        void registerDummyPttEntry();

        //! Reload keys from settings
        void reloadHotkeysFromSettings();

    private:
        void addEntry();
        void editEntry();
        void removeEntry();
        void hotkeySlot(bool keyDown);
        void addHotkeyToSettings(const swift::misc::input::CActionHotkey &actionHotkey);
        void updateHotkeyInSettings(const swift::misc::input::CActionHotkey &oldValue, const swift::misc::input::CActionHotkey &newValue);
        void removeHotkeyFromSettings(const swift::misc::input::CActionHotkey &actionHotkey);
        bool checkAndConfirmConflicts(const swift::misc::input::CActionHotkey &actionHotkey, const swift::misc::input::CActionHotkeyList &ignore = {});
        swift::misc::CIdentifierList getAllIdentifiers() const;
        void resizeView();

        QScopedPointer<Ui::CSettingsHotkeyComponent> ui;
        models::CActionHotkeyListModel m_model; //!< hotkeys model
        swift::misc::CSetting<swift::core::application::TActionHotkeys> m_actionHotkeys { this, &CSettingsHotkeyComponent::reloadHotkeysFromSettings };
        swift::core::CActionBind m_action { "/Test/Message", swift::misc::CIcons::StandardIconWrench16, this, &CSettingsHotkeyComponent::hotkeySlot };
    };

    /*!
     * Wizard page for CConfigSimulatorComponent
     */
    class CConfigHotkeyWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CSettingsHotkeyComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

        //! \copydoc QWizardPage::initializePage
        virtual void initializePage() override;

    private:
        CSettingsHotkeyComponent *m_config = nullptr;
    };
} // namespace swift::gui::components

#endif // guard
