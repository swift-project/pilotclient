/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file

#ifndef BLACKCORE_CONTEXTOWNAIRCRAFT_H
#define BLACKCORE_CONTEXTOWNAIRCRAFT_H

#include "blackcore/context.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/voiceroomlist.h"

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextOwnAircraft"

//! DBus object path for context
#define BLACKCORE_CONTEXTOWNAIRCRAFT_OBJECTPATH "/OwnAircraft"

//! @}

namespace BlackCore
{

    //! \brief Own context proxy
    //! \ingroup dbus
    class IContextOwnAircraft : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)

    public:
        //! Log message category
        static QString getMessageCategory() { return "swift.context.ownaircraft"; }

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

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextOwnAircraft *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! Destructor
        virtual ~IContextOwnAircraft() {}

    signals:
        //! \brief Aircraft changed
        //! \remarks local only
        void changedAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! Aircraft situation update
        //! \remarks local only
        void changedAircraftSituation(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! Aircraft position update
        //! \remarks local only
        void changedAircraftPosition(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! Aircraft cockpit update
        //! \remarks DBus and local
        void changedAircraftCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

    public slots:

        //! Get own aircraft
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

        //! Own aircraft
        virtual void updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) = 0;

        //! Own position, be aware height is terrain height
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator) = 0;

        //! Complete situation update
        virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator) = 0;

        //! Update own cockpit
        virtual bool updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) = 0;

        //! Set current pilot
        virtual bool updatePilot(const BlackMisc::Network::CUser &pilot, const QString &originator) = 0;

        //! Output volumes,  volumes 0..100
        virtual void setAudioOutputVolumes(int outputVolumeCom1, int outputVolumeCom2) = 0;

        //! Set individual voice rooms (overrides voice rooms)
        //! \remarks Empty string "" disables voice room override
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url) = 0;

        //! Automatic voice room resolution for frequencies
        virtual void enableAutomaticVoiceRoomResolution(bool enable) = 0;

    protected:
        //! Constructor
        IContextOwnAircraft(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };
}

#endif // guard
