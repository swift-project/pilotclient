// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H
#define SWIFT_MISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

#include "gui/led.h"
#include "gui/swiftguiexport.h"
#include "misc/aviation/transponder.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/simulation/simulatedaircraft.h"

namespace swift::gui::components
{
    //! LEDs representing transponder mode state
    class SWIFT_GUI_EXPORT CCockpitTransponderModeLedsComponent : public QFrame, public swift::misc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitTransponderModeLedsComponent(QWidget *parent = nullptr);

        //! Set the mode
        void setMode(swift::misc::aviation::CTransponder::TransponderMode mode, bool force = false);

    private:
        static constexpr int LedWidth = 14; //!< LED width

        //! \copydoc IContextOwnAircraft::changedAircraftCockpit
        void onAircraftCockpitChanged(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                      const swift::misc::CIdentifier &originator);

        //! LED clicked
        void onLedClicked();

        //! Init either in horizontal or vertical layout
        void init(bool horizontal);

        //! Own Transponder
        swift::misc::aviation::CTransponder getOwnTransponder() const;

        //! Own Aircraft
        swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const;

        swift::misc::aviation::CTransponder::TransponderMode m_mode = swift::misc::aviation::CTransponder::StateStandby;
        QScopedPointer<swift::gui::CLedWidget> m_ledStandby;
        QScopedPointer<swift::gui::CLedWidget> m_ledModes;
        QScopedPointer<swift::gui::CLedWidget> m_ledIdent;
    };
} // namespace swift::gui::components

#endif // guard
