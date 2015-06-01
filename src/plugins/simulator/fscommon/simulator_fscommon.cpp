/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator_fscommon.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fscommon/modelmappingsprovidervpilot.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;

namespace BlackSimPlugin
{
    namespace FsCommon
    {
        CSimulatorFsCommon::CSimulatorFsCommon(
            const CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *renderedAircraftProvider,
            IPluginStorageProvider *pluginStorageProvider,
            QString simRootDirectory,
            QStringList excludedDirectories,
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, renderedAircraftProvider, pluginStorageProvider, parent),
            m_fsuipc(new CFsuipc()),
            m_aircraftCfgParser(simRootDirectory, excludedDirectories),
            m_modelMatcher(CAircraftMatcher::AllModes, this)
        {
            // hack to init mapper
            connect(&m_modelMatcher, &CAircraftMatcher::initializationFinished, this, &CSimulatorFsCommon::ps_mapperInitialized);
            auto modelMappingsProvider = std::unique_ptr<IModelMappingsProvider> { BlackMisc::make_unique<CModelMappingsProviderVPilot>(true) };
            m_modelMatcher.setModelMappingProvider(std::move(modelMappingsProvider));

            CVariant aircraftCfg = getPluginData(this, "aircraft_cfg");
            if (aircraftCfg.isValid())
            {
                m_modelMatcher.setInstalledModels(aircraftCfg.value<CAircraftCfgEntriesList>().toAircraftModelList());
                m_modelMatcher.init();
            }
            //
            // reading from settings would go here
            //
            else
            {
                connect(&m_aircraftCfgParser, &CAircraftCfgParser::parsingFinished, this, &CSimulatorFsCommon::ps_aircraftCfgParsingFinished);
                m_aircraftCfgParser.parse();
            }
        }

        CSimulatorFsCommon::~CSimulatorFsCommon() { }

        void CSimulatorFsCommon::ps_mapperInitialized()
        {
            emit this->installedAircraftModelsChanged();
        }

        bool CSimulatorFsCommon::disconnectFrom()
        {
            if (this->m_fsuipc) { this->m_fsuipc->disconnect(); }

            // reset flags
            m_simPaused = false;
            emitSimulatorCombinedStatus();
            return true;
        }

        bool CSimulatorFsCommon::isFsuipcConnected() const
        {
            return !m_fsuipc.isNull() && m_fsuipc->isConnected();
        }

        CTime CSimulatorFsCommon::getTimeSynchronizationOffset() const
        {
            return m_syncTimeOffset;
        }

        bool CSimulatorFsCommon::setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset)
        {
            this->m_simTimeSynced = enable;
            this->m_syncTimeOffset = offset;
            return true;
        }

        CAirportList CSimulatorFsCommon::getAirportsInRange() const
        {
            return m_airportsInRange;
        }

        void CSimulatorFsCommon::setOwnAircraftModel(const QString &modelName)
        {
            CAircraftModel model = getOwnAircraftModel();
            model.setModelString(modelName);
            this->setOwnAircraftModel(model);
        }

        void CSimulatorFsCommon::setOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
        {
            if (getOwnAircraftModel() != model)
            {
                CAircraftModel newModel(model);
                newModel.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                CSimulatorFsCommon::reverseLookupIcaoData(newModel);
                updateOwnModel(newModel);
                emit ownAircraftModelChanged(getOwnAircraft());
            }
        }

        void CSimulatorFsCommon::reverseLookupIcaoData(CAircraftModel &model)
        {
            if (m_modelMatcher.isInitialized())
            {
                // reverse lookup of ICAO
                CAircraftIcaoData icao =  m_modelMatcher.getIcaoForModelString(model.getModelString());
                icao.updateMissingParts(model.getIcao());
                model.setIcao(icao); // now best ICAO info in model
            }
        }

        CAircraftModel CSimulatorFsCommon::getClosestMatch(const CSimulatedAircraft &remoteAircraft)
        {
            return m_modelMatcher.getClosestMatch(remoteAircraft);
        }

        CAircraftModelList CSimulatorFsCommon::getInstalledModels() const
        {
            return m_aircraftCfgParser.getAircraftCfgEntriesList().toAircraftModelList();
        }

        CAircraftIcaoData CSimulatorFsCommon::getIcaoForModelString(const QString &modelString) const
        {
            if (!m_modelMatcher.isInitialized()) { return CAircraftIcaoData(); }
            return m_modelMatcher.getIcaoForModelString(modelString);
        }

        void CSimulatorFsCommon::reloadInstalledModels()
        {
            m_aircraftCfgParser.parse();
        }

        CPixmap CSimulatorFsCommon::iconForModel(const QString &modelString) const
        {
            static const CPixmap empty;
            if (modelString.isEmpty()) { return empty; }
            CAircraftCfgEntriesList cfgEntries = m_aircraftCfgParser.getAircraftCfgEntriesList().findByTitle(modelString);
            if (cfgEntries.isEmpty())
            {
                CLogMessage(this).warning("No .cfg entry for '%1'") << modelString;
                return empty;
            }

            // normally we should have only one entry
            if (cfgEntries.size() > 1)
            {
                CLogMessage(this).warning("Multiple FSX .cfg entries for '%1'") << modelString;
            }

            // use first with icon
            for (const CAircraftCfgEntries &entry : cfgEntries)
            {
                const QString thumbnail = entry.getThumbnailFileName();
                if (thumbnail.isEmpty()) { continue; }
                QPixmap pm;
                if (pm.load(thumbnail))
                {
                    return CPixmap(pm);
                }
            }
            return empty;
        }

        bool CSimulatorFsCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft, const COriginator &originator)
        {
            if (originator == this->originator()) { return false; }

            // remove upfront, and then enable / disable again
            this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
            return this->changeRemoteAircraftEnabled(aircraft, originator);
        }

        bool CSimulatorFsCommon::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const COriginator &originator)
        {
            if (originator == this->originator()) { return false; }
            if (aircraft.isEnabled())
            {
                this->physicallyAddRemoteAircraft(aircraft);
            }
            else
            {
                this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
            }
            return true;
        }

        void CSimulatorFsCommon::enableDebugMessages(bool driver, bool interpolator)
        {
            if (this->m_interpolator)
            {
                this->m_interpolator->enableDebugMessages(interpolator);
            }
            CSimulatorCommon::enableDebugMessages(driver, interpolator);
        }

        void CSimulatorFsCommon::unload()
        {
            this->m_aircraftCfgParser.cancelParsing();
            CSimulatorCommon::unload();
        }

        void CSimulatorFsCommon::ps_aircraftCfgParsingFinished(bool success)
        {
            if (!success) { return; }
            setPluginData(this, "aircraft_cfg", CVariant::from(m_aircraftCfgParser.getAircraftCfgEntriesList()));
            m_modelMatcher.setInstalledModels(m_aircraftCfgParser.getAircraftCfgEntriesList().toAircraftModelList());

            // Now the matcher has all required information to be initialized
            m_modelMatcher.init();
        }

    } // namespace
} // namespace
