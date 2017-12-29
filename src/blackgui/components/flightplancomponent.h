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

#include "blackgui/overlaymessagesframe.h"
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

namespace Ui { class CFlightPlanComponent; }
namespace BlackMisc { namespace Simulation { class CSimulatedAircraft; } }
namespace BlackGui
{
    namespace Components
    {
        //! Flight plan widget
        class BLACKGUI_EXPORT CFlightPlanComponent : public COverlayMessagesTabWidget
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

            //! Call \sa buildRemarksString from combo box signal
            void currentTextChangedToBuildRemarks(const QString &text) { this->buildRemarksString(); Q_UNUSED(text); }

            //! Send flightplan
            void sendFlightPlan();

            //! Reset Flightplan
            void resetFlightPlan();

            //! Load from disk
            void loadFromDisk();

            //! Save to disk
            void saveToDisk();

            //! Set SELCAL in own aircrafr
            void setSelcalInOwnAircraft();

            //! Load Flightplan
            void loadFlightPlanFromNetwork();

            //! Validate Flightplan
            void validateFlightPlan();

            //! Remarks
            void buildRemarksString();

            //! Copy over
            void copyRemarksConfirmed() { this->copyRemarks(true); }

            //! Copy over
            void copyRemarks(bool confirm = true);

            //! Show generator tab page
            void currentTabGenerator();

            //! GUI init complete
            void swiftWebDataRead();

            //! Build "H/B737/F"
            void buildPrefixIcaoSuffix();

            //! Prefix check box changed
            void prefixCheckBoxChanged();

            //! Aircraft type changed
            void aircraftTypeChanged();

            //! Something like "H/B737/F"
            QString getPrefixIcaoSuffix() const;

            //! Aircraft type as ICAO code
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCode() const;

            //! Show tab of equipment codes
            void showEquipmentCodesTab();

            //! VFR rules?
            bool isVfr() const;

            //! Override remarks message dialog
            bool overrideRemarks();

            //! Guess some FP values/setting
            void anticipateValues();
        };
    } // ns
} // ns
#endif // guard
