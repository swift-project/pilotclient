// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRPORTSMALLCOMPLETER_H
#define SWIFT_GUI_COMPONENTS_AIRPORTSMALLCOMPLETER_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/airport.h"

namespace Ui
{
    class CAirportSmallCompleter;
}
namespace swift::gui::components
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
        ~CAirportSmallCompleter() override;

        //! Set airport
        void setAirport(const swift::misc::aviation::CAirport &airport);

        //! Set airport
        void setAirportIcaoCode(const swift::misc::aviation::CAirportIcaoCode &airportCode);

        //! Get airport
        const swift::misc::aviation::CAirport &getAirport() const { return m_current; }

        //! Get airport ICAO code
        swift::misc::aviation::CAirportIcaoCode getAirportIcaoCode() const;

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
        void changedAirport(const swift::misc::aviation::CAirport &airport);

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
        swift::misc::aviation::CAirport m_current; //!< this airport
    };
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AIRPORTSMALLCOMPLETER_H
