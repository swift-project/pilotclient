/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_OWNAIRCRAFT_PROXY_H
#define BLACKCORE_OWNAIRCRAFT_PROXY_H

#include <QObject>
#include <QString>

#include "blackcore/blackcoreexport.h"
#include "blackcore/contextownaircraft.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/simulation/simulatedaircraft.h"

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

    //! Own aircraft context proxy
    //! \ingroup dbus
    class BLACKCORE_EXPORT CContextOwnAircraftProxy : public IContextOwnAircraft
    {
        Q_OBJECT
        friend class IContextOwnAircraft;

    public:
        //! Destructor
        virtual ~CContextOwnAircraftProxy() {}

    public slots:
        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

        //! \copydoc IContextOwnAircraft::updateOwnPosition
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;

        //! \copydoc IContextOwnAircraft::updateCockpit
        virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updateActiveComFrequency
        virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnAircraftPilot
        virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override;

        //! \copydoc IContextOwnAircraft::updateSelcal
        virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnCallsign
        virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
        virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;

        //! \copydoc IContextOwnAircraft::setAudioOutputVolume
        virtual void setAudioOutputVolume(int outputVolume) override;

        //! \copydoc IContextOwnAircraft::setAudioVoiceRoomOverrideUrls
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url) override;

        //! \copydoc IContextOwnAircraft::enableAutomaticVoiceRoomResolution
        virtual void enableAutomaticVoiceRoomResolution(bool enable) override;

        //! \copydoc IContextOwnAircraft::parseCommandLine
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

#endif // guard
