// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRPORTDIALOG_H
#define BLACKGUI_COMPONENTS_AIRPORTDIALOG_H

#include "blackmisc/aviation/airport.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CAirportDialog;
}
namespace BlackGui::Components
{
    //! Dialog for finding an airport
    class CAirportDialog : public QDialog
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportDialog(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirportDialog();

        //! Set airport
        void setAirport(const BlackMisc::Aviation::CAirport &airport);

        //! Get airport
        BlackMisc::Aviation::CAirport getAirport() const;

        //! Clear the form
        void clear();

    private:
        QScopedPointer<Ui::CAirportDialog> ui;
    };
} // ns

#endif // guard
