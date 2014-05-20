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

    public slots: // IContextOwnAircraft overrides

        //! \copydoc IContextOwnAircraft::setOwnAircraft()
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) override;

        //! \copydoc IContextOwnAircraft::updateOwnPosition()
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;

        //! \copydoc IContextOwnAircraft::updateOwnSituation()
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

        //! \copydoc IContextOwnAircraft::updateOwnCockpit()
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) override;

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
        BlackMisc::Aviation::CAircraft m_ownAircraft;

        //! Init my very own aircraft
        void initOwnAircraft();


    };
}

#endif // guard
