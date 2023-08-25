// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEW_AIRCRAFTMODELSTATISTICSDIALOG_H
#define BLACKGUI_VIEW_AIRCRAFTMODELSTATISTICSDIALOG_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAircraftModelStatisticsDialog;
}
namespace QtCharts
{
    class QBarSeries;
    class QHorizontalBarSeries;
}
namespace BlackGui::Views
{
    //! Info about the models
    class CAircraftModelStatisticsDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAircraftModelStatisticsDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAircraftModelStatisticsDialog();

        //! Set and analyze the models
        void analyzeModels(const BlackMisc::Simulation::CAircraftModelList &models);

    private:
        //! Display the HTML matrix
        void displayHTMLMatrix();

        //! Chart
        void showChart();

        //! Chart for distributors
        void chartDistributors();

        //! Chart for aircraft ICAO
        void chartAircraftIcao();

        //! Orientation
        Qt::Orientation getOrientation() const;

        QScopedPointer<Ui::CAircraftModelStatisticsDialog> ui;
        BlackMisc::Simulation::CAircraftModelList m_models;
    };
} // ns

#endif // guard
