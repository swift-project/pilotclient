// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRCRAFTMODELSETVALIDATIONDIALOG_H
#define BLACKGUI_COMPONENTS_AIRCRAFTMODELSETVALIDATIONDIALOG_H

#include "blackgui/blackguiexport.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAircraftModelSetValidationDialog;
}
namespace swift::misc
{
    class CStatusMessageList;
    namespace simulation
    {
        class CSimulatorInfo;
        class CAircraftModelList;
    }
}
namespace BlackGui::Components
{
    /*!
     * Dialog for CAircraftModelValidationComponent
     * \remark aircraft model view has a simplified version BlackGui::Views::CAircraftModelValidationDialog
     */
    class BLACKGUI_EXPORT CAircraftModelSetValidationDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelSetValidationDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelSetValidationDialog() override;

        //! \copydoc CAircraftModelValidationComponent::validatedModelSet
        void validatedModelSet(const swift::misc::simulation::CSimulatorInfo &simulator, const swift::misc::simulation::CAircraftModelList &valid, const swift::misc::simulation::CAircraftModelList &invalid, bool stopped, const swift::misc::CStatusMessageList &msgs);

    protected:
        //! \copydoc QObject::event
        virtual bool event(QEvent *event) override;

    private:
        QScopedPointer<Ui::CAircraftModelSetValidationDialog> ui;
    };
} // ns

#endif // guard
