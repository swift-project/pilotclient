/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelstatisticsdialog.h"
#include "ui_aircraftmodelstatisticsdialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/simulation/aircraftmodelutils.h"

#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QStringList>
#include <QtCharts>

using namespace QtCharts;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelStatisticsDialog::CAircraftModelStatisticsDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAircraftModelStatisticsDialog)
        {
            ui->setupUi(this);
            ui->tw_ModelStatistics->setCurrentIndex(0);
            this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

            connect(ui->pb_GenerateMatrix, &QPushButton::released, this, &CAircraftModelStatisticsDialog::displayHTMLMatrix);
            connect(ui->pb_ShowChart, &QPushButton::released, this, &CAircraftModelStatisticsDialog::showChart);
            connect(ui->tb_ZoomIn,  &QToolButton::released, this, &CAircraftModelStatisticsDialog::zoom);
            connect(ui->tb_ZoomOut, &QToolButton::released, this, &CAircraftModelStatisticsDialog::zoom);

            this->initChart();
        }

        CAircraftModelStatisticsDialog::~CAircraftModelStatisticsDialog()
        { }

        void CAircraftModelStatisticsDialog::analyzeModels(const CAircraftModelList &models)
        {
            m_models = models;
            ui->te_GeneralStatistics->setHtml(models.htmlStatistics(true, true));
        }

        void CAircraftModelStatisticsDialog::displayHTMLMatrix()
        {
            const QString file = CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrixFile(m_models, CGuiApplication::getTemporaryDirectory());
            if (file.isEmpty()) { return; }
            QDesktopServices::openUrl(QUrl::fromLocalFile(file));
        }

        void CAircraftModelStatisticsDialog::zoom()
        {
            const QObject *sender = QObject::sender();
            QChart *chart = ui->qv_Chart->chart();
            if (sender == ui->tb_ZoomIn)  { chart->zoomIn();  return; }
            if (sender == ui->tb_ZoomOut) { chart->zoomOut(); return; }
        }

        void CAircraftModelStatisticsDialog::initChart()
        {
            QChart *chart = new QChart();
            chart->setAnimationOptions(QChart::SeriesAnimations);
            chart->legend()->setVisible(true);
            chart->legend()->setAlignment(Qt::AlignBottom);
            chart->setTheme(QChart::ChartThemeBlueIcy);
            ui->qv_Chart->setChart(chart);
            ui->qv_Chart->setRenderHint(QPainter::Antialiasing);
        }

        void CAircraftModelStatisticsDialog::resetChart()
        {
            QChart *chart = ui->qv_Chart->chart();
            chart->removeAllSeries();
            const auto axes = chart->axes();
            for (auto axis : axes)
            {
                chart->removeAxis(axis);
            }
        }

        void CAircraftModelStatisticsDialog::showChart()
        {
            if (ui->rb_Distributors->isChecked()) { this->chartDistributors(); return; }
        }

        void CAircraftModelStatisticsDialog::chartDistributors()
        {
            QChart *chart = ui->qv_Chart->chart();
            this->resetChart();
            const QMap<CDistributor, int> distributors = m_models.countPerDistributor();
            QStringList distributorsForAxis;
            QBarSet *setDistributors = new QBarSet("Distributors");

            // using number as uique key as it can happen there a identical distributor keys
            // and QChart requires uniques values
            int n = 1;
            for (const CDistributor &distributor : distributors.keys())
            {
                const int c = distributors[distributor];
                if (c < 1) { continue; }
                distributorsForAxis << QString::number(n) % u": " % distributor.getDbKey() % u" " % QString::number(c);
                *setDistributors << c;
                n++;
                // QString s += distributor.getDbKey() % u" " % QString::number(c) % "\n";
            }

            QHorizontalBarSeries *series = new QHorizontalBarSeries(this);
            series->append(setDistributors);
            chart->addSeries(series);
            // chart->setTitle("Distributors");

            // Y
            QBarCategoryAxis *axisY = new QBarCategoryAxis();
            axisY->append(distributorsForAxis);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);

            // X
            QValueAxis *axisX = new QValueAxis();
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);
            axisX->applyNiceNumbers();
        }
    } // ns
} // ns
