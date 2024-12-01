// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_OWNAIRCRAFT_PROXY_H
#define SWIFT_CORE_CONTEXT_OWNAIRCRAFT_PROXY_H

#include <QString>

#include "core/context/contextownaircraft.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/selcal.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/identifier.h"
#include "misc/network/user.h"
#include "misc/pq/frequency.h"
#include "misc/simulation/simulatedaircraft.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace swift::misc
{
    class CGenericDBusInterface;
    namespace aviation
    {
        class CAircraftIcaoCode;
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
        //! Own aircraft context proxy
        //! \ingroup dbus
        class SWIFT_CORE_EXPORT CContextOwnAircraftProxy : public IContextOwnAircraft
        {
            Q_OBJECT
            friend class IContextOwnAircraft;

        public:
            //! Destructor
            virtual ~CContextOwnAircraftProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \copydoc swift::core::context::IContextOwnAircraft::getOwnAircraft
            virtual swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc swift::core::context::IContextOwnAircraft::getOwnComSystem
            virtual swift::misc::aviation::CComSystem
            getOwnComSystem(swift::misc::aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc swift::core::context::IContextOwnAircraft::getOwnTransponder
            virtual swift::misc::aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc swift::core::context::IContextOwnAircraft::getOwnAircraftSituation
            virtual swift::misc::aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateOwnPosition
            virtual bool updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position,
                                           const swift::misc::aviation::CAltitude &altitude,
                                           const swift::misc::aviation::CAltitude &pressureAltitude) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateCockpit
            virtual bool updateCockpit(const swift::misc::aviation::CComSystem &com1,
                                       const swift::misc::aviation::CComSystem &com2,
                                       const swift::misc::aviation::CTransponder &transponder,
                                       const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateTransponderMode
            virtual bool
            updateTransponderMode(const swift::misc::aviation::CTransponder::TransponderMode &transponderMode,
                                  const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                                  swift::misc::aviation::CComSystem::ComUnit comUnit,
                                                  const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateOwnAircraftPilot
            virtual bool updateOwnAircraftPilot(const swift::misc::network::CUser &pilot) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateSelcal
            virtual bool updateSelcal(const swift::misc::aviation::CSelcal &selcal,
                                      const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateOwnCallsign
            virtual bool updateOwnCallsign(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::updateOwnIcaoCodes
            virtual bool updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                            const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::toggleTransponderMode
            virtual void toggleTransponderMode() override;

            //! \copydoc swift::core::context::IContextOwnAircraft::setTransponderMode
            virtual bool setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode) override;

            //! \copydoc swift::core::context::IContextOwnAircraft::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

        protected:
            //! \brief Constructor
            CContextOwnAircraftProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime)
                : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr)
            {}

            //! \brief DBus version constructor
            CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection,
                                     CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        private:
            swift::misc::CGenericDBusInterface *m_dBusInterface; //!< DBus interface */

            //! \brief Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);
        };
    } // namespace context
} // namespace swift::core
#endif // guard
