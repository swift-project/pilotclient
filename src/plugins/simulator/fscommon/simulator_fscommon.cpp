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
#include "blackmisc/simulation/fscommon/vpilotmodelmappings.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"

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
            const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *renderedAircraftProvider,
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, renderedAircraftProvider, parent),
            m_fsuipc(new FsCommon::CFsuipc())
        {
            // hack to init mapper
            CAircraftMapper *mapper = mapperInstance();
            connect(mapper, &CAircraftMapper::initCompleted, this, &CSimulatorFsCommon::ps_mapperInitialized);
            mapper->initCompletelyInBackground();
        }

        CSimulatorFsCommon::~CSimulatorFsCommon()
        { }

        void CSimulatorFsCommon::ps_mapperInitialized(bool success)
        {
            if (success) { emit this->installedAircraftModelsChanged(); }
        }

        bool CSimulatorFsCommon::disconnectFrom()
        {
            if (this->m_fsuipc) { this->m_fsuipc->disconnect(); }
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

        bool CSimulatorFsCommon::setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset)
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
            CAircraftModel model = ownAircraft().getModel();
            model.setModelString(modelName);
            this->setOwnAircraftModel(model);
        }

        void CSimulatorFsCommon::setOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
        {
            if (ownAircraft().getModel() != model)
            {
                CAircraftModel newModel(model);
                newModel.setModelType(CAircraftModel::TypeOwnSimulatorModel);
                CSimulatorFsCommon::reverseLookupIcaoData(newModel);
                ownAircraft().setModel(newModel);
                emit ownAircraftModelChanged(ownAircraft());
            }
        }

        void CSimulatorFsCommon::reverseLookupIcaoData(CAircraftModel &model)
        {
            if (mapperInstance() && mapperInstance()->isInitialized())
            {
                // reverse lookup of ICAO
                CAircraftIcao icao =  mapperInstance()->getIcaoForModelString(model.getModelString());
                icao.updateMissingParts(icao);
            }
        }

        CAircraftMapper *CSimulatorFsCommon::mapperInstance()
        {
            static CVPilotModelMappings *mappings = new CVPilotModelMappings(true);

            // tries to access simObjectsDir, if this is an mapped remote directory
            // init might hang for a while
            static CAircraftMapper *mapper = new CAircraftMapper(
                std::unique_ptr<CVPilotModelMappings>(mappings), // currently hard wired
                simObjectsDir()
            );
            return mapper;
        }

        CAircraftModel CSimulatorFsCommon::modelMatching(const CSimulatedAircraft &remoteAircraft)
        {
            //! \todo Model Matching before models are read

            // default model
            CAircraftModel aircraftModel(remoteAircraft); // set defaults

            // Manually set string?
            if (remoteAircraft.getModel().hasManuallySetString())
            {
                // manual set model, maybe update missing parts
                aircraftModel.updateMissingParts(remoteAircraft.getModel());
                CSimulatorFsCommon::reverseLookupIcaoData(aircraftModel);
                return aircraftModel;
            }

            // mapper ready?
            if (!mapperInstance()->isInitialized())
            {
                // will be removed later, just for experimental version
                aircraftModel = CAircraftMapper::getDefaultModel();
                aircraftModel.setCallsign(remoteAircraft.getCallsign());
                CLogMessage(static_cast<CSimulatorFsCommon *>(nullptr)).warning("Mapper not ready, set to default model");
                return aircraftModel;
            }

            // Model by queried string
            const CClient remoteClient = remoteAircraft.getClient();
            if (remoteClient.getAircraftModel().hasQueriedModelString())
            {
                QString directModelString = remoteClient.getAircraftModel().getModelString();
                if (!directModelString.isEmpty() && mapperInstance()->containsModelWithTitle(directModelString))
                {
                    aircraftModel = mapperInstance()->getModelWithTitle(directModelString);
                    aircraftModel.setModelType(CAircraftModel::TypeQueriedFromNetwork);
                }
            }

            // ICAO to model
            if (!aircraftModel.hasModelString())
            {
                CAircraftIcao icao = remoteAircraft.getIcaoInfo();
                BlackMisc::Network::CAircraftMappingList mappingList = mapperInstance()->getAircraftMappingList().findByIcaoAircraftAndAirlineDesignator(icao, true);
                if (!mappingList.isEmpty())
                {
                    CAircraftModel modelFromMappings = mappingList.front().getModel();
                    // now turn the model from the mapping rules into a model from the simulator which has more metadata
                    aircraftModel = mapperInstance()->getModelWithTitle(modelFromMappings.getModelString());
                    Q_ASSERT(aircraftModel.getModelString() == modelFromMappings.getModelString());
                    aircraftModel.updateMissingParts(modelFromMappings); // update ICAO
                    aircraftModel.setModelType(CAircraftModel::TypeModelMatching);
                }
            }

            // default or sanity check
            if (!aircraftModel.hasModelString())
            {
                aircraftModel = CAircraftMapper::getDefaultModel();
            }
            else
            {
                // check, do we have the model on disk
                if (!mapperInstance()->containsModelWithTitle(aircraftModel.getModelString()))
                {
                    const QString m = QString("Missing model: %1").arg(aircraftModel.getModelString());
                    Q_ASSERT_X(false, "modelMatching", m.toLocal8Bit().constData());
                }
            }
            aircraftModel.setCallsign(remoteAircraft.getCallsign());

            Q_ASSERT(!aircraftModel.getCallsign().isEmpty());
            Q_ASSERT(aircraftModel.hasModelString());
            Q_ASSERT(aircraftModel.getModelType() != CAircraftModel::TypeUnknown);
            return aircraftModel;
        }

        QString CSimulatorFsCommon::simObjectsDir()
        {
            //! \todo add FS9 dir
            QString dir = CFsCommonUtil::fsxSimObjectsDirFromRegistry();
            if (!dir.isEmpty()) { return dir; }
            return "P:/FlightSimulatorX (MSI)/SimObjects";
            // "p:/temp/SimObjects"
        }

        CAircraftModelList CSimulatorFsCommon::getInstalledModels() const
        {
            if (!mapperInstance()) { return CAircraftModelList(); }
            return mapperInstance()->getAircraftCfgEntriesList().toAircraftModelList();
        }

        CAircraftIcao CSimulatorFsCommon::getIcaoForModelString(const QString &modelString) const
        {
            if (!mapperInstance()) { return CAircraftIcao(); }
            return mapperInstance()->getIcaoForModelString(modelString);
        }

        void CSimulatorFsCommon::reloadInstalledModels()
        {
            if (mapperInstance())
            {
                mapperInstance()->markUninitialized();
                mapperInstance()->initCompletelyInBackground();
            }
        }

        CPixmap CSimulatorFsCommon::iconForModel(const QString &modelString) const
        {
            static const CPixmap empty;
            if (modelString.isEmpty() || !mapperInstance()->isInitialized()) { return empty; }
            CAircraftCfgEntriesList cfgEntries = mapperInstance()->getAircraftCfgEntriesList().findByTitle(modelString);
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

        bool CSimulatorFsCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft, const QString &originator)
        {
            if (originator == simulatorOriginator()) { return false; }

            // remove upfront, and then enable / disable again
            this->removeRemoteAircraft(aircraft.getCallsign());
            return this->changeRemoteAircraftEnabled(aircraft, originator);
        }

        bool CSimulatorFsCommon::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const QString &originator)
        {
            if (originator == simulatorOriginator()) { return false; }
            if (aircraft.isEnabled())
            {
                this->addRemoteAircraft(aircraft);
            }
            else
            {
                this->removeRemoteAircraft(aircraft.getCallsign());
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

    } // namespace
} // namespace
