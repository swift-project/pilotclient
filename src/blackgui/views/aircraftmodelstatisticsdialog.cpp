// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui::Views
{
    CAircraftModelStatisticsDialog::CAircraftModelStatisticsDialog(QWidget *parent) : QDialog(parent),
                                                                                      ui(new Ui::CAircraftModelStatisticsDialog)
    {
        ui->setupUi(this);
        ui->tw_ModelStatistics->setCurrentIndex(0);
        this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        connect(ui->pb_GenerateMatrix, &QPushButton::released, this, &CAircraftModelStatisticsDialog::displayHTMLMatrix);
    }

    CAircraftModelStatisticsDialog::~CAircraftModelStatisticsDialog()
    {}

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
} // ns
