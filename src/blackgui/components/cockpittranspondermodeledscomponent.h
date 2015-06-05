/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H
#define BLACKMISC_COCKPITTRANSPONDERMODELEDSCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "enableforruntime.h"
#include "../led.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/aviation/transponder.h"
#include <QFrame>

namespace BlackGui
{
    namespace Components
    {

        //! LEDs representing transponder mode state
        class BLACKGUI_EXPORT CCockpitTransponderModeLedsComponent :
            public QFrame,
            public CEnableForRuntime,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CCockpitTransponderModeLedsComponent(QWidget *parent = nullptr);

        protected:
            //! \copydoc CEnableForRuntime::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:
            //! \copydoc IContextOwnAircraft::changedAircraftCockpit
            void ps_onAircraftCockpitChanged(const BlackMisc::Aviation::CAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! LED clicked
            void ps_onLedClicked();

        private:
            const int LedWidth = 14; //!< LED width

            //! Init either in horizontal or vertical layout
            void init(bool horizontal);

            //! Set the mode
            void setMode(BlackMisc::Aviation::CTransponder::TransponderMode mode);

            //! Own Transponder
            BlackMisc::Aviation::CTransponder getOwnTransponder() const;

            //! Own Aircraft
            BlackMisc::Aviation::CAircraft getOwnAircraft() const;

            QScopedPointer<BlackGui::CLedWidget> m_ledStandby;
            QScopedPointer<BlackGui::CLedWidget> m_ledModes;
            QScopedPointer<BlackGui::CLedWidget> m_ledIdent;
        };
    } // namespace
} // namespace

#endif // guard
