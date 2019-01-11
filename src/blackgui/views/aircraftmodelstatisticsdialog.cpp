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
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/simulation/aircraftmodelutils.h"

#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QStringBuilder>
#include <QStringList>

using namespace QtCharts;
using namespace BlackMisc::Aviation;
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

        void CAircraftModelStatisticsDialog::showChart()
        {
            if (ui->rb_Distributors->isChecked()) { this->chartDistributors(); return; }
            if (ui->rb_AircraftIcao->isChecked()) { this->chartAircraftIcao(); return; }
        }

        void CAircraftModelStatisticsDialog::chartDistributors()
        {
            const QMap<CDistributor, int> distributors = m_models.countPerDistributor();
            QStringList distributorsForAxis;

            QList<double> series;
            for (const CDistributor &distributor : distributors.keys())
            {
                const int c = distributors[distributor];
                if (c < 1) { continue; }
                distributorsForAxis << (distributor.getDbKey() % u" (" % QString::number(c) % u")");
                series << c;
            }

            ui->qwt_Chart->setSymbols("distributors", "white");
            ui->qwt_Chart->setTitle("distributors");
            ui->qwt_Chart->setSamples1Bar(series, distributorsForAxis);
        }

        void CAircraftModelStatisticsDialog::chartAircraftIcao()
        {
            const QMap<CAircraftIcaoCode, int> icaos = m_models.countPerAircraftIcao();
            QStringList icaosForAxis;

            QList<double> series;
            int unknown = 0;
            for (const CAircraftIcaoCode &icao : icaos.keys())
            {
                const int c = icaos[icao];
                if (c < 1) { continue; }
                if (!icao.hasKnownDesignator()) { unknown += c; continue; }
                icaosForAxis << (icao.getDesignatorDbKey() % u" (" % QString::number(c) % u")");
                series << c;
            }

            ui->qwt_Chart->setSymbols("aircraft", "white");
            ui->qwt_Chart->setTitle("aircraft ICAO");
            ui->qwt_Chart->setSamples1Bar(series, icaosForAxis);
        }

    } // ns
} // ns
