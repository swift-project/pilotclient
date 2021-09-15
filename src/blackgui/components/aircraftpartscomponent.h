/* Copyright (C) 2020
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_AIRCRAFTPARTSCOMPONENT_H
#define BLACKGUI_COMPONENTS_AIRCRAFTPARTSCOMPONENT_H

#include "blackmisc/aviation/callsign.h"

#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CAircraftPartsComponent; }
namespace BlackGui::Components
{
    //! Allows to display and manipulate parts
    class CAircraftPartsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAircraftPartsComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CAircraftPartsComponent() override;

        //! Set selected callsign
        void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);

    signals:
        //! Currently used callsign
        void callsignChanged(const BlackMisc::Aviation::CCallsign &callsign);

    private:
        //! Send aircraft parts
        void sendAircraftParts();

        //! Current parts in UI
        void setCurrentParts();

        //! Request parts (aka aircraft config) from network
        void requestPartsFromNetwork();

        //! Display own aircraft parts
        void displayOwnParts();

        //! Own aircraft parts in simulator
        void displayLogInSimulator();

        //! Completer has been changed
        void onCallsignChanged();

        QScopedPointer<Ui::CAircraftPartsComponent> ui;
    };
} // ns

#endif // guard
