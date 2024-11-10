// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_HOTKEYDIALOG_H
#define BLACKGUI_COMPONENTS_HOTKEYDIALOG_H

#include "blackgui/models/actionmodel.h"
#include "blackgui/horizontalcombobox.h"
#include "blackgui/blackguiexport.h"
#include "misc/input/actionhotkey.h"
#include "misc/identifierlist.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

class QItemSelection;

namespace Ui
{
    class CHotkeyDialog;
}
namespace swift::core
{
    class CInputManager;
}
namespace swift::misc::input
{
    class CHotkeyCombination;
    class CKeyboardKeyList;
}
namespace BlackGui::Components
{
    /*!
     * ComboBox for selecting keyboard keys
     */
    class BLACKGUI_EXPORT CKeySelectionBox : public CHorizontalComboBox
    {
        Q_OBJECT

    public:
        //! Constructor
        CKeySelectionBox(QWidget *parent = nullptr);

        //! Set key with index as selected
        void setSelectedIndex(int index);

    signals:
        //! User has changed the selection
        void keySelectionChanged(int oldIndex, int newIndex);

    private:
        void updateSelectedIndex(int index);

        int m_oldIndex; //!< last selected index
    };

    /*!
     * Hotkey dialog
     */
    class BLACKGUI_EXPORT CHotkeyDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        CHotkeyDialog(
            const swift::misc::input::CActionHotkey &actionHotkey,
            const swift::misc::CIdentifierList &identifiers,
            QWidget *parent = nullptr);

        //! Destructor
        virtual ~CHotkeyDialog() override;

        //! Get hotkey selected by user
        swift::misc::input::CActionHotkey getSelectedActionHotkey() const { return m_actionHotkey; }

        //! Init style sheet
        void initStyleSheet();

        //! \copydoc QDialog::accept
        virtual void accept() override;

        //! Runs the hotkey dialog and returns the result
        static swift::misc::input::CActionHotkey getActionHotkey(
            const swift::misc::input::CActionHotkey &initial,
            const swift::misc::CIdentifierList &identifiers,
            QWidget *parent = nullptr);

    private:
        void advancedModeChanged();
        void captureHotkey();
        void combinationSelectionChanged(const swift::misc::input::CHotkeyCombination &combination);
        void combinationSelectionFinished(const swift::misc::input::CHotkeyCombination &combination);
        void changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected);
        CKeySelectionBox *addSelectionBox(const swift::misc::input::CKeyboardKeyList &allSupportedKeys, const swift::misc::input::CKeyboardKey &keyboardKey = {});
        CKeySelectionBox *addSelectionBox(const swift::misc::input::CJoystickButtonList &allAvailableButtons, const swift::misc::input::CJoystickButton &joystickButton = {});

        void changeApplicableMachine(int index);
        void synchronize();
        void synchronizeSimpleSelection();
        void synchronizeAdvancedSelection();
        void setupAdvancedFrame();
        void clearAdvancedFrame();
        void advancedKeyChanged(int oldIndex, int newIndex);
        void selectAction();

        QScopedPointer<Ui::CHotkeyDialog> ui;
        swift::misc::input::CActionHotkey m_actionHotkey;
        BlackGui::Models::CActionModel m_actionModel;

        //! "No key/button"
        static const QString &noKeyButton();
    };
} // ns

#endif
