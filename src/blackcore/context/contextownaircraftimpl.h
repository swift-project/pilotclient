// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_IMPL_H

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/actionbind.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/network/settings/serversettings.h"
#include "blackmisc/network/user.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/input/actionhotkeydefs.h"
#include "blackmisc/icons.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"
#include "blackmisc/simplecommandparser.h"

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <atomic>

// clazy:excludeall=const-signal-or-slot

namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CAircraftSituation;
        class CAltitude;
        class CCallsign;
        class CTransponder;
    }
}
namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Own aircraft context implementation.
        //! Central instance of data for \sa IOwnAircraftProvider .
        class BLACKCORE_EXPORT CContextOwnAircraft :
            public IContextOwnAircraft,
            public BlackMisc::Simulation::IOwnAircraftProvider,
            public BlackMisc::CIdentifiable
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)
            Q_INTERFACES(BlackMisc::Simulation::IOwnAircraftProvider)
            Q_INTERFACES(BlackMisc::IProvider)
            friend class BlackCore::CCoreFacade;
            friend class IContextOwnAircraft;

        public:
            //! Destructor
            virtual ~CContextOwnAircraft() override;

            // IOwnAircraftProvider overrides
            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::getOwnCallsign
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Aviation::CCallsign getOwnCallsign() const override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::getOwnAircraftPosition
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::getOwnAircraftParts
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::getOwnAircraftModel
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::getDistanceToOwnAircraft
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::updateOwnModel
            //! \ingroup ownaircraftprovider
            //! \remark perform reverse lookup if possible
            virtual bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model) override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::updateOwnSituation
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::updateOwnParts
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts) override;

            //! \copydoc BlackMisc::Simulation::IOwnAircraftProvider::updateOwnParts
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnCG(const BlackMisc::PhysicalQuantities::CLength &cg) override;

            //! \copydoc BlackMisc::IProvider::asQObject
            virtual QObject *asQObject() override { return this; }

        signals:
            //! Changed aircraft model
            //! \private Use ISimulatorContext::ownAircraftModelChanged
            //! \remark used for cross context updates
            void ps_changedModel(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &identifier);

        public slots:
            //! \copydoc IContextOwnAircraft::getOwnAircraft()
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc IContextOwnAircraft::getOwnComSystem
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc IContextOwnAircraft::getOwnTransponder()
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc IContextOwnAircraft::getOwnAircraftSituation()
            //! \ingroup ownaircraftprovider
            virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc IContextOwnAircraft::updateOwnCallsign
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc IContextOwnAircraft::updateOwnPosition
            virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude) override;

            //! \copydoc IContextOwnAircraft::updateCockpit
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateTransponderMode
            virtual bool updateTransponderMode(const BlackMisc::Aviation::CTransponder::TransponderMode &transponderMode, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateSelcal
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateOwnAircraftPilot
            virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override;

            //! \copydoc IContextOwnAircraft::toggleTransponderMode
            virtual void toggleTransponderMode() override;

            //! \copydoc IContextOwnAircraft::setTransponderMode
            virtual bool setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) override;

            //! \ingroup swiftdotcommands
            //! <pre>
            //! .x    .xpdr  code       set transponder code    BlackCore::Context::CContextOwnAircraft
            //! .x    .xpdr  mode       set transponder mode    BlackCore::Context::CContextOwnAircraft
            //! .com1 .com2  frequency  set COM1/2 frequency    BlackCore::Context::CContextOwnAircraft
            //! .selcal      code       set SELCAL code         BlackCore::Context::CContextOwnAircraft
            //! </pre>
            //! \copydoc IContextOwnAircraft::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! Register help
            static void registerHelp()
            {
                if (BlackMisc::CSimpleCommandParser::registered("BlackCore::Context::CContextOwnAircraft")) { return; }
                BlackMisc::CSimpleCommandParser::registerCommand({ ".x", "alias: .xpdr" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".x code|mode", "set XPDR code or mode" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".selcal code", "set SELCAL code" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".com1", "alias .c1" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".com1 frequency", "set COM1 frequency" });
                BlackMisc::CSimpleCommandParser::registerCommand({ ".com2 frequency", "set COM2 frequency" });
            }

        protected:
            //! Constructor, with link to runtime
            CContextOwnAircraft(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextOwnAircraft *registerWithDBus(BlackMisc::CDBusServer *server);

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft; //!< my aircraft
            mutable QReadWriteLock m_lockAircraft; //!< lock aircraft

            CActionBind m_actionToggleXpdr { BlackMisc::Input::toggleXPDRStateHotkeyAction(), BlackMisc::Input::toggleXPDRStateHotkeyIcon(), this, &CContextOwnAircraft::actionToggleTransponder };
            CActionBind m_actionIdent { BlackMisc::Input::toggleXPDRIdentHotkeyAction(), BlackMisc::Input::toggleXPDRIdentHotkeyIcon(), this, &CContextOwnAircraft::actionIdent };

            static constexpr qint64 MinHistoryDeltaMs = 1000;
            static constexpr int MaxHistoryElements = 20;
            QTimer m_historyTimer; //!< history timer
            std::atomic_bool m_history { true }; //!< enable history
            BlackMisc::Aviation::CAircraftSituationList m_situationHistory; //!< history, latest situation first

            BlackMisc::Aviation::CComSystem m_lastEvaluatedCom1;
            BlackMisc::Aviation::CComSystem m_lastEvaluatedCom2;

            BlackMisc::CSetting<BlackMisc::Network::Settings::TCurrentTrafficServer> m_currentNetworkServer { this };

            //! Station has disconnected
            //! \ingroup crosscontextfunction
            void xCtxAtcStationDisconnected(const BlackMisc::Aviation::CAtcStation &atcStation);

            //! Simulator model has been changed
            //! \ingroup crosscontextfunction
            void xCtxChangedSimulatorModel(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &identifier);

            //! Simulator status changed
            //! \ingroup crosscontextfunction
            void xCtxChangedSimulatorStatus(int status);

            //! @{
            //! Actions
            void actionToggleTransponder(bool keydown);
            void actionIdent(bool keydown);
            //! @}

            //! Web data loaded
            void allSwiftWebDataRead();

            //! Init my very own aircraft with some defaults, before overridden by simulator
            void initOwnAircraft();

            //! Update position history
            void evaluateUpdateHistory();

            //! Update own model and emit signal with identifier
            bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &identifier);

            //! Evaluate COM stations
            void evaluateComStations(bool atcChanged);

            //! Reverse lookup of the model against DB data
            static BlackMisc::Simulation::CAircraftModel reverseLookupModel(const BlackMisc::Simulation::CAircraftModel &model);
        };
    } // ns
} // ns
#endif // guard
