/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

        //! \copydoc IContextOwnAircraft::setAudioOutputVolumes
        virtual void setAudioOutputVolumes(int outputVolumeCom1, int outputVolumeCom2) override;

        //! \copydoc IContextOwnAircraft::setAudioVoiceRoomOverrideUrls
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url,const QString &voiceRoom2Url);

        //! \copydoc IContextOwnAircraft::enableAutomaticVoiceRoomResolution
        virtual void enableAutomaticVoiceRoomResolution(bool enable);

    };
}

#endif // guard
