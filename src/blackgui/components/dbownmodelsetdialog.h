/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSETDIALOG_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSETDIALOG_H

#include "blackmisc/simulation/simulatorinfo.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CDbOwnModelSetDialog;
}
namespace BlackGui::Components
{
    class CDbOwnModelSetComponent;

    //! Dialog version of
    class CDbOwnModelSetDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CDbOwnModelSetDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbOwnModelSetDialog() override;

        //! \copydoc CDbOwnModelSetComponent::setSimulator
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Direct access to CDbOwnModelSetComponent
        const CDbOwnModelSetComponent *modelSetComponent() const;

        //! Direct access to CDbOwnModelSetComponent
        CDbOwnModelSetComponent *modelSetComponent();

        //! Enable buttons
        void enableButtons(bool firstSet, bool newSet);

    private:
        QScopedPointer<Ui::CDbOwnModelSetDialog> ui;
    };
} // ns
#endif // guard
