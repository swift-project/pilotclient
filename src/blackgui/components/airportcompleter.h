/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRPORTCOMPLETER_H
#define BLACKGUI_COMPONENTS_AIRPORTCOMPLETER_H

#include "blackmisc/aviation/airport.h"
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
        void setAirport(const BlackMisc::Aviation::CAirport &airport);

        //! Get airport
        const BlackMisc::Aviation::CAirport &getAirport() const { return m_current; }

        //! The raw ICAO code text
        QString getIcaoText() const;

        //! Read only
        void setReadOnly(bool readOnly);

        //! Clear the form
        void clear();

    signals:
        //! Airport has been changed
        void changedAirport(const BlackMisc::Aviation::CAirport &airport);

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
        BlackMisc::Aviation::CAirport m_current; //!< current airport
    };
} // ns

#endif // guard
