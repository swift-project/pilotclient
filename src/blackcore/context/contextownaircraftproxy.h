/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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
            //! \publicsection
            //! @{
            virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;
            virtual BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const override;
            virtual BlackMisc::Aviation::CTransponder getOwnTransponder() const override;
            virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const override;
            virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude) override;
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateTransponderMode(const BlackMisc::Aviation::CTransponder::TransponderMode &transponderMode, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override;
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;
            virtual void toggleTransponderMode() override;
            virtual bool setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) override;
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            //! @}

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
