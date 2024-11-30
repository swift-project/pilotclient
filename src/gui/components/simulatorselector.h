// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SIMULATORSELECTOR_H
#define SWIFT_GUI_COMPONENTS_SIMULATORSELECTOR_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"
#include "misc/digestsignal.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CSimulatorSelector;
}
namespace swift::gui::components
{
    /*!
     * Select simulator (as radio buttons or checkboxes)
     */
    class SWIFT_GUI_EXPORT CSimulatorSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! How to display
        enum Mode
        {
            CheckBoxes, //!< multiple selections
            RadioButtons, //!< single
            ComboBox //!< single
        };

        //! Constructor
        explicit CSimulatorSelector(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CSimulatorSelector() override;

        //! How to display
        void setMode(Mode mode, bool forced = false);

        //! Get mode
        Mode getMode() { return m_mode; }

        //! No selection treated same as all selected (filters)
        void setNoSelectionMeansAll(bool v) { m_noSelectionMeansAll = v; }

        //! Get the value
        swift::misc::simulation::CSimulatorInfo getValue() const;

        //! Set the value
        void setValue(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Set to last selection
        void setToLastSelection();

        //! Set to the connected simulator
        void setToConnectedSimulator(bool makeReadOnly = true);

        //! Set to the connected simulator but deferred
        void setToConnectedSimulator(int deferredMs, bool makeReadOnly = true);

        //! Only show FSX/P3D
        void setFsxP3DOnly();

        //! Enable FG
        void enableFG(bool enabled);

        //! Set all, only making sense with checkboxes
        void checkAll();

        //! Unset all, only making sense with checkboxes
        void uncheckAll();

        //! Not selected at all
        bool isUnselected() const;

        //! All selected
        bool areAllSelected() const;

        //! Set left margin
        void setLeftMargin(int margin);

        //! Remember selection
        void setRememberSelection(bool remember) { m_rememberSelection = remember; }

        //! Remember selection
        void setRememberSelectionAndSetToLastSelection();

        //! Is rembering selection?
        bool isRememberingSelection() const { return m_rememberSelection; }

        //! Clear values
        void clear();

        //! Single selection mode (radio buttons)
        bool isSingleSelection() const;

        //! Set read only
        void setReadOnly(bool readOnly);

    signals:
        //! Value has been changed
        void changed(const swift::misc::simulation::CSimulatorInfo &simulator);

    private:
        //! Radio button changed
        void radioButtonChanged(bool checked);

        //! Checkbox changed
        void checkBoxChanged(bool checked);

        //! ComboBox has been changed
        void comboBoxChanged(const QString &value);

        //! Remember last selection
        void rememberSelection();

        //! Last selection has been changed
        void changedLastSelection();

        //! Last selection has been changed
        void changedLastSelectionRb();

        //! Last selection has been changed
        void changedLastSelectionCb();

        //! Trigger CSimulatorSelector::setToLastSelection
        void triggerSetToLastSelection();

        //! Emit the CSimulatorSelector::changed signal
        void emitChangedSignal();

        //! Add all combobox values
        void addComboxBoxValues();

        QScopedPointer<Ui::CSimulatorSelector> ui;
        Mode m_mode = CheckBoxes;
        bool m_noSelectionMeansAll = false; //!< for filters, no selection means all
        bool m_rememberSelection = false; //!< remember last selection
        misc::CDigestSignal m_digestButtonsChanged { this, &CSimulatorSelector::emitChangedSignal,
                                                     std::chrono::milliseconds(250), 3 };
        swift::misc::CData<swift::misc::simulation::data::TSimulatorLastSelection> m_currentSimulator {
            this, &CSimulatorSelector::changedLastSelectionRb
        }; //!< current simulator (used with radio buttons)
        swift::misc::CData<swift::misc::simulation::data::TSimulatorLastSelections> m_currentSimulators {
            this, &CSimulatorSelector::changedLastSelectionCb
        }; //!< current simulators (used with multiple checkboxes)
    };
} // namespace swift::gui::components

#endif // guard
