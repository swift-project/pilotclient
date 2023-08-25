// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H
#define BLACKMISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/led.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QFrame>
#include <QObject>
#include <QScopedPointer>

namespace BlackGui::Components
{
    //! LEDs representing transponder mode state
    class BLACKGUI_EXPORT CCockpitTransponderModeLedsComponent :
        public QFrame,
        public BlackMisc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CCockpitTransponderModeLedsComponent(QWidget *parent = nullptr);

        //! Set the mode
        void setMode(BlackMisc::Aviation::CTransponder::TransponderMode mode, bool force = false);

    private:
        static constexpr int LedWidth = 14; //!< LED width

        //! \copydoc IContextOwnAircraft::changedAircraftCockpit
        void onAircraftCockpitChanged(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! LED clicked
        void onLedClicked();

        //! Init either in horizontal or vertical layout
        void init(bool horizontal);

        //! Own Transponder
        BlackMisc::Aviation::CTransponder getOwnTransponder() const;

        //! Own Aircraft
        BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const;

        BlackMisc::Aviation::CTransponder::TransponderMode m_mode = BlackMisc::Aviation::CTransponder::StateStandby;
        QScopedPointer<BlackGui::CLedWidget> m_ledStandby;
        QScopedPointer<BlackGui::CLedWidget> m_ledModes;
        QScopedPointer<BlackGui::CLedWidget> m_ledIdent;
    };
} // namespace

#endif // guard
