// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_IMPL_H
#define SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_IMPL_H

#include <atomic>

#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>

#include "core/actionbind.h"
#include "core/context/contextownaircraft.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/icons.h"
#include "misc/identifiable.h"
#include "misc/identifier.h"
#include "misc/input/actionhotkeydefs.h"
#include "misc/network/settings/serversettings.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/settingscache.h"
#include "misc/simplecommandparser.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/simulatedaircraft.h"

// clazy:excludeall=const-signal-or-slot

namespace swift::misc
{
    class CDBusServer;
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CAircraftSituation;
        class CAltitude;
        class CCallsign;
        class CTransponder;
    } // namespace aviation
} // namespace swift::misc
namespace swift::core
{
    class CCoreFacade;
    namespace context
    {
        //! Own aircraft context implementation.
        //! Central instance of data for \sa IOwnAircraftProvider .
        class SWIFT_CORE_EXPORT CContextOwnAircraft :
            public IContextOwnAircraft,
            public swift::misc::simulation::IOwnAircraftProvider,
            public swift::misc::CIdentifiable
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)
            Q_INTERFACES(swift::misc::simulation::IOwnAircraftProvider)
            Q_INTERFACES(swift::misc::IProvider)
            friend class swift::core::CCoreFacade;
            friend class IContextOwnAircraft;

        public:
            //! Destructor
            virtual ~CContextOwnAircraft() override;

            // IOwnAircraftProvider overrides
            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::getOwnCallsign
            //! \ingroup ownaircraftprovider
            virtual swift::misc::aviation::CCallsign getOwnCallsign() const override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::getOwnAircraftPosition
            //! \ingroup ownaircraftprovider
            virtual swift::misc::geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::getOwnAircraftParts
            //! \ingroup ownaircraftprovider
            virtual swift::misc::aviation::CAircraftParts getOwnAircraftParts() const override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::getOwnAircraftModel
            //! \ingroup ownaircraftprovider
            virtual swift::misc::simulation::CAircraftModel getOwnAircraftModel() const override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::getDistanceToOwnAircraft
            //! \ingroup ownaircraftprovider
            virtual swift::misc::physical_quantities::CLength
            getDistanceToOwnAircraft(const swift::misc::geo::ICoordinateGeodetic &position) const override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::updateOwnModel
            //! \ingroup ownaircraftprovider
            //! \remark perform reverse lookup if possible
            virtual bool updateOwnModel(const swift::misc::simulation::CAircraftModel &model) override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::updateOwnSituation
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnSituation(const swift::misc::aviation::CAircraftSituation &situation) override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::updateOwnParts
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnParts(const swift::misc::aviation::CAircraftParts &parts) override;

            //! \copydoc swift::misc::simulation::IOwnAircraftProvider::updateOwnParts
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnCG(const swift::misc::physical_quantities::CLength &cg) override;

            //! \copydoc swift::misc::IProvider::asQObject
            virtual QObject *asQObject() override { return this; }

        signals:
            //! Changed aircraft model
            //! \private Use ISimulatorContext::ownAircraftModelChanged
            //! \remark used for cross context updates
            void ps_changedModel(const swift::misc::simulation::CAircraftModel &model,
                                 const swift::misc::CIdentifier &identifier);

        public slots:
            //! \copydoc IContextOwnAircraft::getOwnAircraft()
            //! \ingroup ownaircraftprovider
            virtual swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc IContextOwnAircraft::getOwnComSystem
            //! \ingroup ownaircraftprovider
            virtual swift::misc::aviation::CComSystem
            getOwnComSystem(swift::misc::aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc IContextOwnAircraft::getOwnTransponder()
            //! \ingroup ownaircraftprovider
            virtual swift::misc::aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc IContextOwnAircraft::getOwnAircraftSituation()
            //! \ingroup ownaircraftprovider
            virtual swift::misc::aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc IContextOwnAircraft::updateOwnCallsign
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnCallsign(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
            //! \ingroup ownaircraftprovider
            virtual bool updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                            const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc IContextOwnAircraft::updateOwnPosition
            virtual bool updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position,
                                           const swift::misc::aviation::CAltitude &altitude,
                                           const swift::misc::aviation::CAltitude &pressureAltitude) override;

            //! \copydoc IContextOwnAircraft::updateCockpit
            virtual bool updateCockpit(const swift::misc::aviation::CComSystem &com1,
                                       const swift::misc::aviation::CComSystem &com2,
                                       const swift::misc::aviation::CTransponder &transponder,
                                       const swift::misc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateTransponderMode
            virtual bool
            updateTransponderMode(const swift::misc::aviation::CTransponder::TransponderMode &transponderMode,
                                  const swift::misc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateSelcal
            virtual bool updateSelcal(const swift::misc::aviation::CSelcal &selcal,
                                      const swift::misc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                                  swift::misc::aviation::CComSystem::ComUnit comUnit,
                                                  const swift::misc::CIdentifier &originator) override;

            //! \copydoc IContextOwnAircraft::updateOwnAircraftPilot
            virtual bool updateOwnAircraftPilot(const swift::misc::network::CUser &pilot) override;

            //! \copydoc IContextOwnAircraft::toggleTransponderMode
            virtual void toggleTransponderMode() override;

            //! \copydoc IContextOwnAircraft::setTransponderMode
            virtual bool setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode) override;

            //! \ingroup swiftdotcommands
            //! <pre>
            //! .x    .xpdr  code       set transponder code    swift::core::context::CContextOwnAircraft
            //! .x    .xpdr  mode       set transponder mode    swift::core::context::CContextOwnAircraft
            //! .com1 .com2  frequency  set COM1/2 frequency    swift::core::context::CContextOwnAircraft
            //! .selcal      code       set SELCAL code         swift::core::context::CContextOwnAircraft
            //! </pre>
            //! \copydoc IContextOwnAircraft::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

            //! Register help
            static void registerHelp()
            {
                if (swift::misc::CSimpleCommandParser::registered("swift::core::context::CContextOwnAircraft"))
                {
                    return;
                }
                swift::misc::CSimpleCommandParser::registerCommand({ ".x", "alias: .xpdr" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".x code|mode", "set XPDR code or mode" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".selcal code", "set SELCAL code" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".com1", "alias .c1" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".com1 frequency", "set COM1 frequency" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".com2 frequency", "set COM2 frequency" });
            }

        protected:
            //! Constructor, with link to runtime
            CContextOwnAircraft(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextOwnAircraft *registerWithDBus(swift::misc::CDBusServer *server);

        private:
            swift::misc::simulation::CSimulatedAircraft m_ownAircraft; //!< my aircraft
            mutable QReadWriteLock m_lockAircraft; //!< lock aircraft

            CActionBind m_actionToggleXpdr { swift::misc::input::toggleXPDRStateHotkeyAction(),
                                             swift::misc::input::toggleXPDRStateHotkeyIcon(), this,
                                             &CContextOwnAircraft::actionToggleTransponder };
            CActionBind m_actionIdent { swift::misc::input::toggleXPDRIdentHotkeyAction(),
                                        swift::misc::input::toggleXPDRIdentHotkeyIcon(), this,
                                        &CContextOwnAircraft::actionIdent };

            static constexpr qint64 MinHistoryDeltaMs = 1000;
            static constexpr int MaxHistoryElements = 20;
            QTimer m_historyTimer; //!< history timer
            std::atomic_bool m_history { true }; //!< enable history
            swift::misc::aviation::CAircraftSituationList m_situationHistory; //!< history, latest situation first

            swift::misc::CSetting<swift::misc::network::settings::TCurrentTrafficServer> m_currentNetworkServer {
                this
            };

            //! Simulator model has been changed
            //! \ingroup crosscontextfunction
            void xCtxChangedSimulatorModel(const swift::misc::simulation::CAircraftModel &model,
                                           const swift::misc::CIdentifier &identifier);

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
            bool updateOwnModel(const swift::misc::simulation::CAircraftModel &model,
                                const swift::misc::CIdentifier &identifier);

            //! Reverse lookup of the model against DB data
            static swift::misc::simulation::CAircraftModel
            reverseLookupModel(const swift::misc::simulation::CAircraftModel &model);
        };
    } // namespace context
} // namespace swift::core
#endif // SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_IMPL_H
