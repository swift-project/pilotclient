/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_SIMULATORSELECTOR_H
#define BLACKGUI_SIMULATORSELECTOR_H

#include "blackmisc/simulation/simulatorinfo.h"
#include <QScopedPointer>
#include <QFrame>

namespace Ui { class CSimulatorSelector; }

namespace BlackGui
{
    /*!
     * Select simulator
     */
    class CSimulatorSelector : public QFrame
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
        ~CSimulatorSelector();

        //! How to display
        void setMode(Mode mode);

        //! Get the value
        BlackMisc::Simulation::CSimulatorInfo getValue() const;

        //! Set the value
        void setValue(const BlackMisc::Simulation::CSimulatorInfo &info);

        //! Set all, only making sense with checkboxes
        void setAll();

    private:
        QScopedPointer<Ui::CSimulatorSelector> ui;
        Mode m_mode = CheckBoxes;
    };
}

#endif // guard
