/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatedaircraft.h"
#include "blackmisc/propertyindex.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {

        CSimulatedAircraft::CSimulatedAircraft()
        {
            init();
        }

        CSimulatedAircraft::CSimulatedAircraft(const CAircraft &aircraft, const CAircraftModel &model, const CClient &client) :
            CValueObjectStdTuple(aircraft), m_model(model), m_client(client)
        {
            init();
        }

        void CSimulatedAircraft::init()
        {
            // sync some values, order here is crucial
            this->setCallsign(this->getCallsign());
            this->setIcaoInfo(this->getIcaoInfo());
            this->setModel(this->getModel());
            this->setPilot(this->hasValidRealName() ? this->getPilot() : this->getClient().getUser());
        }

        CVariant CSimulatedAircraft::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                return this->m_model.propertyByIndex(index.copyFrontRemoved());
            case IndexClient:
                return this->m_client.propertyByIndex(index.copyFrontRemoved());
            case IndexEnabled:
                return CVariant::fromValue(this->isEnabled());
            default:
                return CAircraft::propertyByIndex(index);
            }
        }

        void CSimulatedAircraft::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexModel:
                this->m_model.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexClient:
                this->m_client.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexEnabled:
                this->m_enabled = variant.toBool();
                break;
            default:
                CAircraft::setPropertyByIndex(variant, index);
                break;
            }
        }

        void CSimulatedAircraft::setModel(const CAircraftModel &model)
        {
            // sync the callsigns
            this->m_model = model;
            this->setCallsign(this->hasValidCallsign() ? this->getCallsign() : model.getCallsign());
            this->setIcaoInfo(model.getIcao());
        }

        void CSimulatedAircraft::setCallsign(const CCallsign &callsign)
        {
            this->m_model.setCallsign(callsign);
            this->m_client.setUserCallsign(callsign);
            CAircraft::setCallsign(callsign);
        }

        void CSimulatedAircraft::setIcaoInfo(const CAircraftIcao &icao)
        {
            // snyc ICAO info
            CAircraftIcao newIcao(icao);
            newIcao.updateMissingParts(this->getIcaoInfo());
            newIcao.updateMissingParts(this->getModel().getIcao());

            // now we have a superset of ICAO data
            this->m_model.setIcao(newIcao);
            CAircraft::setIcaoInfo(newIcao);
        }

        void CSimulatedAircraft::setPilot(const CUser &user)
        {
            this->m_client.setUser(user);
            CAircraft::setPilot(user);
        }

        void CSimulatedAircraft::setClient(const CClient &client)
        {
            Q_ASSERT(client.getCallsign() == this->getCallsign());
            m_client = client;
        }

        //! \todo Smarter way to do this?
        void CSimulatedAircraft::update(const CAircraft &aircraft)
        {
            // override
            (*this) = CSimulatedAircraft(aircraft, this->getModel(), this->getClient());
        }

        QString CSimulatedAircraft::convertToQString(bool i18n) const
        {
            QString s = CAircraft::convertToQString(i18n);
            s += " enabled: ";
            s += this->isEnabled() ? "yes" : "no";
            s += " ";
            s += this->m_model.toQString(i18n);
            s += " ";
            s += this->m_client.toQString(i18n);
            return s;
        }

    } // namespace
} // namespace
