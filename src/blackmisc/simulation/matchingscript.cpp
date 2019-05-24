/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "matchingscript.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Simulation
    {
        MSSwiftValues::MSSwiftValues(
            const QString &cs,
            const CAircraftIcaoCode aircraftIcao,
            const CAirlineIcaoCode airlineIcao,
            const QString &livery, int liveryId) :
            MSSwiftValues(cs,
                          aircraftIcao.getDesignator(), aircraftIcao.getDbKey(),
                          airlineIcao.getDesignator(),  airlineIcao.getVDesignator(), airlineIcao.getDbKey(),
                          livery, liveryId)
        { }

        MSSwiftValues::MSSwiftValues(
            const QString &cs,
            const CAircraftIcaoCode aircraftIcao,
            const CLivery livery) :
            MSSwiftValues(cs,
                          aircraftIcao,
                          livery.getAirlineIcaoCode(),
                          livery.getCombinedCode(), livery.getDbKey())
        { }

        MSSwiftValues::MSSwiftValues(const MSSwiftValues &sv) :
            MSSwiftValues(sv.m_callsign,
                          sv.getAircraftIcao(), sv.getDbAircraftIcaoId(),
                          sv.getAirlineIcao(), sv.getVirtualAirlineIcao(), sv.getDbAirlineIcaoId(),
                          sv.getLivery(), sv.getDbLiveryId(),
                          sv.m_logMessage,
                          sv.isModified(), sv.isRerun())
        { }

        void MSSwiftValues::setCallsign(const QString &callsign)
        {
            if (m_callsign == callsign) { return; }
            m_callsign = callsign;
            emit this->callsignChanged();
        }

        void MSSwiftValues::setDbAircraftIcaoId(int id)
        {
            if (id == m_dbAircraftIcaoId) { return; }
            m_dbAircraftIcaoId = id;
            emit this->dbAircraftIcaoIdChanged();
        }

        void MSSwiftValues::setDbAirlineIcaoId(int id)
        {
            if (id == m_dbAirlineIcaoId) { return; }
            m_dbAirlineIcaoId = id;
            emit this->dbAirlineIcaoIdChanged();
        }

        void MSSwiftValues::setDbLiveryId(int id)
        {
            if (id == m_dbLiveryId) { return; }
            m_dbLiveryId = id;
            emit this->dbLiveryIdChanged();
        }

        void MSSwiftValues::setAircraftIcao(const QString &aircraftIcao)
        {
            if (aircraftIcao == m_aircraftIcao) { return; }
            m_aircraftIcao = aircraftIcao;
            emit this->aircraftIcaoChanged();
        }

        void MSSwiftValues::setAirlineIcao(const QString &airlineIcao)
        {
            if (airlineIcao == m_airlineIcao) { return; }
            m_airlineIcao = airlineIcao;
            emit this->airlineIcaoChanged();
        }

        void MSSwiftValues::setVirtualAirlineIcao(const QString &virtualAirlineIcao)
        {
            if (virtualAirlineIcao == m_vAirlineIcao) { return; }
            m_vAirlineIcao = virtualAirlineIcao;
            emit this->virtualAirlineIcaoChanged();
        }

        void MSSwiftValues::setLivery(const QString &livery)
        {
            if (livery == m_livery) { return; }
            m_livery = livery;
            emit this->liveryChanged();
        }

        void MSSwiftValues::setLogMessage(const QString &msg)
        {
            if (msg == m_logMessage) { return; }
            m_logMessage = msg;
            emit this->logMessageChanged();
        }

        void MSSwiftValues::setModified(bool modified)
        {
            if (modified == m_modified) { return; }
            m_modified = modified;
            emit this->modifiedChanged();
        }

        void MSSwiftValues::setRerun(bool rerun)
        {
            if (rerun == m_rerun) { return; }
            m_rerun = rerun;
            emit this->rerunChanged();
        }

        bool MSSwiftValues::hasChangedAircraftIcao(const Aviation::CAircraftIcaoCode &aircraftIcao) const
        {
            if (aircraftIcao.hasValidDbKey() && aircraftIcao.getDbKey() != m_dbAircraftIcaoId)  { return true; }
            if (aircraftIcao.hasDesignator() && aircraftIcao.getDesignator() != m_aircraftIcao) { return true; }
            return true;
        }

        bool MSSwiftValues::hasChangedAirlineIcao(const Aviation::CAirlineIcaoCode &airlineIcao) const
        {
            if (airlineIcao.hasValidDbKey() && airlineIcao.getDbKey() != m_dbAirlineIcaoId)  { return true; }
            if (airlineIcao.hasValidDesignator() && airlineIcao.getDesignator()  != m_airlineIcao)  { return true; }
            if (airlineIcao.hasValidDesignator() && airlineIcao.getVDesignator() != m_vAirlineIcao) { return true; }
            return true;
        }

    } // namespace
} // namespace
