// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODEL_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODEL_H

#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/memotable.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/orderable.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/blackmiscexport.h"

#include <QFlags>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaType>
#include <QStringList>
#include <Qt>
#include <QFileInfo>
#include <QDir>
#include <tuple>

BLACK_DECLARE_VALUEOBJECT_MIXINS(BlackMisc::Simulation, CAircraftModel)

namespace BlackMisc
{
    class CIcon;

    namespace Simulation
    {
        //! DB ids
        struct DBTripleIds
        {
            int model = -1; //!< model id
            int livery = -1; //!< livery id, by that I have airline id
            int aircraft = -1; //!< aircraft ICAO id

            //! Any valid id?
            bool hasAnyId() const
            {
                return model >= 0 || livery >= 0 || aircraft >= 0;
            }

            //! Return as string
            QString toQString() const
            {
                return QStringLiteral("Model: %1 Livery: %2 Aircraft: %3").arg(model).arg(livery).arg(aircraft);
            }
        };

        //! Aircraft model (used by another pilot, my models on disk)
        //! \remarks Simulator independent class, supposed to be common denominator
        class BLACKMISC_EXPORT CAircraftModel :
            public CValueObject<CAircraftModel>,
            public Db::IDatastoreObjectWithIntegerKey, // also ITimestampBased
            public IOrderable
        {
        public:
            //! Model type
            enum ModelType
            {
                TypeUnknown,
                TypeQueriedFromNetwork, //!< model was queried by network protocol (ICAO data)
                TypeFSInnData, //!< model based on FSD ICAO data
                TypeReverseLookup, //!< reverse lookup model
                TypeModelMatching, //!< model is result of model matching
                TypeModelMatchingDefaultModel, //!< a default model assigned by model matching
                TypeDatabaseEntry, //!< used along with mapping definition
                TypeManuallySet, //!< manually set, e.g. from GUI
                TypeOwnSimulatorModel, //!< represents own simulator model (AI model, model on disk)
                TypeVPilotRuleBased, //!< based on a vPilot rule
                TypeTerrainProbe //!< peudo aircraft used for terrain probing (FSX)
            };

            //! Mode, decides if a model is supposed to be used in the model set for model matching
            //! or not. The combined type "All" is meant for search operations.
            enum ModelMode
            {
                Undefined = 0,
                Include = 1 << 0,
                Exclude = 1 << 1,
                All = Include | Exclude
            };

            //! Supposed to be used only in filter operations
            Q_DECLARE_FLAGS(ModelModeFilter, ModelMode)

            //! Indexes
            enum ColumnIndex
            {
                IndexModelString = CPropertyIndexRef::GlobalIndexCAircraftModel,
                IndexModelStringAlias,
                IndexAllModelStrings, //!< indluding aliases
                IndexCallsign,
                IndexName,
                IndexDescription,
                IndexSimulatorInfo,
                IndexSimulatorInfoAsString,
                IndexAircraftIcaoCode,
                IndexLivery,
                IndexCG,
                IndexDistributor,
                IndexFileName,
                IndexFileTimestamp,
                IndexFileTimestampFormattedYmdhms,
                IndexIconPath,
                IndexModelType,
                IndexModelTypeAsString,
                IndexModelMode,
                IndexModelModeAsString,
                IndexSupportedParts,
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
            CAircraftModel(const QString &model, ModelType type, const Aviation::CAircraftIcaoCode &icao, const Aviation::CLivery &livery);

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const QString &description, const Aviation::CAircraftIcaoCode &icao, const Aviation::CLivery &livery = Aviation::CLivery());

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description);

            //! Constructor.
            CAircraftModel(const QString &model, ModelType type, const CSimulatorInfo &simulator, const QString &name, const QString &description, const Aviation::CAircraftIcaoCode &icao, const Aviation::CLivery &livery = Aviation::CLivery());

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftModel &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Can be initialized from FSD
            bool canInitializeFromFsd() const;

            //! Corresponding callsign if applicable
            const Aviation::CCallsign &getCallsign() const { return m_callsign; }

            //! Corresponding callsign if applicable
            void setCallsign(const Aviation::CCallsign &callsign);

            //! Callsign empty
            bool isCallsignEmpty() const { return m_callsign.isEmpty(); }

            //! Model key, either queried or loaded from simulator model
            const QString &getModelString() const { return m_modelString; }

            //! Model string and DB key (if available)
            QString getModelStringAndDbKey() const;

            //! Model string
            void setModelString(const QString &modelString) { m_modelString = modelString.trimmed().toUpper(); }

            //! Model key, either queried or loaded from simulator model
            const QString &getModelStringAlias() const { return m_modelStringAlias; }

            //! Get model string and aliases
            QString getAllModelStringsAndAliases() const;

            //! Get model string and aliases
            QString getAllModelStringsAliasesAndDbKey() const;

            //! Model string alias
            void setModelStringAlias(const QString &alias) { m_modelStringAlias = alias.trimmed().toUpper(); }

            //! Descriptive text
            const QString &getDescription() const { return m_description; }

            //! Descriptive text
            void setDescription(const QString &description) { m_description = description.trimmed(); }

            //! Set queried model string
            void setQueriedModelString(const QString &model)
            {
                m_modelString = model;
                m_modelType = TypeQueriedFromNetwork;
            }

            //! Aircraft ICAO code
            const Aviation::CAircraftIcaoCode &getAircraftIcaoCode() const { return m_aircraftIcao; }

            //! \copydoc BlackMisc::Aviation::CAircraftIcaoCode::getEnginesCount
            int getEngineCount() const { return m_aircraftIcao.getEnginesCount(); }

            //! Aircraft ICAO code designator
            const QString &getAircraftIcaoCodeDesignator() const { return m_aircraftIcao.getDesignator(); }

            //! VTOL aircraft?
            bool isVtol() const;

            //! Airline ICAO code
            const Aviation::CAirlineIcaoCode &getAirlineIcaoCode() const { return m_livery.getAirlineIcaoCode(); }

            //! Set airline ICAO code designator
            bool setAirlineIcaoDesignator(const QString &designator) { return m_livery.setAirlineIcaoCodeDesignator(designator); }

            //! Airline ICAO code designator
            const QString &getAirlineIcaoCodeDesignator() const { return m_livery.getAirlineIcaoCode().getDesignator(); }

            //! Airline ICAO code designator
            const QString getAirlineIcaoCodeVDesignator() const { return m_livery.getAirlineIcaoCode().getVDesignator(); }

            //! Set aircraft ICAO code
            bool setAircraftIcaoCode(const Aviation::CAircraftIcaoCode &aircraftIcaoCode);

            //! Set aircraft ICAO code designator
            void setAircraftIcaoDesignator(const QString &designator);

            //! Set ICAO codes
            void setAircraftIcaoCodes(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CAirlineIcaoCode &airlineIcaoCode);

            //! Valid airline and aircraft designator?
            bool hasValidAircraftAndAirlineDesignator() const;

            //! Has aircraft designator?
            bool hasAircraftDesignator() const;

            //! Has known aircraft designator?
            bool hasKnownAircraftDesignator() const;

            //! Assigned a category?
            bool hasCategory() const;

            //! Airline designator?
            bool hasAirlineDesignator() const;

            //! Designators
            bool hasAircraftAndAirlineDesignator() const;

            //! Military model?
            bool isMilitary() const;

            //! Civilian model?
            bool isCivilian() const;

            //! Get livery
            const Aviation::CLivery &getLivery() const { return m_livery; }

            //! Livery
            void setLivery(const Aviation::CLivery &livery) { m_livery = livery; }

            //! Livery available?
            bool hasLivery() const { return m_livery.hasCompleteData(); }

            //! Get distributor
            const CDistributor &getDistributor() const { return m_distributor; }

            //! Get distributor`s order value
            int getDistributorOrder() const { return m_distributor.getOrder(); }

            //! Set distributor
            void setDistributor(const CDistributor &distributor) { m_distributor = distributor; }

            //! Set the distributor order
            bool setDistributorOrder(int order);

            //! Update distributor`s order attribute
            bool setDistributorOrder(const CDistributorList &distributors);

            //! Distributor loaded from DB
            bool hasDbDistributor() const;

            //! Distributor, but not necessarily loaded from DB
            bool hasDistributor() const;

            //! By distributor
            bool matchesDbDistributor(const CDistributor &distributor) const;

            //! By distributor
            bool matchesAnyDbDistributor(const CDistributorList &distributors) const;

            //! Name
            const QString &getName() const { return m_name; }

            //! Name
            void setName(const QString &name) { m_name = name.trimmed(); }

            //! Get center of gravity
            const PhysicalQuantities::CLength &getCG() const { return m_cg; }

            //! Get center of gravity
            void setCG(const PhysicalQuantities::CLength &cg) { m_cg = cg; }

            //! CG value available?
            bool hasCG() const { return !m_cg.isNull(); }

            //! Supported parts
            const QString &getSupportedParts() const { return m_supportedParts; }

            //! Supported parts
            void setSupportedParts(const QString &supportedParts);

            //! Model type
            ModelType getModelType() const { return m_modelType; }

            //! Terrain probe?
            bool isTerrainProbe() const { return m_modelType == TypeTerrainProbe; }

            //! Model type
            const QString &getModelTypeAsString() const { return modelTypeToString(getModelType()); }

            //! Set type
            void setModelType(ModelType type) { m_modelType = type; }

            //! Model mode
            ModelMode getModelMode() const { return m_modelMode; }

            //! Matches given mode?
            bool matchesMode(Simulation::CAircraftModel::ModelModeFilter mode) const;

            //! Model mode as string
            const QString &getModelModeAsString() const { return modelModeToString(getModelMode()); }

            //! Model mode as string
            const CIcon &getModelModeAsIcon() const;

            //! Set model mode
            void setModelMode(ModelMode mode) { m_modelMode = mode; }

            //! Set model mode as string
            void setModelModeAsString(const QString &mode);

            //! Simulator info
            CSimulatorInfo getSimulator() const { return m_simulator; }

            //! Set simulator info
            void setSimulator(const CSimulatorInfo &simulator) { m_simulator = simulator; }

            //! Matches given simulator?
            bool matchesSimulator(const CSimulatorInfo &simulator) const;

            //! Matches given simulator?
            bool matchesSimulatorFlag(CSimulatorInfo::Simulator simulator) const;

            //! swift livery string (to be sent via network)
            //! \sa parseNetworkLiveryString
            QString getSwiftLiveryString(bool aircraftIcao = true, bool livery = true, bool model = true) const;

            //! swift livery string (to be sent via network) for simulator
            QString getSwiftLiveryString(const CSimulatorInfo &sim) const;

            //! Update missing parts from another model
            void updateMissingParts(const CAircraftModel &otherModel, bool dbModelPriority = true);

            //! Update the directories from other model
            //! \sa updateLocalFileNames
            void updateByExistingDirectories(const CAircraftModel &otherModel);

            //! Queried model string?
            bool hasQueriedModelString() const;

            //! Model string which was manually set
            bool hasManuallySetString() const;

            //! Non empty model string?
            bool hasModelString() const { return !m_modelString.isEmpty(); }

            //! Non empty model string alias?
            bool hasModelStringAlias() const { return !m_modelStringAlias.isEmpty(); }

            //! Any model string or alias?
            bool hasAnyModelString() const { return this->hasModelString() || this->hasModelStringAlias(); }

            //! Description
            bool hasDescription(bool ignoreAutoGenerated = false) const;

            //! Valid simulator
            bool hasValidSimulator() const;

            //! Info, which members (Livery, Aircraft ICAO, ...) are already based on DB data
            QString getMembersDbStatus() const;

            //! Matches model string?
            bool matchesModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Matches model string or alias?
            bool matchesModelStringOrAlias(const QString &modelString, Qt::CaseSensitivity sensitivity) const;

            //! Calculate score
            int calculateScore(const CAircraftModel &compareModel, bool preferColorLiveries, CStatusMessageList *log = nullptr) const;

            //! Validate
            CStatusMessageList validate(bool withNestedObjects) const;

            //! Considered equal for publishing, compares if livery etc. are the same DB values
            bool isEqualForPublishing(const CAircraftModel &dbModel, CStatusMessageList *details = nullptr) const;

            //! Helper class used by implementation.
            using MemoHelper = CMemoHelper<Aviation::CAircraftIcaoCode, Aviation::CLivery, CDistributor>;

            //! To JSON with memoized members (used by CAircraftModelList)
            QJsonObject toMemoizedJson(MemoHelper::CMemoizer &) const;

            //! From JSON with memoized members (used by CAircraftModelList)
            void convertFromMemoizedJson(const QJsonObject &json, const MemoHelper::CUnmemoizer &);

            //! To database JSON
            QJsonObject toDatabaseJson() const;

            //! To database JSON
            QString toDatabaseJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

            //! As a brief HTML summary (e.g. used in tooltips)
            QString asHtmlSummary(const QString &separator = "<br>") const;

            // ---------------- simulator file related functions -------------------

            //! File name (corresponding data for simulator, only available if representing simulator model
            //! \remark normally parh and name, like with QFile name
            const QString &getFileName() const { return m_fileName; }

            //! File name as lower case
            QString getFileNameLowerCase() const { return m_fileName.toLower(); }

            //! File name?
            bool hasFileName() const { return !m_fileName.isEmpty(); }

            //! Matching file name?
            bool matchesFileName(const QString &fileName) const;

            //! Does the corresponding file exist?
            bool hasExistingCorrespondingFile() const;

            //! Directory
            QDir getFileDirectory() const;

            //! Directory path if any
            QString getFileDirectoryPath() const;

            //! Is the file in the give path
            bool isInPath(const QString &path, Qt::CaseSensitivity cs) const;

            //! File name
            void setFileName(const QString &fileName) { m_fileName = fileName; }

            //! Icon file representing model
            const QString &getIconFile() const { return m_iconFile; }

            //! Icon file representing model
            void setIconFile(const QString &iconFile) { m_iconFile = iconFile; }

            //! Is the icon file existing?
            bool hasExistingIconFile() const;

            //! Get timestamp
            QDateTime getFileTimestamp() const;

            //! File timestamp
            QString getFormattedFileTimestampYmdhms() const;

            //! Valid file timestamp?
            bool hasValidFileTimestamp() const;

            //! Set file timestamp
            void setFileTimestamp(const QDateTime &timestampUtc);

            //! Set file timestamp
            void setFileTimestamp(qint64 timestamp);

            //! Set file timestamp, timestamp and file name
            void setFileDetailsAndTimestamp(const QFileInfo &fileInfo);

            //! Load icon from disk
            CPixmap loadIcon(CStatusMessage &success) const;

            //! File path for DB (absolute paths make no sense in DB)
            void normalizeFileNameForDb();

            //! Update file names from local model
            //! \remark if we have local file names, we use those names
            void updateLocalFileNames(const CAircraftModel &model);

            //! Adjust file names to a new directory
            bool adjustLocalFileNames(const QString &newModelDir, const QString &stripModelDirIndicator = {});

            // ---------------- end file related functions --------------

            //! Verify the model data
            CStatusMessageList verifyModelData() const;

            //! Model type
            static const QString &modelTypeToString(ModelType type);

            //! File path used for DB
            static QString normalizeFileNameForDb(const QString &filePath);

            //! Model mode
            static ModelMode modelModeFromString(const QString &mode);

            //! Model mode
            static const QString &modelModeToString(ModelMode mode);

            //! From swift DB JSON
            static CAircraftModel fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString("mod_"));

            //! From swift DB JSON, caching during this process (faster)
            static CAircraftModel fromDatabaseJsonCaching(const QJsonObject &json, Aviation::AircraftIcaoIdMap &aircraftIcaos, const Aviation::AircraftCategoryIdMap &categories, Aviation::LiveryIdMap &liveries, DistributorIdMap &distributors, const QString &prefix = QString("mod_"));

            //! Livery string prefix
            static const QString &liveryStringPrefix();

            //! swift livery string
            static bool isSwiftLiveryString(const QString &liveryString);

            //! Split swift network string
            //! \sa getSwiftLiveryString
            static DBTripleIds parseNetworkLiveryString(const QString &liveryString);

            //! Hint, that model was automatically generated (e.g. by auto stashing)
            static const QString &autoGenerated();

            //! Clean up parts string
            static QString cleanUpPartsString(const QString &p);

            //! Supported parts
            //! \remark Engine, Flaps, Gear, Lights, Spoilers
            static const QString &supportedParts();

        private:
            //! Common implemenation of all fromDatabaseJson functions
            static CAircraftModel fromDatabaseJsonBaseImpl(const QJsonObject &json, const QString &prefix, const Aviation::CAircraftIcaoCode &aircraftIcao, const Aviation::CLivery &livery, const CDistributor &distributor);

            Aviation::CCallsign m_callsign; //!< aircraft's callsign if any
            Aviation::CAircraftIcaoCode m_aircraftIcao; //!< ICAO code if available
            Aviation::CLivery m_livery; //!< livery information
            CSimulatorInfo m_simulator; //!< model for given simulator
            CDistributor m_distributor; //!< who designed or distributed the model
            QString m_modelString; //!< Simulator model key, unique
            QString m_modelStringAlias; //!< Simulator model key alias, unique
            QString m_name; //!< Model name
            QString m_description; //!< descriptive text
            QString m_fileName; //!< file name
            QString m_iconFile; //!< a file representing the aircraft as icon
            QString m_supportedParts; //!< supported parts
            qint64 m_fileTimestamp = -1; //!< file timestamp of originating file (if applicable)
            ModelType m_modelType = TypeUnknown; //!< model string is coming representing ...?
            ModelMode m_modelMode = Include; //!< model mode (include / exclude)
            PhysicalQuantities::CLength m_cg = PhysicalQuantities::CLength::null(); //!< center of gravity

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
                BLACK_METAMEMBER(cg),
                BLACK_METAMEMBER(supportedParts),
                BLACK_METAMEMBER(modelString, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(modelStringAlias, 0, CaseInsensitiveComparison),
                BLACK_METAMEMBER(name),
                BLACK_METAMEMBER(description, 0, DisabledForComparison),
                BLACK_METAMEMBER(fileName, 0, DisabledForComparison),
                BLACK_METAMEMBER(iconFile, 0, DisabledForComparison),
                BLACK_METAMEMBER(fileTimestamp, 0, DisabledForComparison),
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
