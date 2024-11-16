// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRCRAFTMODELSETVALIDATIONDIALOG_H
#define SWIFT_GUI_COMPONENTS_AIRCRAFTMODELSETVALIDATIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "gui/swiftguiexport.h"

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
    } // namespace simulation
} // namespace swift::misc
namespace swift::gui::components
{
    /*!
     * Dialog for CAircraftModelValidationComponent
     * \remark aircraft model view has a simplified version swift::gui::views::CAircraftModelValidationDialog
     */
    class SWIFT_GUI_EXPORT CAircraftModelSetValidationDialog : public QDialog
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
} // namespace swift::gui::components

#endif // guard
