// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBOWNMODELSETDIALOG_H
#define SWIFT_GUI_COMPONENTS_DBOWNMODELSETDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CDbOwnModelSetDialog;
}
namespace swift::gui::components
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
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Direct access to CDbOwnModelSetComponent
        const CDbOwnModelSetComponent *modelSetComponent() const;

        //! Direct access to CDbOwnModelSetComponent
        CDbOwnModelSetComponent *modelSetComponent();

        //! Enable buttons
        void enableButtons(bool firstSet, bool newSet);

    private:
        QScopedPointer<Ui::CDbOwnModelSetDialog> ui;
    };
} // namespace swift::gui::components
#endif // guard
