// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/matchingscript.h"

#include "misc/stringutils.h"

using namespace swift::misc::aviation;

namespace swift::misc::simulation
{
    MSInOutValues::MSInOutValues(const CCallsign &cs, const CAircraftIcaoCode &aircraftIcao,
                                 const CAirlineIcaoCode &airlineIcao, const QString &livery, int liveryId)
        : MSInOutValues(cs.asString(), cs.getStringAsSet(), cs.getFlightNumber(), aircraftIcao.getDesignator(),
                        aircraftIcao.getFamily(), aircraftIcao.getCombinedType(), aircraftIcao.getDbKey(),
                        airlineIcao.getDesignator(), airlineIcao.getVDesignator(), airlineIcao.getDbKey(), livery,
                        liveryId)
    {}

    MSInOutValues::MSInOutValues(const CCallsign &cs, const CAircraftIcaoCode &aircraftIcao, const CLivery &livery)
        : MSInOutValues(cs, aircraftIcao, livery.getAirlineIcaoCode(), livery.getCombinedCode(), livery.getDbKey())
    {}

    MSInOutValues::MSInOutValues(const CAircraftModel &model)
        : MSInOutValues(model.getCallsign(), model.getAircraftIcaoCode(), model.getLivery())
    {}

    MSInOutValues::MSInOutValues(const MSInOutValues &sv)
        : MSInOutValues(sv.m_callsign, sv.m_callsignAsSet, sv.m_flightNumber, sv.getAircraftIcao(),
                        sv.getCombinedType(), sv.getAircraftFamily(), sv.getDbAircraftIcaoId(), sv.getAirlineIcao(),
                        sv.getVirtualAirlineIcao(), sv.getDbAirlineIcaoId(), sv.getLivery(), sv.getDbLiveryId(),
                        sv.m_logMessage, sv.isModified(), sv.isRerun())
    {}

    void MSInOutValues::setCallsign(const QString &callsign)
    {
        if (m_callsign == callsign) { return; }
        m_callsign = callsign;
        emit this->callsignChanged();
    }

    void MSInOutValues::setDbAircraftIcaoId(int id)
    {
        if (id == m_dbAircraftIcaoId) { return; }
        m_dbAircraftIcaoId = id;
        emit this->dbAircraftIcaoIdChanged();
    }

    void MSInOutValues::setDbAirlineIcaoId(int id)
    {
        if (id == m_dbAirlineIcaoId) { return; }
        m_dbAirlineIcaoId = id;
        emit this->dbAirlineIcaoIdChanged();
    }

    void MSInOutValues::setDbLiveryId(int id)
    {
        if (id == m_dbLiveryId) { return; }
        m_dbLiveryId = id;
        emit this->dbLiveryIdChanged();
    }

    void MSInOutValues::setDbModelId(int id)
    {
        if (id == m_dbModelId) { return; }
        m_dbModelId = id;
        emit this->dbModelIdChanged();
    }

    void MSInOutValues::setAircraftIcao(const QString &aircraftIcao)
    {
        if (aircraftIcao == m_aircraftIcao) { return; }
        m_aircraftIcao = aircraftIcao;
        emit this->aircraftIcaoChanged();
    }

    void MSInOutValues::setAirlineIcao(const QString &airlineIcao)
    {
        if (airlineIcao == m_airlineIcao) { return; }
        m_airlineIcao = airlineIcao;
        emit this->airlineIcaoChanged();
    }

    void MSInOutValues::setVirtualAirlineIcao(const QString &virtualAirlineIcao)
    {
        if (virtualAirlineIcao == m_vAirlineIcao) { return; }
        m_vAirlineIcao = virtualAirlineIcao;
        emit this->virtualAirlineIcaoChanged();
    }

    void MSInOutValues::setLivery(const QString &livery)
    {
        if (livery == m_livery) { return; }
        m_livery = livery;
        emit this->liveryChanged();
    }

    void MSInOutValues::setModelString(const QString &modelString)
    {
        if (modelString == m_modelString) { return; }
        m_modelString = modelString;
        emit this->modelStringChanged();
    }

    void MSInOutValues::setCombinedType(const QString &type)
    {
        if (type == m_combinedType) { return; }
        m_combinedType = type.trimmed().toUpper();
        emit this->combinedTypeChanged();
    }

    void MSInOutValues::setLogMessage(const QString &msg)
    {
        if (msg == m_logMessage) { return; }
        m_logMessage = msg;
        emit this->logMessageChanged();
    }

    void MSInOutValues::setModified(bool modified)
    {
        if (modified == m_modified) { return; }
        m_modified = modified;
        emit this->modifiedChanged();
    }

    void MSInOutValues::setRerun(bool rerun)
    {
        if (rerun == m_rerun) { return; }
        m_rerun = rerun;
        emit this->rerunChanged();
    }

    void MSInOutValues::evaluateChanges(const CAircraftIcaoCode &aircraft, const CAirlineIcaoCode &airline)
    {
        m_modifiedAircraftDesignator = aircraft.getDesignator() != m_aircraftIcao;
        m_modifiedAircraftFamily = aircraft.getFamily() != m_aircraftFamily;
        if (airline.isVirtualAirline()) { m_modifiedAirlineDesignator = airline.getVDesignator() != m_vAirlineIcao; }
        else { m_modifiedAirlineDesignator = airline.getDesignator() != m_airlineIcao; }
    }

    bool MSInOutValues::hasChangedAircraftIcao(const aviation::CAircraftIcaoCode &aircraftIcao) const
    {
        if (aircraftIcao.hasValidDbKey() && aircraftIcao.getDbKey() != m_dbAircraftIcaoId) { return true; }
        if (aircraftIcao.hasDesignator() && aircraftIcao.getDesignator() != m_aircraftIcao) { return true; }
        return true;
    }

    bool MSInOutValues::hasChangedAircraftIcaoId(const CAircraftIcaoCode &aircraftIcao) const
    {
        if (aircraftIcao.hasValidDbKey() && m_dbAircraftIcaoId >= 0)
        {
            // valid ids to be compared
            return m_dbAircraftIcaoId != aircraftIcao.getDbKey();
        }
        return false;
    }

    bool MSInOutValues::hasChangedAirlineIcao(const aviation::CAirlineIcaoCode &airlineIcao) const
    {
        if (airlineIcao.hasValidDbKey() && airlineIcao.getDbKey() != m_dbAirlineIcaoId) { return true; }
        if (airlineIcao.hasValidDesignator() && airlineIcao.getDesignator() != m_airlineIcao) { return true; }
        if (airlineIcao.hasValidDesignator() && airlineIcao.getVDesignator() != m_vAirlineIcao) { return true; }
        return false;
    }

    bool MSInOutValues::hasChangedAirlineIcaoId(const CAirlineIcaoCode &airlineIcao) const
    {
        if (airlineIcao.hasValidDbKey() && m_dbAirlineIcaoId >= 0)
        {
            // valid ids to be compared
            return m_dbAirlineIcaoId != airlineIcao.getDbKey();
        }
        return false;
    }

    bool MSInOutValues::hasChangedModelString(const QString &modelString) const
    {
        return !stringCompare(m_modelString, modelString, Qt::CaseInsensitive);
    }

    bool MSInOutValues::hasChangedLiveryId(const CLivery &livery) const
    {
        if (livery.hasValidDbKey() && m_dbLiveryId >= 0)
        {
            // valid ids to be compared
            return m_dbLiveryId != livery.getDbKey();
        }
        return false;
    }

    bool MSInOutValues::hasChangedModelId(const CAircraftModel &model) const
    {
        if (model.hasValidDbKey() && m_dbModelId >= 0)
        {
            // valid ids to be compared
            return m_dbModelId != model.getDbKey();
        }
        return false;
    }

    bool MSInOutValues::hasChangedModel(const CAircraftModel &model) const
    {
        return this->hasChangedModelString(model.getModelString()) || this->hasChangedModelId(model);
    }

    void MSModelSet::setSimulator(const QString &simulator)
    {
        if (m_simulator == simulator) { return; }
        m_simulator = simulator.trimmed().toUpper();
        emit this->simulatorChanged();
    }

    void MSModelSet::setAvailable(bool available)
    {
        if (m_available == available) { return; }
        m_available = available;
        emit this->availabilityChanged();
    }

    void MSModelSet::setInputAircraftAndAirlineCount(int count)
    {
        if (m_inputAircraftAndAirlineCount == count) { return; }
        m_inputAircraftAndAirlineCount = count;
        emit this->inputAircraftAndAirlineCountChanged();
    }

    QString MSModelSet::findCombinedTypeWithClosestColorLivery(const QString &combinedType,
                                                               const QString &rgbColor) const
    {
        if (combinedType.isEmpty() || rgbColor.isEmpty()) { return QString(); }
        CAircraftModelList models = m_modelSet.findByCombinedTypeWithColorLivery(combinedType);
        if (models.isEmpty()) { return QString(); }
        const CRgbColor color(rgbColor);
        models = models.findClosestFuselageColorDistance(color);
        return models.isEmpty() ? QString() : models.front().getModelString();
    }

    QString MSModelSet::findClosestCombinedTypeWithClosestColorLivery(const QString &combinedType,
                                                                      const QString &rgbColor) const
    {
        QString ms = this->findCombinedTypeWithClosestColorLivery(combinedType, rgbColor);
        if (!ms.isEmpty()) { return ms; }
        if (combinedType.size() != 3) { return ms; }
        QString wildCard(combinedType);
        if (wildCard.size() != 3) { return QString(); }
        wildCard[1] = '*';
        return this->findCombinedTypeWithClosestColorLivery(wildCard, rgbColor);
    }

    QString MSModelSet::findAircraftFamilyWithClosestColorLivery(const QString &family, const QString &rgbColor) const
    {
        if (family.isEmpty() || rgbColor.isEmpty()) { return QString(); }
        CAircraftModelList models = m_modelSet.findByFamilyWithColorLivery(family);
        if (models.isEmpty()) { return QString(); }
        const CRgbColor color(rgbColor);
        models = models.findClosestFuselageColorDistance(color);
        return models.isEmpty() ? QString() : models.front().getModelString();
    }

    MSModelSet::MSModelSet(const CAircraftModelList &modelSet) { this->initByModelSet(modelSet); }

    void MSModelSet::initByModelSet(const CAircraftModelList &modelSet)
    {
        m_modelSet = modelSet;
        m_available = !modelSet.isEmpty();
    }

    void MSModelSet::initByAircraftAndAirline(const CAircraftIcaoCode &aircraft, const CAirlineIcaoCode &airline)
    {
        if (m_modelSet.isEmpty() || !aircraft.hasDesignator() || !airline.hasValidDesignator())
        {
            m_inputAircraftAndAirlineCount = false;
            return;
        }

        const CAircraftModelList models = m_modelSet.findByAircraftAndAirline(aircraft, airline);
        m_inputAircraftAndAirlineCount = models.size();
    }

} // namespace swift::misc::simulation
