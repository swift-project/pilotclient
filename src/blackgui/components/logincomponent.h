/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LOGINCOMPONENT_H
#define BLACKGUI_LOGINCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/enableforruntime.h"
#include "blackcore/data/globalsetup.h"
#include "blackcore/settings/network.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/entityflags.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CLoginComponent; }

namespace BlackGui
{
    namespace Components
    {
        //! Login component
        class BLACKGUI_EXPORT CLoginComponent :
            public QFrame,
            public CEnableForRuntime
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLoginComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CLoginComponent();

        signals:
            //! Login
            void loginOrLogoffSuccessful();

            //! Cancelled
            void loginOrLogoffCancelled();

            //! Request network settings
            void requestNetworkSettings();

        public slots:
            //! Main info area chnaged
            void mainInfoAreaChanged(const QWidget *currentWidget);

        protected:
            //! \copydoc CEnableForRuntime::runtimeHasBeenSet
            virtual void runtimeHasBeenSet() override;

        private slots:
            //! Login cancelled
            void ps_loginCancelled();

            //! Login requested
            void ps_toggleNetworkConnection();

            //! VATSIM data file was loaded
            void ps_onWebServiceDataRead(int entity, int state, int number);

            //! Validate aircaft
            bool ps_validateAircraftValues();

            //! Validate VATSIM credentials
            bool ps_validateVatsimValues();

            //! Settings have been changed
            void ps_reloadSettings();

            //! Logoff countdown
            void ps_logoffCountdown();

            //! Reverse lookup model
            void ps_reverseLookupModel();

        private:
            //! GUI aircraft values, formatted
            struct CGuiAircraftValues
            {
                QString ownCallsign;
                QString ownAircraftIcaoTypeDesignator;
                QString ownAircraftIcaoAirline;
                QString ownAircraftCombinedType;
                QString ownAircraftSimulatorModel;
            };

            //! VATSIM login data
            struct CVatsimValues
            {
                QString vatsimId;
                QString vatsimPassword;
                QString vatsimRealName;
                QString vatsimHomeAirport;
            };

            //! GUI values from aircraft
            void setGuiValuesFromAircraft(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            //! Load from settings
            void loadFromSettings();

            //! Values from GUI
            CGuiAircraftValues getAircraftValuesFromGui() const;

            //! Values from GUI
            CVatsimValues getVatsimValuesFromGui() const;

            //! User from VATSIM data
            BlackMisc::Network::CUser getUserFromVatsimGuiValues() const;

            //! Callsign from GUI
            BlackMisc::Aviation::CCallsign getCallsignFromGui() const;

            //! Selected server (VATSIM)
            BlackMisc::Network::CServer getCurrentVatsimServer() const;

            //! Selected server (others)
            BlackMisc::Network::CServer getCurrentOtherServer() const;

            //! Set OK button string
            void setOkButtonString(bool connected);

            //! Show/hide elements as appropriate
            void setGuiVisibility(bool connected);

            //! Logoff countdown
            void startLogoffTimerCountdown();

            //! Own model string
            void setOwnModel();

            //! Set ICAO values
            void setGuiIcaoValues(const BlackMisc::Simulation::CAircraftModel &model, bool onlyIfEmpty);

            bool m_visible = false; //!< is this component selected?
            QScopedPointer<Ui::CLoginComponent> ui;
            const int LogoffIntervalSeconds = 10;
            QTimer *m_logoffCountdownTimer { nullptr };
            BlackMisc::CSetting<BlackCore::Settings::Network::TrafficServers> m_otherTrafficNetworkServers { this, &CLoginComponent::ps_reloadSettings };
            BlackMisc::CData<BlackCore::Data::GlobalSetup> m_setup {this}; //!< setup cache
        };

    } // namespace
} // namespace

#endif // guard
