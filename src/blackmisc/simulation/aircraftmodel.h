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

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/orderable.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/variant.h"

#include <QFlags>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <Qt>
#include <tuple>

namespace BlackMisc
{
    class CIcon;
    class CStatusMessage;

    namespace Simulation
    {
        //! Aircraft model (used by another pilot, my models on disk)
        //! \remarks Simulator independent class, supposed to be common denominator
        class BLACKMISC_EXPORT CAircraftModel :
            public CValueObject<CAircraftModel>,
            public BlackMisc::Db::IDatastoreObjectWithIntegerKey,
            public BlackMisc::IOrderable
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
                Undefined = 0,
                Include   = 1 << 0,
                Exclude   = 1 << 1,
                All       = Include | Exclude
            };

            //! Supposed to be used only in filter operations
            Q_DECLARE_FLAGS(ModelModeFilter, ModelMode)

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
                IndexIconPath,
                IndexModelType,
                IndexModelTypeAsString,
                IndexModelMode,
                IndexModelModeAsString,
                IndexModelModeAsIcon,
                IndexHasQueriedModelString,
                IndexMembersDbStatus
            };

            //! \copydoc BlackMisc::CValueObject::registerMetadata
            static void registerMetadata();

            //! Default constructor.
            CAircraftModel() {}

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type);

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const QString &description, const BlackMisc::Aviation::CAircraftIcaoCode &icao, const BlackMisc::Aviation::CLivery &livery = BlackMisc::Aviation::CLivery());

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description, const BlackMisc::Aviation::CAircraftIcaoCode &icao, const BlackMisc::Aviation::CLivery &livery = BlackMisc::Aviation::CLivery());

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(const CPropertyIndex &index, const CAircraftModel &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Can be initialized from FSD
            bool canInitializeFromFsd() const;

            //! Corresponding callsign if applicable
            const BlackMisc::Aviation::CCallsign &getCallsign() const { return this->m_callsign; }

            //! Corresponding callsign if applicable
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) { this->m_callsign = callsign; }

            //! Callsign empty
            bool isCallsignEmpty() const { return this->m_callsign.isEmpty(); }

            //! Model key, either queried or loaded from simulator model
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

            //! Valid airline and aircraft designator?
            bool hasValidAircraftAndAirlineDesignator() const;

            //! Has aircraft designator?
            bool hasAircraftDesignator() const;

            //! Has known aircraft designator?
            bool hasKnownAircraftDesignator() const;

            //! Airline designator?
            bool hasAirlineDesignator() const;

            //! Military modles
            bool isMilitary() const;

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

            //! Update distributor`s order attribute
            bool updateDistributorOrder(const CDistributorList &distributors);

            //! Distributor
            bool hasDistributor() const;

            //! By distributor
            bool matchesDistributor(const CDistributor &distributor) const;

            //! By distributor
            bool matchesAnyDistributor(const CDistributorList &distributors) const;

            //! Name
            const QString &getName() const { return this->m_name; }

            //! Name
            void setName(const QString &name) { this->m_name = name.trimmed(); }

            //! Model type
            ModelType getModelType() const { return m_modelType; }

            //! Model type
            QString getModelTypeAsString() const { return modelTypeToString(getModelType()); }

            //! Set type
            void setModelType(ModelType type) { this->m_modelType = type; }

            //! Model mode
            ModelMode getModelMode() const { return m_modelMode; }

            //! Matches given mode?
            bool matchesMode(BlackMisc::Simulation::CAircraftModel::ModelModeFilter mode) const;

            //! Model mode as string
            const QString &getModelModeAsString() const { return modelModeToString(getModelMode()); }

            //! Model mode as string
            const BlackMisc::CIcon &getModelModeAsIcon() const;

            //! Set model mode
            void setModelMode(ModelMode mode) { m_modelMode = mode; }

            //! Set model mode as string
            void setModelModeAsString(const QString &mode);

            //! Simulator info
            CSimulatorInfo getSimulator() const { return this->m_simulator; }

            //! Set simulator info
            void setSimulator(const CSimulatorInfo &simulator) { this->m_simulator = simulator; }

            //! Matches given simulator?
            bool matchesSimulator(const CSimulatorInfo &simulator) const;

            //! File name (corresponding data for simulator, only available if representing simulator model
            QString getFileName() const { return m_fileName; }

            //! File name?
            bool hasFileName() const { return !m_fileName.isEmpty(); }

            //! File name
            void setFileName(const QString &fileName) { m_fileName = fileName; }

            //! File representing model
            const QString &getIconPath() const { return m_iconPath; }

            //! File representing model
            void setIconPath(const QString &iconFile) { m_iconPath = iconFile; }

            //! Load icon from disk
            CPixmap loadIcon(CStatusMessage &success) const;

            //! swift livery string (to be sent via network), "liveryCode [modelString]";
            //! \sa splitNetworkLiveryString
            QString getSwiftLiveryString() const;

            //! Update missing parts from another model
            void updateMissingParts(const CAircraftModel &otherModel, bool dbModelPriority = true);

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

            //! Info, which members (Livery, Aircraft ICAO, ...) are already based on DB data
            QString getMembersDbStatus() const;

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Validate
            BlackMisc::CStatusMessageList validate(bool withNestedObjects) const;

            //! To database JSON
            QJsonObject toDatabaseJson() const;

            //! To database JSON
            QString toDatabaseJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

            //! Model type
            static QString modelTypeToString(ModelType type);

            //! Model mode
            static ModelMode modelModeFromString(const QString &mode);

            //! Model mode
            static const QString &modelModeToString(ModelMode mode);

            //! From swift DB JSON
            static CAircraftModel fromDatabaseJson(const QJsonObject &json, const QString prefix = QString("mod_"));

            //! Split swift network string "DLH._STD" [modelname]"
            //! \return QStringList [0] livery code , [1] model string
            //! \sa getSwiftLiveryString
            static QStringList splitNetworkLiveryString(const QString &liveryString);

        private:
            BlackMisc::Aviation::CCallsign m_callsign;             //!< aircraft's callsign if any
            BlackMisc::Aviation::CAircraftIcaoCode m_aircraftIcao; //!< ICAO code if available
            BlackMisc::Aviation::CLivery m_livery;                 //!< livery information
            CSimulatorInfo m_simulator;                            //!< model for given simulator
            CDistributor   m_distributor;                          //!< who designed or distributed the model
            QString        m_modelString;                          //!< Simulator model key, unique
            QString        m_name;                                 //!< Model name
            QString        m_description;                          //!< descriptive text
            QString        m_fileName;                             //!< file name
            QString        m_iconPath;                             //!< a file representing the aircraft as icon
            ModelType      m_modelType = TypeUnknown;              //!< model string is coming representing ...?
            ModelMode      m_modelMode = Include;                  //!< model mode (include / exclude)

            BLACK_METACLASS(
                CAircraftModel,
                BLACK_METAMEMBER(dbKey),
                BLACK_METAMEMBER(timestampMSecsSinceEpoch),
                BLACK_METAMEMBER(order),
                BLACK_METAMEMBER(callsign),
                BLACK_METAMEMBER(aircraftIcao),
                BLACK_METAMEMBER(livery),
                BLACK_METAMEMBER(simulator),
                BLACK_METAMEMBER(distributor),
                BLACK_METAMEMBER(modelString, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(name),
                BLACK_METAMEMBER(description, 0, DisabledForComparison),
                BLACK_METAMEMBER(fileName, 0, DisabledForComparison),
                BLACK_METAMEMBER(iconPath, 0, DisabledForComparison),
                BLACK_METAMEMBER(modelType),
                BLACK_METAMEMBER(modelMode)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel::ModelType)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel::ModelMode)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModel::ModelModeFilter)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CAircraftModel::ModelModeFilter)

#endif // guard
