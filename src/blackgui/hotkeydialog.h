/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_HOTKEYDIALOG_H
#define BLACKGUI_HOTKEYDIALOG_H

#include "blackgui/models/actionmodel.h"
#include "blackcore/input_manager.h"
#include "blackmisc/input/hotkeycombination.h"
#include "blackmisc/input/actionhotkeylist.h"
#include "blackmisc/identifierlist.h"
#include <QDialog>
#include <QComboBox>
#include <QItemSelection>

namespace Ui
{
    class CHotkeyDialog;
}

namespace BlackGui
{

    /*!
     * Combobox for selecting keyboard keys
     */
    class CKeySelectionBox : public QComboBox
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

    private slots:
        void ps_updateSelectedIndex(int index);

    private:
        int m_oldIndex;
    };

    /*!
     * Hotkey dialog
     */
    class CHotkeyDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        CHotkeyDialog(const BlackMisc::Input::CActionHotkey &actionHotkey, QWidget *parent = nullptr);

        //! Destructor
        ~CHotkeyDialog();

        //! Get hotkey selected by user
        BlackMisc::Input::CActionHotkey getSelectedActionHotkey() const { return m_actionHotkey; }

        //! Set registered applications
        void setRegisteredApplications(const BlackMisc::CIdentifierList &applications);

        //! Init style sheet
        void initStyleSheet();

        //! getHotkey runs the hotkey dialog and returns the result
        static BlackMisc::Input::CActionHotkey getActionHotkey(const BlackMisc::Input::CActionHotkey &initial, const BlackMisc::CIdentifierList &applications,
                QWidget *parent = nullptr);

    private:
        void ps_advancedModeChanged();
        void ps_selectHotkey();
        void ps_combinationSelectionChanged(const BlackMisc::Input::CHotkeyCombination &combination);
        void ps_combinationSelectionFinished(const BlackMisc::Input::CHotkeyCombination &combination);
        void ps_changeSelectedAction(const QItemSelection &selected, const QItemSelection &deselected);
        void ps_accept();

        void synchronize();
        void synchronizeSimpleSelection();
        void synchronizeAdvancedSelection();
        void setupAdvancedFrame();
        void clearAdvancedFrame();
        void advancedKeyChanged(int oldIndex, int newIndex);
        void selectAction();

        QScopedPointer<Ui::CHotkeyDialog> ui;
        BlackMisc::Input::CActionHotkey m_actionHotkey;
        BlackGui::Models::CActionModel m_actionModel;
        BlackCore::CInputManager *m_inputManager;
    };

}

#endif
