// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTMODELVALIDATIONDIALOG_H
#define BLACKGUI_VIEWS_AIRCRAFTMODELVALIDATIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include "blackmisc/simulation/aircraftmodellist.h"

namespace Ui
{
    class CAircraftModelValidationDialog;
}
namespace BlackGui::Views
{
    /*!
     * Model validation
     * \remark there is also a more specific validation version BlackGui::Components::CAircraftModelSetValidationDialog
     */
    class CAircraftModelValidationDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelValidationDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelValidationDialog() override;

        //! Models
        void setModels(
            const BlackMisc::Simulation::CAircraftModelList &models,
            const BlackMisc::Simulation::CSimulatorInfo &simulator,
            const QString &simulatorDir);

        //! Trigger a validation
        void triggerValidation(int delayMs = 2500);

    private:
        //! Validate
        void validate();

        QScopedPointer<Ui::CAircraftModelValidationDialog> ui;
        BlackMisc::Simulation::CAircraftModelList m_models;
        BlackMisc::Simulation::CSimulatorInfo m_simulator { BlackMisc::Simulation::CSimulatorInfo::None };
        QString m_simulatorDir;
    };
} // ns

#endif // guard
