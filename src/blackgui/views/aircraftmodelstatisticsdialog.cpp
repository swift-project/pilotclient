/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelstatisticsdialog.h"
#include "ui_aircraftmodelstatisticsdialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
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
            CDistributorList distributorList(distributors.keys());
            distributorList.sortByKey();
            for (const CDistributor &distributor : std::as_const(distributorList))
            {
                const int c = distributors[distributor];
                if (c < 1) { continue; }
                distributorsForAxis << (distributor.getDbKey() % u" (" % QString::number(c) % u")");
                series << c;
            }

            QwtText title("distributor");
            title.setRenderFlags(Qt::AlignLeft);
            ui->qwt_Chart->setSymbols("distributors", "white");
            ui->qwt_Chart->setTitle(title);
            ui->qwt_Chart->setSamples1Bar(series, distributorsForAxis, this->getOrientation());
        }

        void CAircraftModelStatisticsDialog::chartAircraftIcao()
        {
            const QMap<CAircraftIcaoCode, int> icaos = m_models.countPerAircraftIcao();
            QStringList icaosForAxis;

            QList<double> series;
            int unknown = 0;
            CAircraftIcaoCodeList icaoList(icaos.keys());
            icaoList.sortByDesignatorAndRank();

            for (const CAircraftIcaoCode &icao : std::as_const(icaoList))
            {
                const int c = icaos[icao];
                if (c < 1) { continue; }
                if (!icao.hasKnownDesignator()) { unknown += c; continue; }
                icaosForAxis << (icao.getDesignatorDbKey() % u" (" % QString::number(c) % u")");
                series << c;
            }

            QwtText title("aircraft ICAO");
            title.setRenderFlags(Qt::AlignLeft);
            ui->qwt_Chart->setSymbols("aircraft", "white");
            ui->qwt_Chart->setTitle(title);
            ui->qwt_Chart->setSamples1Bar(series, icaosForAxis, this->getOrientation());
        }

        Qt::Orientation CAircraftModelStatisticsDialog::getOrientation() const
        {
            return ui->rb_Vertical->isChecked() ? Qt::Vertical : Qt::Horizontal;
        }

    } // ns
} // ns
