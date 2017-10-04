/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_FLIGHTPLANCOMPONENT_H
#define BLACKGUI_COMPONENTS_FLIGHTPLANCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/user.h"
#include "blackmisc/identifier.h"
#include "blackmisc/statusmessagelist.h"

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTabWidget>
#include <QtGlobal>
#include <QFileDialog>

class QWidget;

namespace Ui { class CFlightPlanComponent; }
namespace BlackMisc { namespace Simulation { class CSimulatedAircraft; } }
namespace BlackGui
{
    namespace Components
    {
        //! Flight plan widget
        class BLACKGUI_EXPORT CFlightPlanComponent : public QTabWidget
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CFlightPlanComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CFlightPlanComponent();

        public slots:
            //! Login data were set
            void loginDataSet();

            //! Prefill with aircraft data
            void prefillWithAircraftData(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            //! Prefill with user data
            void prefillWithUserData(const BlackMisc::Network::CUser &user);

            //! Prefill with aircraft dara
            void fillWithFlightPlanData(const BlackMisc::Aviation::CFlightPlan &flightPlan);

            //! Get this flight plan
            BlackMisc::Aviation::CFlightPlan getFlightPlan() const;

        private:
            QScopedPointer<Ui::CFlightPlanComponent> ui;

            //! My flight plan
            BlackMisc::Aviation::CFlightPlan m_flightPlan;

            //! Validate, generates status messages
            BlackMisc::CStatusMessageList validateAndInitializeFlightPlan(BlackMisc::Aviation::CFlightPlan &fligtPlan);

            //! Flightplan identifier
            BlackMisc::CIdentifier m_identifier;

            //! Default value for airport ICAO airports
            static const QString &defaultIcao() { static const QString d("ICAO"); return d; }

            //! Default value for time
            static const QString &defaultTime() { static const  QString t("00:00"); return t; }

            //! Identifier
            BlackMisc::CIdentifier flightPlanIdentifier();

            //! Set completers
            void initCompleters();

            //! File name for load/save
            QString getDefaultFilename(bool load);

        private slots:
            //! Call \sa ps_buildRemarksString from combo box signal
            void ps_currentTextChangedToBuildRemarks(const QString &text) { this->ps_buildRemarksString(); Q_UNUSED(text); }

            //! Send flightplan
            void ps_sendFlightPlan();

            //! Reset Flightplan
            void ps_resetFlightPlan();

            //! Load from disk
            void ps_loadFromDisk();

            //! Save to disk
            void ps_saveToDisk();

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

            //! GUI init complete
            void ps_swiftWebDataRead();
        };
    } // ns
} // ns
#endif // guard
