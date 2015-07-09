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
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/aviation/aircrafticaodata.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/network/user.h"
#include "blackmisc/propertyindex.h"

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
                TypeModelMatching,             //!< model is result of model matching
                TypeModelMatchingDefaultModel, //!< a default model assigned by model matching
                TypeModelMapping,              //!< used along with mapping definition
                TypeManuallySet,               //!< manually set, e.g. from GUI
                TypeOwnSimulatorModel          //!< represents own simulator model
            };

            //! Indexes
            enum ColumnIndex
            {
                IndexModelString = BlackMisc::CPropertyIndex::GlobalIndexCAircraftModel,
                IndexCallsign,
                IndexDescription,
                IndexIcao,
                IndexLivery,
                IndexFileName,
                IndexModelType,
                IndexModelTypeAsString,
                IndexHasQueriedModelString
            };

            //! Default constructor.
            CAircraftModel() {}

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type);

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const QString &description, const BlackMisc::Aviation::CAircraftIcaoData &icao, const BlackMisc::Aviation::CLivery &livery = BlackMisc::Aviation::CLivery());

            //! Constructor
            CAircraftModel(const BlackMisc::Aviation::CAircraft &aircraft);

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
            void setModelString(const QString &modelString) { this->m_modelString = modelString; }

            //! Descriptive text
            const QString &getDescription() const { return this->m_description; }

            //! Descriptive text
            void setDescription(const QString &description) { this->m_description = description; }

            //! Set queried model string
            void setQueriedModelString(const QString &model) { this->m_modelString = model; this->m_modelType = TypeQueriedFromNetwork; }

            //! ICAO code
            BlackMisc::Aviation::CAircraftIcaoData getIcao() const { return this->m_icao; }

            //! Set ICAO info
            void setIcao(const BlackMisc::Aviation::CAircraftIcaoData &icao) { this->m_icao = icao; }

            //! \copydoc CAircraftIcaoData::hasAircraftAndAirlineDesignator
            bool hasAircraftAndAirlineDesignator() const { return this->m_icao.hasAircraftAndAirlineDesignator(); }

            //! \copydoc CAircraftIcaoData::hasAircraftDesignator
            bool hasAircraftDesignator() const { return this->m_icao.hasAircraftDesignator(); }

            //! Aircraft ICAO code
            const BlackMisc::Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const;

            //! Airline ICAO code
            const BlackMisc::Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const;

            //! Get livery
            const BlackMisc::Aviation::CLivery &getLivery() const { return m_livery; }

            //! Livery
            void setLivery(const BlackMisc::Aviation::CLivery &livery) { this->m_livery = livery; }

            //! Livery available?
            bool hasLivery() const { return m_livery.hasCompleteData();}

            //! Model type
            ModelType getModelType() const { return m_modelType; }

            //! Model type
            QString getModelTypeAsString() const { return modelTypeToString(getModelType()); }

            //! Set type
            void setModelType(ModelType type) { this->m_modelType = type; }

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

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Model type
            static QString modelTypeToString(ModelType type);

            //! From swift DB JSON
            static CAircraftModel fromDatabaseJson(const QJsonObject &json);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAircraftModel)
            BlackMisc::Aviation::CCallsign m_callsign;        //!< aircraft's callsign if any
            BlackMisc::Aviation::CAircraftIcaoData m_icao;    //!< ICAO data if available
            BlackMisc::Aviation::CLivery m_livery;            //!< livery information
            CSimulatorInfo m_simulator;                       //!< model for given simulator
            QString m_modelString;                            //!< Simulator model string
            QString m_description;                            //!< descriptive text
            QString m_fileName;                               //!< file name
            ModelType m_modelType = TypeUnknown;              //!< model string is coming from ...?
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(
    BlackMisc::Simulation::CAircraftModel, (
        attr(o.m_callsign),
        attr(o.m_icao),
        attr(o.m_simulator),
        attr(o.m_modelString, flags<CaseInsensitiveComparison>()),
        attr(o.m_description, flags<DisabledForComparison>()),
        attr(o.m_fileName, flags <DisabledForComparison> ()),
        attr(o.m_modelType)
    ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel)

#endif // guard
