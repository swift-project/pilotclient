/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ownaircraftprovider.h"
#include "blackmisc/aviation/aircraft.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {

        const CSimulatedAircraft &COwnAircraftProviderSupportReadOnly::ownAircraft() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "ownAircraft", "No object available");
            return this->m_ownAircraftProvider->ownAircraft();
        }

        const CSimulatedAircraft &COwnAircraftProviderSupport::ownAircraft() const
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "ownAircraft", "No object available");
            return this->m_ownAircraftProvider->ownAircraft();
        }

        CSimulatedAircraft &COwnAircraftProviderSupport::ownAircraft()
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "ownAircraft", "No object available");
            return this->m_ownAircraftProvider->ownAircraft();
        }

        bool COwnAircraftProviderSupport::providerUpdateCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder, const QString &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "providerUpdateCockpit", "No object available");
            return this->m_ownAircraftProvider->updateCockpit(com1, com2, transponder, originator);
        }

        bool COwnAircraftProviderSupport::providerUpdateActiveComFrequency(const CFrequency &frequency, int comUnit, const QString &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "providerUpdateActiveComFrequency", "No object available");
            return this->m_ownAircraftProvider->updateActiveComFrequency(frequency, comUnit, originator);
        }

        bool COwnAircraftProviderSupport::providerUpdateSelcal(const CSelcal &selcal, const QString &originator)
        {
            Q_ASSERT_X(this->m_ownAircraftProvider, "providerUpdateSelcal", "No object available");
            return this->m_ownAircraftProvider->updateSelcal(selcal, originator);
        }

        bool COwnAircraftProviderDummy::updateCockpit(const Aviation::CComSystem &com1, const Aviation::CComSystem &com2, const Aviation::CTransponder &transponder, const QString &originator)
        {
            ownAircraft().setCom1System(com1);
            ownAircraft().setCom2System(com2);
            ownAircraft().setTransponder(transponder);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateActiveComFrequency(const PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator)
        {
            if (!CComSystem::isValidComFrequency(frequency)) { return false; }
            CComSystem::ComUnit comUnitEnum = static_cast<CComSystem::ComUnit>(comUnit);
            CComSystem com = ownAircraft().getComSystem(comUnitEnum);
            com.setFrequencyActive(frequency);
            ownAircraft().setComSystem(com, comUnitEnum);
            Q_UNUSED(originator);
            return true;
        }

        bool COwnAircraftProviderDummy::updateSelcal(const CSelcal &selcal, const QString &originator)
        {
            ownAircraft().setSelcal(selcal);
            Q_UNUSED(originator);
            return true;
        }

        COwnAircraftProviderDummy *COwnAircraftProviderDummy::instance()
        {
            static COwnAircraftProviderDummy *dummy = new COwnAircraftProviderDummy();
            return dummy;
        }

    } // namespace
} // namespace
