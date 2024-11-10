// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_AIRCRAFTMODELVALIDATIONDIALOG_H
#define BLACKGUI_VIEWS_AIRCRAFTMODELVALIDATIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include "misc/simulation/aircraftmodellist.h"

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
            const swift::misc::simulation::CAircraftModelList &models,
            const swift::misc::simulation::CSimulatorInfo &simulator,
            const QString &simulatorDir);

        //! Trigger a validation
        void triggerValidation(int delayMs = 2500);

    private:
        //! Validate
        void validate();

        QScopedPointer<Ui::CAircraftModelValidationDialog> ui;
        swift::misc::simulation::CAircraftModelList m_models;
        swift::misc::simulation::CSimulatorInfo m_simulator { swift::misc::simulation::CSimulatorInfo::None };
        QString m_simulatorDir;
    };
} // ns

#endif // guard
