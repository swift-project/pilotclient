/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRPORTDIALOG_H
#define BLACKGUI_COMPONENTS_AIRPORTDIALOG_H

#include "blackmisc/aviation/airport.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui { class CAirportDialog; }
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
