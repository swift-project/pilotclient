// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_H
#define SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_H

#include <QObject>
#include <QString>

#include "core/context/context.h"
#include "core/corefacade.h"
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

//! \ingroup dbus
//! DBus interface for context
#define SWIFT_CORE_CONTEXTOWNAIRCRAFT_INTERFACENAME "org.swift_project.swift_core.contextownaircraft"

//! \ingroup dbus
//! DBus object path for context
#define SWIFT_CORE_CONTEXTOWNAIRCRAFT_OBJECTPATH "/ownaircraft"

class QDBusConnection;
namespace swift::misc
{
    class CDBusServer;
    namespace aviation
    {
        class CAircraftIcaoCode;
        class CAltitude;
        class CCallsign;
        class CTransponder;
    } // namespace aviation
} // namespace swift::misc
namespace swift::core::context
{
    //! Own context proxy
    //! \ingroup dbus
    class SWIFT_CORE_EXPORT IContextOwnAircraft : public IContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(SWIFT_CORE_CONTEXTOWNAIRCRAFT_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(SWIFT_CORE_CONTEXTOWNAIRCRAFT_OBJECTPATH);
            return s;
        }

        //! \copydoc IContext::getPathAndContextId()
        virtual QString getPathAndContextId() const override { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextOwnAircraft *create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                           swift::misc::CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextOwnAircraft() override {}

    signals:
        //! Aircraft cockpit update
        void changedAircraftCockpit(const swift::misc::simulation::CSimulatedAircraft &aircraft,
                                    const swift::misc::CIdentifier &originator);

        //! Changed SELCAL code
        void changedSelcal(const swift::misc::aviation::CSelcal &selcal, const swift::misc::CIdentifier &originator);

        //! Own callsign was changed
        void changedCallsign(const swift::misc::aviation::CCallsign &callsign);

        //! Own ICAO was changed
        void changedAircraftIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                      const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode);

        //! Own pilot (aka the swift user) changed
        void changedPilot(const swift::misc::network::CUser &pilot);

        //! Aircraft has been moved from one location to another (changed scenery)
        void movedAircraft(const swift::misc::physical_quantities::CLength &distance);

        //! Just airborne
        void isTakingOff();

        //! Just landed
        void isTouchingDown();

    public slots:
        //! Get own aircraft
        virtual swift::misc::simulation::CSimulatedAircraft getOwnAircraft() const = 0;

        //! Get own COM system
        virtual swift::misc::aviation::CComSystem
        getOwnComSystem(swift::misc::aviation::CComSystem::ComUnit unit) const = 0;

        //! Get own transponder
        virtual swift::misc::aviation::CTransponder getOwnTransponder() const = 0;

        //! Get own aircraft
        virtual swift::misc::aviation::CAircraftSituation getOwnAircraftSituation() const = 0;

        //! Update position
        //! \note this is in \sa IContextOwnAircraft as we want to set test positions from the GUI / elsewhere
        virtual bool updateOwnPosition(const swift::misc::geo::CCoordinateGeodetic &position,
                                       const swift::misc::aviation::CAltitude &altitude,
                                       const swift::misc::aviation::CAltitude &pressureAltitude) = 0;

        //! Update own cockpit
        virtual bool updateCockpit(const swift::misc::aviation::CComSystem &com1,
                                   const swift::misc::aviation::CComSystem &com2,
                                   const swift::misc::aviation::CTransponder &transponder,
                                   const swift::misc::CIdentifier &originator) = 0;

        //! Update own transponder mode
        virtual bool updateTransponderMode(const swift::misc::aviation::CTransponder::TransponderMode &transponderMode,
                                           const swift::misc::CIdentifier &originator) = 0;

        //! Toggle XPDR mode
        virtual void toggleTransponderMode() = 0;

        //! Set XPDR mode
        virtual bool setTransponderMode(swift::misc::aviation::CTransponder::TransponderMode mode) = 0;

        //! Tune in a COM frequency
        virtual bool updateActiveComFrequency(const swift::misc::physical_quantities::CFrequency &frequency,
                                              swift::misc::aviation::CComSystem::ComUnit comUnit,
                                              const swift::misc::CIdentifier &originator) = 0;

        //! Set current pilot
        virtual bool updateOwnAircraftPilot(const swift::misc::network::CUser &pilot) = 0;

        //! Set ICAO data
        virtual bool updateOwnIcaoCodes(const swift::misc::aviation::CAircraftIcaoCode &aircraftIcaoCode,
                                        const swift::misc::aviation::CAirlineIcaoCode &airlineIcaoCode) = 0;

        //! Set callsign
        virtual bool updateOwnCallsign(const swift::misc::aviation::CCallsign &callsign) = 0;

        //! Own SELCAL code
        virtual bool updateSelcal(const swift::misc::aviation::CSelcal &selcal,
                                  const swift::misc::CIdentifier &originator) = 0;

        //! Default situation
        //! \remark normally used when no driver is attached
        static const swift::misc::aviation::CAircraftSituation &getDefaultSituation();

        //! Default own aircraft
        //! \remark normally used when no driver is attached
        static swift::misc::simulation::CAircraftModel getDefaultOwnAircraftModel();

    protected:
        //! Constructor
        IContextOwnAircraft(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContext(mode, runtime) {}
    };
} // namespace swift::core::context

#endif // SWIFT_CORE_CONTEXT_CONTEXTOWNAIRCRAFT_H
