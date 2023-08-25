// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_CONTEXT_OWNAIRCRAFT_PROXY_H
#define BLACKCORE_CONTEXT_OWNAIRCRAFT_PROXY_H

#include <QObject>
#include <QString>

#include "blackcore/context/contextownaircraft.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"

// clazy:excludeall=const-signal-or-slot

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Aviation
    {
        class CAircraftIcaoCode;
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
        //! Own aircraft context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextOwnAircraftProxy : public IContextOwnAircraft
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
            //! \copydoc BlackCore::Context::IContextOwnAircraft::getOwnAircraft
            virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::getOwnComSystem
            virtual BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::getOwnTransponder
            virtual BlackMisc::Aviation::CTransponder getOwnTransponder() const override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::getOwnAircraftSituation
            virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateOwnPosition
            virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateCockpit
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateTransponderMode
            virtual bool updateTransponderMode(const BlackMisc::Aviation::CTransponder::TransponderMode &transponderMode, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateActiveComFrequency
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateOwnAircraftPilot
            virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateSelcal
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateOwnCallsign
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::updateOwnIcaoCodes
            virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::toggleTransponderMode
            virtual void toggleTransponderMode() override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::setTransponderMode
            virtual bool setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) override;

            //! \copydoc BlackCore::Context::IContextOwnAircraft::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

        protected:
            //! \brief Constructor
            CContextOwnAircraftProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr) {}

            //! \brief DBus version constructor
            CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface; //!< DBus interface */

            //! \brief Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);
        };
    } // ns
} // ns
#endif // guard
