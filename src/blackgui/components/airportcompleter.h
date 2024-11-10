// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRPORTCOMPLETER_H
#define BLACKGUI_COMPONENTS_AIRPORTCOMPLETER_H

#include "misc/aviation/airport.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CAirportCompleter;
}
namespace BlackGui::Components
{
    //! Airport completer, UI element so select by ICAO, name or location
    class CAirportCompleter : public QFrame
    {
        Q_OBJECT

    public:
        //! Get the correct airport
        explicit CAirportCompleter(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAirportCompleter();

        //! Set airport
        void setAirport(const swift::misc::aviation::CAirport &airport);

        //! Get airport
        const swift::misc::aviation::CAirport &getAirport() const { return m_current; }

        //! The raw ICAO code text
        QString getIcaoText() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Clear the form
        void clear();

    signals:
        //! Airport has been changed
        void changedAirport(const swift::misc::aviation::CAirport &airport);

    private:
        //! ICAO code has been changed in UI
        void onIcaoChanged();

        //! Name has been changed in UI
        void onNameChanged();

        //! Location has been changed in UI
        void onLocationChanged();

        //! Airports backend data changed
        void onAirportsChanged();

        QScopedPointer<Ui::CAirportCompleter> ui;
        swift::misc::aviation::CAirport m_current; //!< current airport
    };
} // ns

#endif // guard
