/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorfscommon.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/fscommon/modelmappingsprovidervpilot.h"

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
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, renderedAircraftProvider, pluginStorageProvider, parent),
            m_fsuipc(new CFsuipc()),
            m_aircraftCfgParser(CAircraftCfgParser::createModelLoader(CSimulatorInfo(info.getIdentifier()))),
            m_modelMatcher(CAircraftMatcher::AllModes, this)
        {
            // hack to init mapper
            connect(&m_modelMatcher, &CAircraftMatcher::initializationFinished, this, &CSimulatorFsCommon::ps_mapperInitialized);
            auto modelMappingsProvider = std::unique_ptr<IModelMappingsProvider> { std::make_unique<CModelMappingsProviderVPilot>(true) };
            m_modelMatcher.setModelMappingProvider(std::move(modelMappingsProvider));

            bool c = connect(m_aircraftCfgParser.get(), &CAircraftCfgParser::loadingFinished, this, &CSimulatorFsCommon::ps_aircraftCfgParsingFinished);
            Q_ASSERT_X(c, Q_FUNC_INFO, "Cannot connect signal");
            Q_UNUSED(c);

            //! \todo remove from plugin if data there are cached as well
            /**
            CVariant aircraftCfg = getPluginData(this, "aircraft_cfg");
            if (aircraftCfg.isValid())
            {
                // will behave like parsing was finished
                m_aircraftCfgParser->updateCfgEntriesList(aircraftCfg.value<CAircraftCfgEntriesList>());
            }
            else
            {
                m_aircraftCfgParser->startLoading(CAircraftCfgParser::LoadInBackground);
            }
            **/
            m_aircraftCfgParser->startLoading(CAircraftCfgParser::InBackgroundWithCache);
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
                updateOwnModel(newModel);
                emit ownAircraftModelChanged(getOwnAircraft());
            }
        }

        CAircraftModel CSimulatorFsCommon::getClosestMatch(const CSimulatedAircraft &remoteAircraft)
        {
            return m_modelMatcher.getClosestMatch(remoteAircraft);
        }

        CAircraftModelList CSimulatorFsCommon::getInstalledModels() const
        {
            return m_modelMatcher.getInstalledModelsList();
        }

        void CSimulatorFsCommon::reloadInstalledModels()
        {
            m_aircraftCfgParser->startLoading();
        }

        CPixmap CSimulatorFsCommon::iconForModel(const QString &modelString) const
        {
            CStatusMessage msg;
            CPixmap pm(m_aircraftCfgParser->iconForModel(modelString, msg));
            if (!msg.isEmpty()) { CLogMessage::preformatted(msg);}
            return pm;
        }

        bool CSimulatorFsCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }

            // remove upfront, and then enable / disable again
            this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
            return this->changeRemoteAircraftEnabled(aircraft, originator);
        }

        bool CSimulatorFsCommon::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }
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
            this->m_aircraftCfgParser->cancelLoading();
            this->m_modelMatcher.cancelInit();
            CSimulatorCommon::unload();
        }

        void CSimulatorFsCommon::ps_aircraftCfgParsingFinished(bool success)
        {
            if (!success) { return; }
            setPluginData(this, "aircraft_cfg", CVariant::from(m_aircraftCfgParser->getAircraftCfgEntriesList()));
            m_modelMatcher.setInstalledModels(m_aircraftCfgParser->getAircraftCfgEntriesList().toAircraftModelList());

            // Now the matcher has all required information to be initialized
            m_modelMatcher.init();
        }

    } // namespace
} // namespace
