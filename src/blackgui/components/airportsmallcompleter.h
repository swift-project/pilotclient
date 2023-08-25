// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRPORTSMALLCOMPLETER_H
#define BLACKGUI_COMPONENTS_AIRPORTSMALLCOMPLETER_H

#include "blackmisc/aviation/airport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAirportSmallCompleter;
}
namespace BlackGui::Components
{
    class CAirportDialog;

    //! Smaller version of CAirportCompleter, using a CAirportDialog
    class CAirportSmallCompleter : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAirportSmallCompleter(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirportSmallCompleter();

        //! Set airport
        void setAirport(const BlackMisc::Aviation::CAirport &airport);

        //! Set airport
        void setAirportIcaoCode(const BlackMisc::Aviation::CAirportIcaoCode &airportCode);

        //! Get airport
        const BlackMisc::Aviation::CAirport &getAirport() const { return m_current; }

        //! Get airport ICAO code
        BlackMisc::Aviation::CAirportIcaoCode getAirportIcaoCode() const;

        //! The raw ICAO code text
        QString getIcaoText() const;

        //! Empty field
        bool isEmpty() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Clear
        void clear();

        //! Clear if code not valid
        void clearIfInvalidCode(bool strictValidation);

    signals:
        //! Airport has been changed
        void changedAirport(const BlackMisc::Aviation::CAirport &airport);

        //! \copydoc QLineEdit::editingFinished
        void editingFinished();

    private:
        //! ICAO code has been changed in UI
        void onIcaoChanged();

        //! Airports backend data changed
        void onAirportsChanged();

        //! Display dialog
        void showAirportsDialog();

        QScopedPointer<Ui::CAirportSmallCompleter> ui;
        QScopedPointer<CAirportDialog> m_airportsDialog; //!< UI completer
        BlackMisc::Aviation::CAirport m_current; //!< this airport
    };
} // ns

#endif // guard
