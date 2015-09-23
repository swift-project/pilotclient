/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODEL_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODEL_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/network/user.h"
#include "blackmisc/propertyindex.h"
#include <QUrlQuery>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Aircraft model (used by another pilot, my models on disk)
        //! \remarks Simulator independent class, supposed to be common denominator
        class BLACKMISC_EXPORT CAircraftModel :
            public CValueObject<CAircraftModel>,
            public BlackMisc::IDatastoreObjectWithIntegerKey
        {
        public:
            //! Model type
            enum ModelType
            {
                TypeUnknown,
                TypeQueriedFromNetwork,        //!< model was queried by network protocol
                TypeFsdData,                   //!< model based on FSD ICAO data
                TypeModelMatching,             //!< model is result of model matching
                TypeModelMatchingDefaultModel, //!< a default model assigned by model matching
                TypeDatabaseEntry,             //!< used along with mapping definition
                TypeManuallySet,               //!< manually set, e.g. from GUI
                TypeOwnSimulatorModel,         //!< represents own simulator model
                TypeVPilotRuleBased            //!< based on a vPilot rule
            };

            //! Mode
            enum ModelMode
            {
                Include,
                Exclude
            };

            //! Indexes
            enum ColumnIndex
            {
                IndexModelString = BlackMisc::CPropertyIndex::GlobalIndexCAircraftModel,
                IndexCallsign,
                IndexName,
                IndexDescription,
                IndexSimulatorInfo,
                IndexSimulatorInfoAsString,
                IndexAircraftIcaoCode,
                IndexLivery,
                IndexDistributor,
                IndexFileName,
                IndexModelType,
                IndexModelTypeAsString,
                IndexModelMode,
                IndexModelModeAsString,
                IndexHasQueriedModelString
            };

            //! Default constructor.
            CAircraftModel() {}

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type);

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const QString &description, const BlackMisc::Aviation::CAircraftIcaoCode &icao, const BlackMisc::Aviation::CLivery &livery = BlackMisc::Aviation::CLivery());

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, CSimulatorInfo &simulator, const QString &name, const QString &description, const BlackMisc::Aviation::CAircraftIcaoCode &icao, const BlackMisc::Aviation::CLivery &livery = BlackMisc::Aviation::CLivery());

            //! \copydoc CValueObject::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc CValueObject::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Corresponding callsign if applicable
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_callsign; }

            //! Corresponding callsign if applicable
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { this->m_callsign = callsign; }

            //! Callsign empty
            bool isCallsignEmpty() const { return this->m_callsign.isEmpty(); }

            //! Model string, either queried or loaded from simulator model
            const QString &getModelString() const { return this->m_modelString; }

            //! Model string
            void setModelString(const QString &modelString) { this->m_modelString = modelString.trimmed().toUpper(); }

            //! Descriptive text
            const QString &getDescription() const { return this->m_description; }

            //! Descriptive text
            void setDescription(const QString &description) { this->m_description = description; }

            //! Set queried model string
            void setQueriedModelString(const QString &model) { this->m_modelString = model; this->m_modelType = TypeQueriedFromNetwork; }

            //! Aircraft ICAO code
            const BlackMisc::Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const { return this->m_aircraftIcao; }

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const { return this->m_aircraftIcao.getDesignator(); }

            //! Airline ICAO code
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const { return this->m_livery.getAirlineIcaoCode(); }

            //! Airline ICAO code designator
            const QString &getAirlineIcaoCodeDesignator() const { return this->m_livery.getAirlineIcaoCode().getDesignator(); }

            //! Set aircraft ICAO code
            bool setAircraftIcaoCode(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode);

            //! Set aircraft ICAO code designator
            void setAircraftIcaoDesignator(const QString &designator);

            //! Set ICAO codes
            void setAircraftIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Airline and aircraft designator?
            bool hasAircraftAndAirlineDesignator() const;

            //! Has aircraft designator?
            bool hasAircraftDesignator() const;

            //! Airline designator?
            bool hasAirlineDesignator() const;

            //! Get livery
            const BlackMisc::Aviation::CLivery &getLivery() const { return m_livery; }

            //! Livery
            void setLivery(const BlackMisc::Aviation::CLivery &livery) { this->m_livery = livery; }

            //! Livery available?
            bool hasLivery() const { return m_livery.hasCompleteData();}

            //! Get distributor
            const CDistributor &getDistributor() const { return m_distributor; }

            //! Set distributor
            void setDistributor(const CDistributor &distributor) { m_distributor = distributor; }

            //! Name
            const QString &getName() const { return this->m_modelName; }

            //! Name
            void setName(const QString &name) { this->m_modelName = name.trimmed(); }

            //! Model type
            ModelType getModelType() const { return m_modelType; }

            //! Model type
            QString getModelTypeAsString() const { return modelTypeToString(getModelType()); }

            //! Set type
            void setModelType(ModelType type) { this->m_modelType = type; }

            //! Model mode
            ModelMode getModelMode() const { return m_modelMode; }

            //! Model mode as string
            QString getModelModeAsString() const { return modelModeToString(getModelMode()); }

            //! Set model mode
            void setModelMode(ModelMode mode) { m_modelMode = mode; }

            //! Simulator info
            CSimulatorInfo getSimulatorInfo() const { return this->m_simulator; }

            //! Set simulator info
            void setSimulatorInfo(const CSimulatorInfo &simulator) { this->m_simulator = simulator; }

            //! File name (corresponding data for simulator, only available if representing simulator model=
            QString getFileName() const { return m_fileName; }

            //! File name?
            bool hasFileName() const { return !m_fileName.isEmpty(); }

            //! File name
            void setFileName(const QString &fileName) { m_fileName = fileName; }

            //! Update missing parts from another model
            void updateMissingParts(const CAircraftModel &model);

            //! Queried model string?
            bool hasQueriedModelString() const;

            //! Model string which was manually set
            bool hasManuallySetString() const;

            //! Non empty model string
            bool hasModelString() const { return !m_modelString.isEmpty(); }

            //! Description
            bool hasDescription() const { return !m_description.isEmpty(); }

            //! Valid simulator
            bool hasValidSimulator() const;

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Validate
            BlackMisc::CStatusMessageList validate(bool withNestedObjects) const;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Model type
            static QString modelTypeToString(ModelType type);

            //! Model mode
            static ModelMode modelModeFromString(const QString &mode);

            //! Model mode
            static QString modelModeToString(ModelMode mode);

            //! From swift DB JSON
            static CAircraftModel fromDatabaseJson(const QJsonObject &json, const QString prefix = QString("mod_"));

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftModel)
            BlackMisc::Aviation::CCallsign m_callsign;             //!< aircraft's callsign if any
            BlackMisc::Aviation::CAircraftIcaoCode m_aircraftIcao; //!< ICAO code if available
            BlackMisc::Aviation::CLivery m_livery;                 //!< livery information
            CSimulatorInfo m_simulator;                            //!< model for given simulator
            CDistributor m_distributor;                            //!< who designed or distributed the model
            QString m_modelString;                                 //!< Simulator model string
            QString m_modelName;                                   //!< Model name
            QString m_description;                                 //!< descriptive text
            QString m_fileName;                                    //!< file name
            ModelType m_modelType = TypeUnknown;                   //!< model string is coming representing ...?
            ModelMode m_modelMode = Include;                       //!< model mode (include / exclude)
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(
    BlackMisc::Simulation::CAircraftModel, (
        attr(o.m_dbKey),
        attr(o.m_timestampMSecsSinceEpoch),
        attr(o.m_callsign),
        attr(o.m_aircraftIcao),
        attr(o.m_livery),
        attr(o.m_simulator),
        attr(o.m_distributor),
        attr(o.m_modelString, flags<CaseInsensitiveComparison>()),
        attr(o.m_modelName),
        attr(o.m_description, flags<DisabledForComparison>()),
        attr(o.m_fileName, flags <DisabledForComparison> ()),
        attr(o.m_modelType),
        attr(o.m_modelMode)
    ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel)

#endif // guard
