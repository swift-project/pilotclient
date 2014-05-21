/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTOWNAIRCRAFT_IMPL_H
#define BLACKCORE_CONTEXTOWNAIRCRAFT_IMPL_H

#include "blackcore/context_ownaircraft.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_runtime.h"
#include "blackcore/dbus_server.h"

namespace BlackCore
{

    //! Network context implementation
    class CContextOwnAircraft : public IContextOwnAircraft
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextOwnAircraft();

        //! Own aircraft
        const BlackMisc::Aviation::CAircraft &ownAircraft() const { return this->m_ownAircraft; }

        //! Own aircraft
        BlackMisc::Aviation::CAircraft &ownAircraft() { return this->m_ownAircraft; }

    public slots: // IContextOwnAircraft overrides

        //! \copydoc IContextOwnAircraft::setOwnAircraft()
        virtual void updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnPosition()
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnSituation()
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::updateOwnCockpit()
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) override;

        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const override;

    protected:
        //! Constructor, with link to runtime
        CContextOwnAircraft(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextOwnAircraft *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextOwnAircraft::ObjectPath(), this);
            return this;
        }

    private:
        BlackMisc::Aviation::CAircraft m_ownAircraft; //!< my aircraft

        //! Init my very own aircraft
        void initOwnAircraft();

    };
}

#endif // guard
