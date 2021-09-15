/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_H
#define BLACKCORE_CONTEXT_CONTEXTOWNAIRCRAFT_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/context.h"
#include "blackcore/corefacade.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QObject>
#include <QString>

// clazy:excludeall=const-signal-or-slot

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME "org.swift_project.blackcore.contextownaircraft"

//! DBus object path for context
#define BLACKCORE_CONTEXTOWNAIRCRAFT_OBJECTPATH "/ownaircraft"

//! @}

class QDBusConnection;
namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CAltitude;
        class CCallsign;
        class CTransponder;
    }
}
namespace BlackCore::Context
{
    //! Own context proxy
    //! \ingroup dbus
    class BLACKCORE_EXPORT IContextOwnAircraft : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTOWNAIRCRAFT_OBJECTPATH);
            return s;
        }

        //! \copydoc IContext::getPathAndContextId()
        virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextOwnAircraft *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, BlackMisc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextOwnAircraft() override {}

    signals:
        //! Aircraft cockpit update
        void changedAircraftCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! Changed SELCAL code
        void changedSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator);

        //! Own callsign was changed
        void changedCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Own ICAO was changed
        void changedAircraftIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode);

        //! Own pilot (aka the swift user) changed
        void changedPilot(const BlackMisc::Network::CUser &pilot);

        //! Aircraft has been moved from one location to another (changed scenery)
        void movedAircraft(const BlackMisc::PhysicalQuantities::CLength &distance);

        //! Just airborne
        void isTakingOff();

        //! Just landed
        void isTouchingDown();

    public slots:
        //! Get own aircraft
        virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const = 0;

        //! Get own COM system
        virtual BlackMisc::Aviation::CComSystem getOwnComSystem(BlackMisc::Aviation::CComSystem::ComUnit unit) const = 0;

        //! Get own transponder
        virtual BlackMisc::Aviation::CTransponder getOwnTransponder() const = 0;

        //! Get own aircraft
        virtual BlackMisc::Aviation::CAircraftSituation getOwnAircraftSituation() const = 0;

        //! Update position
        //! \note this is in \sa IContextOwnAircraft as we want to set test positions from the GUI / elsewhere
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const BlackMisc::Aviation::CAltitude &pressureAltitude) = 0;

        //! Update own cockpit
        virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) = 0;

        //! Update own transponder mode
        virtual bool updateTransponderMode(const BlackMisc::Aviation::CTransponder::TransponderMode &transponderMode, const BlackMisc::CIdentifier &originator) = 0;

        //! Toggle XPDR mode
        virtual void toggleTransponderMode() = 0;

        //! Set XPDR mode
        virtual bool setTransponderMode(BlackMisc::Aviation::CTransponder::TransponderMode mode) = 0;

        //! Tune in a COM frequency
        virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) = 0;

        //! Set current pilot
        virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) = 0;

        //! Set ICAO data
        virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) = 0;

        //! Set callsign
        virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        //! Own SELCAL code
        virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) = 0;

        //! Default situation
        //! \remark normally used when no driver is attached
        static const BlackMisc::Aviation::CAircraftSituation &getDefaultSituation();

        //! Default own aircraft
        //! \remark normally used when no driver is attached
        static BlackMisc::Simulation::CAircraftModel getDefaultOwnAircraftModel();

    protected:
        //! Constructor
        IContextOwnAircraft(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // ns

#endif // guard
