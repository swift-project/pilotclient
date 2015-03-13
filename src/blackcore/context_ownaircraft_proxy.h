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

#include "blackcore/context_ownaircraft.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackmisc/avaircrafticao.h"
#include "blackmisc/simulation/simulatedaircraft.h"

namespace BlackCore
{

    //! Own aircraft context proxy
    //! \ingroup dbus
    class CContextOwnAircraftProxy : public IContextOwnAircraft
    {
        Q_OBJECT
        friend class IContextOwnAircraft;

    public:

        //! Destructor
        virtual ~CContextOwnAircraftProxy() {}

    public slots: // IContextOwnAircraft overrides

        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

        //! \copydoc IContextOwnAircraft::updatePosition
        virtual bool updatePosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;

        //! \copydoc IContextOwnAircraft::updateCockpit
        virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateComFrequency
        virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updatePilot
        virtual bool updatePilot(const BlackMisc::Network::CUser &pilot) override;

        //! \copydoc IContextOwnAircraft::updateSelcal
        virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateCallsign
        virtual bool updateCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextOwnAircraft::updateIcaoData
        virtual bool updateIcaoData(const BlackMisc::Aviation::CAircraftIcao &icaoData) override;

        //! \copydoc IContextOwnAircraft::setAudioOutputVolumes
        virtual void setAudioOutputVolume(int outputVolume) override;

        //! \copydoc IContextOwnAircraft::setAudioVoiceRoomOverrideUrls
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url);

        //! \copydoc IContextOwnAircraft::enableAutomaticVoiceRoomResolution
        virtual void enableAutomaticVoiceRoomResolution(bool enable);

        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const QString &originator) override;

    protected:
        //! \brief Constructor
        CContextOwnAircraftProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr) {}

        //! \brief DBus version constructor
        CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface; //!< DBus interface */

        //! \brief Relay connection signals to local signals.
        void relaySignals(const QString &serviceName, QDBusConnection &connection);


    };
}

#endif // guard
