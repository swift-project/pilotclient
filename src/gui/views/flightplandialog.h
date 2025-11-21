// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_VIEWS_FLIGHTPLANDIALOG_H
#define SWIFT_GUI_VIEWS_FLIGHTPLANDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/aviation/callsign.h"

namespace Ui
{
    class CFlightPlanDialog;
}
namespace swift::gui::views
{
    //! Flight plan as dialog, also meant for other callsigns
    class CFlightPlanDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CFlightPlanDialog(QWidget *parent = nullptr);

        //! Destructor
        ~CFlightPlanDialog() override;

        //! Show a particular callsign flight plan
        void showFlightPlan(const swift::misc::aviation::CCallsign &callsign);

    private:
        //! Title
        void setDialogTitle(const swift::misc::aviation::CCallsign &callsign);

        //! Load FP
        void loadFp();

        QScopedPointer<Ui::CFlightPlanDialog> ui;
    };

} // namespace swift::gui::views

#endif // SWIFT_GUI_VIEWS_FLIGHTPLANDIALOG_H
