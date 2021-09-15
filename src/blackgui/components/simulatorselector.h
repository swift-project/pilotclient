/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SIMULATORSELECTOR_H
#define BLACKGUI_COMPONENTS_SIMULATORSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/digestsignal.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CSimulatorSelector; }
namespace BlackGui::Components
{
    /*!
     * Select simulator (as radio buttons or checkboxes)
     */
    class BLACKGUI_EXPORT CSimulatorSelector : public QFrame
    {
        Q_OBJECT

    public:
        //! How to display
        enum Mode
        {
            CheckBoxes,    //!< multiple selections
            RadioButtons,  //!< single
            ComboBox       //!< single
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
        BlackMisc::Simulation::CSimulatorInfo getValue() const;

        //! Set the value
        void setValue(const BlackMisc::Simulation::CSimulatorInfo &simulator);

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
        void changed(const BlackMisc::Simulation::CSimulatorInfo &simulator);

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
        bool m_rememberSelection   = false; //!< remember last selection
        BlackMisc::CDigestSignal m_digestButtonsChanged { this, &CSimulatorSelector::emitChangedSignal, 250, 3 };
        BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelection>  m_currentSimulator  { this, &CSimulatorSelector::changedLastSelectionRb }; //!< current simulator (used with radio buttons)
        BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelections> m_currentSimulators { this, &CSimulatorSelector::changedLastSelectionCb }; //!< current simulators (used with multiple checkboxes)
    };
} // ns

#endif // guard
