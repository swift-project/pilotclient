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
#include "blackmisc/avallclasses.h"

namespace BlackCore
{

    //! \brief Network context proxy
    //! \ingroup dbus
    class CContextOwnAircraftProxy : public IContextOwnAircraft
    {
        Q_OBJECT
        friend class IContextOwnAircraft;

    public:

        //! Destructor
        virtual ~CContextOwnAircraftProxy() {}

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

        //! \brief Relay connection signals to local signals.
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        //! \brief Constructor
        CContextOwnAircraftProxy(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextOwnAircraft(mode, runtime), m_dBusInterface(nullptr) {}

        //! \brief DBus version constructor
        CContextOwnAircraftProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime);

    public slots: // IContextOwnAircraft overrides

        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

        //! \copydoc IContextOwnAircraft::setOwnAircraft()
        virtual void updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnPosition()
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnSituation()
        virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnCockpit()
        virtual bool updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updatePilot()
        virtual bool updatePilot(const BlackMisc::Network::CUser &pilot, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateSelcal
        virtual bool updateSelcal(const Aviation::CSelcal &selcal, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::setAudioOutputVolumes
        virtual void setAudioOutputVolumes(int outputVolumeCom1, int outputVolumeCom2) override;

        //! \copydoc IContextOwnAircraft::setAudioVoiceRoomOverrideUrls
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url,const QString &voiceRoom2Url);

        //! \copydoc IContextOwnAircraft::enableAutomaticVoiceRoomResolution
        virtual void enableAutomaticVoiceRoomResolution(bool enable);

        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine) override;

    };
}

#endif // guard
