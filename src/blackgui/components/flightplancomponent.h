/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_FLIGHTPLANCOMPONENT_H
#define BLACKGUI_FLIGHTPLANCOMPONENT_H

#include "blackgui/components/enableforruntime.h"
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/aviation/flightplan.h"

#include <QTabWidget>

namespace Ui { class CFlightPlanComponent; }
namespace BlackGui
{
    namespace Components
    {

        //! Flight plan widget
        class CFlightPlanComponent :
            public QTabWidget,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CFlightPlanComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CFlightPlanComponent();

        public slots:
            //! Login data were set
            void loginDataSet();

            //! Prefill with aircraft data
            void prefillWithAircraftData(const BlackMisc::Aviation::CAircraft &ownAircraft);

            //! Prefill with aircraft dara
            void fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan);

            //! Get this flight plan
            BlackMisc::Aviation::CFlightPlan getFlightPlan() const;

        protected:
            //! \copydoc CEnableForRuntime::runtimeHasBeenSet
            void runtimeHasBeenSet() override;

        private:
            QScopedPointer<Ui::CFlightPlanComponent> ui;

            //! My flight plan
            BlackMisc::Aviation::CFlightPlan m_flightPlan;

            //! Validate, generates status messages
            BlackMisc::CStatusMessageList validateAndInitializeFlightPlan(BlackMisc::Aviation::CFlightPlan &fligtPlan);

            //! Default value for airport ICAO airports
            static const QString &defaultIcao() { static QString d("ICAO"); return d; }

            //! Default value for time
            static const QString &defaultTime() { static QString t("00:00"); return t; }

            //! Originator
            static const QString &flightPlanOriginator();

        private slots:
            //! Call \sa ps_buildRemarksString from combo box signal
            void ps_currentTextChangedToBuildRemarks(const QString &text) { this->ps_buildRemarksString(); Q_UNUSED(text); }

            //! Send flightplan
            void ps_sendFlightPlan();

            //! Reset Flightplan
            void ps_resetFlightPlan();

            //! Set SELCAL in own aircrafr
            void ps_setSelcalInOwnAircraft();

            //! Load Flightplan
            void ps_loadFlightPlanFromNetwork();

            //! Validate Flightplan
            void ps_validateFlightPlan();

            //! Remark
            void ps_buildRemarksString();

            //! Copy over
            void ps_copyRemarks();

            //! Show generator tab page
            void ps_currentTabGenerator();

        };
    }
}
#endif // guard
