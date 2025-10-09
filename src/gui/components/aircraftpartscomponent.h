// SPDX-FileCopyrightText: Copyright (C) 2020 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AIRCRAFTPARTSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AIRCRAFTPARTSCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>

#include "misc/aviation/callsign.h"

namespace Ui
{
    class CAircraftPartsComponent;
}
namespace swift::gui::components
{
    //! Allows to display and manipulate parts
    class CAircraftPartsComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CAircraftPartsComponent(QWidget *parent = nullptr);

        //! Dtor
        ~CAircraftPartsComponent() override;

        //! Set selected callsign
        void setCallsign(const swift::misc::aviation::CCallsign &callsign);

    signals:
        //! Currently used callsign
        void callsignChanged(const swift::misc::aviation::CCallsign &callsign);

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
} // namespace swift::gui::components

#endif // SWIFT_GUI_COMPONENTS_AIRCRAFTPARTSCOMPONENT_H
