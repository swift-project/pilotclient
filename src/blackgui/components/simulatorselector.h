/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_SIMULATORSELECTOR_H
#define BLACKGUI_COMPONENTS_SIMULATORSELECTOR_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/data/modelcaches.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

class QWidget;

namespace Ui { class CSimulatorSelector; }
namespace BlackGui
{
    namespace Components
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
                CheckBoxes,
                RadioButtons
            };

            //! Constructor
            explicit CSimulatorSelector(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CSimulatorSelector();

            //! How to display
            void setMode(Mode mode);

            //! No selection treated same as all selected (filters)
            void setNoSelectionMeansAll(bool v) { this->m_noSelectionMeansAll = v; }

            //! Get the value
            BlackMisc::Simulation::CSimulatorInfo getValue() const;

            //! Set the value
            void setValue(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Set to last selection
            void setToLastSelection();

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

            //! Is rembering selection?
            bool isRememberingSelection() const { return m_rememberSelection; }

            //! Clear values
            void clear();

        signals:
            //! Value has been changed
            void changed(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private:
            //! Radio button changed
            void radioButtonChanged(bool checked);

            //! Checkbox changed
            void checkBoxChanged(bool checked);

            //! Remember last selection
            void rememberSelection();

            //! Last selection has been changed
            void changedLastSelection();

            //! Last selection has been changed
            void changedLastSelectionRb();

            //! Last selection has been changed
            void changedLastSelectionCb();

            QScopedPointer<Ui::CSimulatorSelector> ui;
            Mode m_mode = CheckBoxes;
            bool m_noSelectionMeansAll = false; //!< for filters, no selection means all
            bool m_rememberSelection = false;   //!< remember last selection
            BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelection>  m_currentSimulator  { this, &CSimulatorSelector::changedLastSelectionRb }; //!< current simulator (used with radio buttons)
            BlackMisc::CData<BlackMisc::Simulation::Data::TSimulatorLastSelections> m_currentSimulators { this, &CSimulatorSelector::changedLastSelectionCb }; //!< current simulators (used with multiple checkboxes)
        };
    } // ns
} // ns

#endif // guard
