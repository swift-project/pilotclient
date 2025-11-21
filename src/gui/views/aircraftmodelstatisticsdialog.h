// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEW_AIRCRAFTMODELSTATISTICSDIALOG_H
#define SWIFT_GUI_VIEW_AIRCRAFTMODELSTATISTICSDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/simulation/aircraftmodellist.h"

namespace Ui
{
    class CAircraftModelStatisticsDialog;
}
namespace swift::gui::views
{
    //! Info about the models
    class CAircraftModelStatisticsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelStatisticsDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CAircraftModelStatisticsDialog() override;

        //! Set and analyze the models
        void analyzeModels(const swift::misc::simulation::CAircraftModelList &models);

    private:
        //! Display the HTML matrix
        void displayHTMLMatrix();

        QScopedPointer<Ui::CAircraftModelStatisticsDialog> ui;
        swift::misc::simulation::CAircraftModelList m_models;
    };
} // namespace swift::gui::views

#endif // SWIFT_GUI_VIEW_AIRCRAFTMODELSTATISTICSDIALOG_H
