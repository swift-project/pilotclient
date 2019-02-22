/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/aircraftmodelfilter.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/simulation/aircraftmodel.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Models
    {
        CAircraftModelFilter::CAircraftModelFilter(int id, const QString &modelKey, const QString &description,
                CAircraftModel::ModelModeFilter modelMode, BlackMisc::Db::DbKeyStateFilter dbKeyFilter,
                Qt::CheckState military, Qt::CheckState colorLiveries,
                const QString &aircraftIcao, const QString &aircraftManufacturer,
                const QString &airlineIcao, const QString &airlineName,
                const QString &liveryCode, const QString &fileName,
                const CSimulatorInfo &simInfo,
                const CDistributor &distributor) :
            m_id(id),
            m_modelKey(modelKey.trimmed().toUpper()), m_description(description.trimmed()),
            m_modelMode(modelMode), m_dbKeyFilter(dbKeyFilter), m_military(military), m_colorLiveries(colorLiveries),
            m_aircraftIcao(aircraftIcao.trimmed().toUpper()), m_aircraftManufacturer(aircraftManufacturer.trimmed().toUpper()),
            m_airlineIcao(airlineIcao.trimmed().toUpper()), m_airlineName(airlineName.trimmed().toUpper()),
            m_liveryCode(liveryCode.trimmed().toUpper()),
            m_fileName(fileName),
            m_simulatorInfo(simInfo),
            m_distributor(distributor)
        {
            m_valid = valid();
        }

        BlackMisc::Simulation::CAircraftModelList CAircraftModelFilter::filter(const CAircraftModelList &inContainer) const
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
                    if (!this->m_simulatorInfo.matchesAny(model.getSimulator())) { continue; }
                }

                if (!this->m_modelKey.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getModelString(), this->m_modelKey)) { continue; }
                }

                if (this->m_military != Qt::PartiallyChecked)
                {
                    if (this->m_military == Qt::Checked)
                    {
                        // military only
                        if (!model.isMilitary()) { continue; }
                    }
                    else if (this->m_military == Qt::Unchecked)
                    {
                        // civilian only
                        if (model.isMilitary()) { continue; }
                    }
                }

                if (this->m_colorLiveries != Qt::PartiallyChecked)
                {
                    if (this->m_colorLiveries == Qt::Checked)
                    {
                        // only color liveries
                        if (!model.getLivery().isColorLivery()) { continue; }
                    }
                    else if (this->m_colorLiveries == Qt::Unchecked)
                    {
                        // Only airline liveries
                        if (model.getLivery().isColorLivery()) { continue; }
                    }
                }

                if (!this->m_description.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getDescription(), this->m_description)) { continue; }
                }

                if (this->m_modelMode != CAircraftModel::All && this->m_modelMode != CAircraftModel::Undefined)
                {
                    if (!model.matchesMode(this->m_modelMode)) { continue; }
                }

                if (this->m_dbKeyFilter != BlackMisc::Db::All && this->m_dbKeyFilter != BlackMisc::Db::Undefined)
                {
                    if (!model.matchesDbKeyState(this->m_dbKeyFilter)) { continue; }
                }

                if (!this->m_fileName.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getFileName(), this->m_fileName)) { continue; }
                }

                if (!this->m_aircraftIcao.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCodeDesignator(), this->m_aircraftIcao)) { continue; }
                }

                if (!this->m_aircraftManufacturer.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAircraftIcaoCode().getManufacturer(), this->m_aircraftManufacturer)) { continue; }
                }

                if (!this->m_airlineIcao.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCodeDesignator(), this->m_airlineIcao)) { continue; }
                }

                if (!this->m_airlineName.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getAirlineIcaoCode().getName(), this->m_airlineName)) { continue; }
                }

                if (!this->m_liveryCode.isEmpty())
                {
                    if (!this->stringMatchesFilterExpression(model.getLivery().getCombinedCode(), this->m_liveryCode)) { continue; }
                }

                if (this->m_distributor.hasValidDbKey())
                {
                    if (!model.getDistributor().matchesKeyOrAlias(this->m_distributor)) { continue; }
                }

                outContainer.push_back(model);
            }
            return outContainer;
        }

        bool CAircraftModelFilter::valid() const
        {
            const bool allEmpty =
                m_id < 0 &&
                this->m_modelKey.isEmpty() && this->m_description.isEmpty() &&
                this->m_aircraftManufacturer.isEmpty() && this->m_aircraftIcao.isEmpty() &&
                this->m_airlineIcao.isEmpty() && this->m_airlineName.isEmpty() &&
                this->m_liveryCode.isEmpty() && this->m_fileName.isEmpty();
            if (!allEmpty) { return true; }
            const bool noSim = this->m_simulatorInfo.isNoSimulator() || this->m_simulatorInfo.isAllSimulators();
            const bool noModelMode = this->m_modelMode == CAircraftModel::Undefined || this->m_modelMode == CAircraftModel::All;
            const bool noDbState = this->m_dbKeyFilter == BlackMisc::Db::Undefined || this->m_dbKeyFilter == BlackMisc::Db::All;
            const bool noKey = !this->m_distributor.hasValidDbKey();
            const bool noColorRestriction = this->m_colorLiveries == Qt::PartiallyChecked;
            const bool noMilitary = this->m_military == Qt::PartiallyChecked;
            return !(noSim && noModelMode && noDbState && noKey && noMilitary && noColorRestriction);
        }
    } // namespace
} // namespace
