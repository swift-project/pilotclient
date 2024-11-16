// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/aircraftmodelfilter.h"

#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/livery.h"
#include "misc/simulation/aircraftmodel.h"

using namespace swift::misc::simulation;

namespace swift::gui::models
{
    CAircraftModelFilter::CAircraftModelFilter(int id, const QString &modelKey, const QString &description,
                                               CAircraftModel::ModelModeFilter modelMode,
                                               swift::misc::db::DbKeyStateFilter dbKeyFilter, Qt::CheckState military,
                                               Qt::CheckState colorLiveries, const QString &aircraftIcao,
                                               const QString &aircraftManufacturer, const QString &airlineIcao,
                                               const QString &airlineName, const QString &liveryCode,
                                               const QString &fileName, const QString &combinedType,
                                               const CSimulatorInfo &simInfo, const CDistributor &distributor)
        : m_id(id), m_modelKey(modelKey.trimmed().toUpper()), m_description(description.trimmed()),
          m_modelMode(modelMode), m_dbKeyFilter(dbKeyFilter), m_military(military), m_colorLiveries(colorLiveries),
          m_aircraftIcao(aircraftIcao.trimmed().toUpper()),
          m_aircraftManufacturer(aircraftManufacturer.trimmed().toUpper()),
          m_airlineIcao(airlineIcao.trimmed().toUpper()), m_airlineName(airlineName.trimmed().toUpper()),
          m_liveryCode(liveryCode.trimmed().toUpper()), m_fileName(fileName), m_combinedType(combinedType),
          m_simulatorInfo(simInfo), m_distributor(distributor)
    {
        m_valid = this->valid();
    }

    CAircraftModelList CAircraftModelFilter::filter(const CAircraftModelList &inContainer) const
    {
        if (!this->isEnabled()) { return inContainer; }
        CAircraftModelList outContainer;
        for (const CAircraftModel &model : inContainer)
        {
            if (m_id >= 0)
            {
                // search only for id
                if (model.isLoadedFromDb() && model.getDbKey() == m_id)
                {
                    outContainer.push_back(model);
                    break;
                }
                continue;
            }

            if (!m_simulatorInfo.isAllSimulators())
            {
                if (!m_simulatorInfo.matchesAny(model.getSimulator())) { continue; }
            }

            if (!m_modelKey.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getModelString(), m_modelKey)) { continue; }
            }

            if (m_military != Qt::PartiallyChecked)
            {
                if (m_military == Qt::Checked)
                {
                    // military only
                    if (!model.isMilitary()) { continue; }
                }
                else if (m_military == Qt::Unchecked)
                {
                    // civilian only
                    if (model.isMilitary()) { continue; }
                }
            }

            if (m_colorLiveries != Qt::PartiallyChecked)
            {
                if (m_colorLiveries == Qt::Checked)
                {
                    // only color liveries
                    if (!model.getLivery().isColorLivery()) { continue; }
                }
                else if (m_colorLiveries == Qt::Unchecked)
                {
                    // Only airline liveries
                    if (model.getLivery().isColorLivery()) { continue; }
                }
            }

            if (!m_description.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getDescription(), m_description)) { continue; }
            }

            if (m_modelMode != CAircraftModel::All && m_modelMode != CAircraftModel::Undefined)
            {
                if (!model.matchesMode(m_modelMode)) { continue; }
            }

            if (m_dbKeyFilter != swift::misc::db::All && m_dbKeyFilter != swift::misc::db::Undefined)
            {
                if (!model.matchesDbKeyState(m_dbKeyFilter)) { continue; }
            }

            if (!m_fileName.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getFileName(), m_fileName)) { continue; }
            }

            if (!m_aircraftIcao.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCodeDesignator(), m_aircraftIcao))
                {
                    continue;
                }
            }

            if (!m_aircraftManufacturer.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCode().getManufacturer(),
                                                         m_aircraftManufacturer))
                {
                    continue;
                }
            }

            if (!m_airlineIcao.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCodeDesignator(), m_airlineIcao))
                {
                    continue;
                }
            }

            if (!m_airlineName.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCode().getName(), m_airlineName))
                {
                    continue;
                }
            }

            if (!m_liveryCode.isEmpty())
            {
                if (!this->stringMatchesFilterExpression(model.getLivery().getCombinedCode(), m_liveryCode))
                {
                    continue;
                }
            }

            if (m_distributor.hasValidDbKey())
            {
                if (!model.getDistributor().matchesKeyOrAlias(m_distributor)) { continue; }
            }

            if (!m_combinedType.isEmpty())
            {
                if (!model.getAircraftIcaoCode().matchesCombinedType(m_combinedType)) { continue; }
            }

            outContainer.push_back(model);
        }
        return outContainer;
    }

    bool CAircraftModelFilter::valid() const
    {
        const bool allEmpty = m_id < 0 && m_modelKey.isEmpty() && m_description.isEmpty() &&
                              m_aircraftManufacturer.isEmpty() && m_aircraftIcao.isEmpty() && m_airlineIcao.isEmpty() &&
                              m_airlineName.isEmpty() && m_liveryCode.isEmpty() && m_fileName.isEmpty() &&
                              m_combinedType.isEmpty();
        if (!allEmpty) { return true; }
        const bool noSim = m_simulatorInfo.isNoSimulator() || m_simulatorInfo.isAllSimulators();
        const bool noModelMode = (m_modelMode == CAircraftModel::Undefined || m_modelMode == CAircraftModel::All);
        const bool noDbState = (m_dbKeyFilter == swift::misc::db::Undefined || m_dbKeyFilter == swift::misc::db::All);
        const bool noKey = !m_distributor.hasValidDbKey();
        const bool noColorRestriction = (m_colorLiveries == Qt::PartiallyChecked);
        const bool noMilitary = (m_military == Qt::PartiallyChecked);
        return !(noSim && noModelMode && noDbState && noKey && noMilitary && noColorRestriction);
    }
} // namespace swift::gui::models
