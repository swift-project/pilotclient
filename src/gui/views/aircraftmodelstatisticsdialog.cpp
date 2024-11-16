// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftmodelstatisticsdialog.h"

#include <QDesktopServices>
#include <QPushButton>
#include <QStringBuilder>
#include <QStringList>
#include <QUrl>

#include "ui_aircraftmodelstatisticsdialog.h"

#include "gui/guiapplication.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/simulation/aircraftmodelutils.h"

using namespace swift::misc::aviation;
using namespace swift::misc::simulation;

namespace swift::gui::views
{
    CAircraftModelStatisticsDialog::CAircraftModelStatisticsDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CAircraftModelStatisticsDialog)
    {
        ui->setupUi(this);
        ui->tw_ModelStatistics->setCurrentIndex(0);
        this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

        connect(ui->pb_GenerateMatrix, &QPushButton::released, this,
                &CAircraftModelStatisticsDialog::displayHTMLMatrix);
    }

    CAircraftModelStatisticsDialog::~CAircraftModelStatisticsDialog() {}

    void CAircraftModelStatisticsDialog::analyzeModels(const CAircraftModelList &models)
    {
        m_models = models;
        ui->te_GeneralStatistics->setHtml(models.htmlStatistics(true, true));
    }

    void CAircraftModelStatisticsDialog::displayHTMLMatrix()
    {
        const QString file = CAircraftModelUtilities::createIcaoAirlineAircraftHtmlMatrixFile(
            m_models, CGuiApplication::getTemporaryDirectory());
        if (file.isEmpty()) { return; }
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }
} // namespace swift::gui::views
