/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingscript.h"

namespace BlackMisc
{
    namespace Simulation
    {
        void MSNetworkValues::setCallsign(const QString &callsign)
        {
            if (m_callsign == callsign) { return; }
            m_callsign = callsign;
            emit this->callsignChanged();
        }

        void MSNetworkValues::setDbAircraftIcaoId(int id)
        {
            if (id == m_dbAircraftIcaoId) { return; }
            m_dbAircraftIcaoId = id;
            emit this->dbAircraftIcaoIdChanged();
        }

        void MSNetworkValues::setDbAirlineIcaoId(int id)
        {
            if (id == m_dbAirlineIcaoId) { return; }
            m_dbAirlineIcaoId = id;
            emit this->dbAirlineIcaoIdChanged();
        }

        void MSNetworkValues::setDbLiveryId(int id)
        {
            if (id == m_dbLiveryId) { return; }
            m_dbLiveryId = id;
            emit this->dbLiveryIdChanged();
        }

        void MSNetworkValues::setAircraftIcao(const QString &aircraftIcao)
        {
            if (aircraftIcao == m_aircraftIcao) { return; }
            m_aircraftIcao = aircraftIcao;
            emit this->aircraftIcaoChanged();
        }

        void MSNetworkValues::setAirlineIcao(const QString &airlineIcao)
        {
            if (airlineIcao == m_airlineIcao) { return; }
            m_airlineIcao = airlineIcao;
            emit this->airlineIcaoChanged();
        }

        void MSNetworkValues::setLivery(const QString &livery)
        {
            if (livery == m_livery) { return; }
            m_livery = livery;
            emit this->liveryChanged();
        }
    } // namespace
} // namespace
