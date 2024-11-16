// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRPORTDIALOG_H
#define SWIFT_GUI_COMPONENTS_AIRPORTDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "misc/aviation/airport.h"

namespace Ui
{
    class CAirportDialog;
}
namespace swift::gui::components
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
        void setAirport(const swift::misc::aviation::CAirport &airport);

        //! Get airport
        swift::misc::aviation::CAirport getAirport() const;

        //! Clear the form
        void clear();

    private:
        QScopedPointer<Ui::CAirportDialog> ui;
    };
} // namespace swift::gui::components

#endif // guard
