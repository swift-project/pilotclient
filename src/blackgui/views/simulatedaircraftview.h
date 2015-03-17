/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SIMULATEDAIRCRAFTVIEW_H
#define BLACKGUI_SIMULATEDAIRCRAFTVIEW_H

#include "viewbase.h"
#include "../models/simulatedaircraftlistmodel.h"
#include <QMenu>

namespace BlackGui
{
    namespace Views
    {
        //! Aircrafts view
        class CSimulatedAircraftView : public CViewBase<Models::CSimulatedAircraftListModel, BlackMisc::Simulation::CSimulatedAircraftList, BlackMisc::Simulation::CSimulatedAircraft>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CSimulatedAircraftView(QWidget *parent = nullptr);

            //! Mode
            void setAircraftMode(Models::CSimulatedAircraftListModel::AircraftMode mode);

        signals:
            //! Request a text message
            void requestTextMessageWidget(const BlackMisc::Aviation::CCallsign &callsign);

            //! Request enable / disable fast position updates
            void requestFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Enable aircraft
            void requestEnableAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(QMenu &menu) const override;

        private slots:
            void ps_requestTextMessage();
            void ps_enableAircraft();
            void ps_fastPositionUpdates();

        };
    } // ns
} // ns
#endif // guard
