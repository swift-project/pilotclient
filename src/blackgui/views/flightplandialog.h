// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_VIEWS_FLIGHTPLANDIALOG_H
#define BLACKGUI_VIEWS_FLIGHTPLANDIALOG_H

#include "blackmisc/aviation/callsign.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CFlightPlanDialog;
}
namespace BlackGui::Views
{
    //! Flight plan as dialog, also meant for other callsigns
    class CFlightPlanDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CFlightPlanDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFlightPlanDialog() override;

        //! Show a particular callsign flight plan
        void showFlightPlan(const BlackMisc::Aviation::CCallsign &callsign);

    private:
        //! Title
        void setDialogTitle(const BlackMisc::Aviation::CCallsign &callsign);

        //! Load FP
        void loadFp();

        QScopedPointer<Ui::CFlightPlanDialog> ui;
    };

} // ns

#endif // guard
