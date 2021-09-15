/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "flightplandialog.h"
#include "ui_flightplandialog.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetwork.h"
#include "blackmisc/aviation/flightplan.h"

using namespace BlackMisc::Aviation;
using namespace BlackCore::Context;
using namespace BlackGui::Components;

namespace BlackGui::Views
{
    CFlightPlanDialog::CFlightPlanDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CFlightPlanDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        connect(ui->pb_LoadFlightPlan, &QPushButton::clicked, this, &CFlightPlanDialog::loadFp);
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEnteredDigest, this, &CFlightPlanDialog::loadFp);
    }

    CFlightPlanDialog::~CFlightPlanDialog()
    { }

    void CFlightPlanDialog::showFlightPlan(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }
        const CFlightPlan fp = sGui->getIContextNetwork()->loadFlightPlanFromNetwork(callsign);
        ui->te_FlightPlan->setText(fp.asHTML(true));
        ui->comp_CallsignCompleter->setCallsign(callsign);
        this->setDialogTitle(callsign);
        this->exec();
    }

    void CFlightPlanDialog::setDialogTitle(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { this->setWindowTitle("Flight plan"); return; }
        this->setWindowTitle("Flight plan for " + callsign.asString());
    }

    void CFlightPlanDialog::loadFp()
    {
        const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
        this->showFlightPlan(cs);
    }

} // ns
