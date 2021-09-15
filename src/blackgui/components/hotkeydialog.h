/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_HOTKEYDIALOG_H
#define BLACKGUI_COMPONENTS_HOTKEYDIALOG_H

#include "blackgui/models/actionmodel.h"
#include "blackgui/horizontalcombobox.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/input/actionhotkey.h"
#include "blackmisc/identifierlist.h"

#include <QDialog>
#include <QObject>
#include <QScopedPointer>

class QItemSelection;

namespace Ui { class CHotkeyDialog; }
namespace BlackCore { class CInputManager; }
namespace BlackMisc::Input
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
            const BlackMisc::Input::CActionHotkey &actionHotkey,
            const BlackMisc::CIdentifierList &identifiers,
            QWidget *parent = nullptr);

        //! Destructor
        virtual ~CHotkeyDialog() override;

        //! Get hotkey selected by user
        BlackMisc::Input::CActionHotkey getSelectedActionHotkey() const { return m_actionHotkey; }

        //! Init style sheet
        void initStyleSheet();

        //! \copydoc QDialog::accept
        virtual void accept() override;

        //! Runs the hotkey dialog and returns the result
        static BlackMisc::Input::CActionHotkey getActionHotkey(
            const BlackMisc::Input::CActionHotkey &initial,
            const BlackMisc::CIdentifierList &identifiers,
            QWidget *parent = nullptr);

    private:
        void advancedModeChanged();
        void captureHotkey();
        void combinationSelectionChanged(const BlackMisc::Input::CHotkeyCombination &combination);
        void combinationSelectionFinished(const BlackMisc::Input::CHotkeyCombination &combination);
        void changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected);
        CKeySelectionBox *addSelectionBox(const BlackMisc::Input::CKeyboardKeyList &allSupportedKeys, const BlackMisc::Input::CKeyboardKey &keyboardKey = {});
        CKeySelectionBox *addSelectionBox(const BlackMisc::Input::CJoystickButtonList &allAvailableButtons, const BlackMisc::Input::CJoystickButton &joystickButton = {});

        void changeApplicableMachine(int index);
        void synchronize();
        void synchronizeSimpleSelection();
        void synchronizeAdvancedSelection();
        void setupAdvancedFrame();
        void clearAdvancedFrame();
        void advancedKeyChanged(int oldIndex, int newIndex);
        void selectAction();

        QScopedPointer<Ui::CHotkeyDialog> ui;
        BlackMisc::Input::CActionHotkey m_actionHotkey;
        BlackGui::Models::CActionModel  m_actionModel;

        //! "No key/button"
        static const QString &noKeyButton();
    };
} // ns

#endif
