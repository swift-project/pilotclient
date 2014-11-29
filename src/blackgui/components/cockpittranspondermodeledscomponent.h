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

#include "enableforruntime.h"
#include "../led.h"
#include "blackmisc/avaircraft.h"
#include "blackmisc/aviotransponder.h"
#include <QFrame>

namespace BlackGui
{
    namespace Components
    {

        //! LEDs representing transponder mode state
        class CCockpitTransponderModeLedsComponent : public QFrame, public CEnableForRuntime
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
            void ps_onAircraftCockpitChanged(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

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

            //! Identifies sender of cockpit updates
            static const QString &ledsOriginator();

            QScopedPointer<BlackGui::CLedWidget> m_ledStandby;
            QScopedPointer<BlackGui::CLedWidget> m_ledIdent;
            QScopedPointer<BlackGui::CLedWidget> m_ledModes;

        };

    } // namespace
} // namespace

#endif // guard
