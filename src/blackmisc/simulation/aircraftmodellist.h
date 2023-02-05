/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H

#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/collection.h"
#include "blackmisc/orderablelist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <Qt>
#include <QHash>
#include <QMap>
#include <atomic>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Simulation, CAircraftModel, CAircraftModelList)

namespace BlackMisc
{
    namespace Aviation
    {
        class CCallsign;
        class CLivery;
        class CAirlineIcaoCode;
    }

    namespace Simulation
    {
        //! Individual (matching) score for each model
        using ScoredModels = QMap<int, CAircraftModel>;

        //! Value object encapsulating a list of aircraft models
        class BLACKMISC_EXPORT CAircraftModelList :
            public CSequence<CAircraftModel>,
            public Db::IDatastoreObjectList<CAircraftModel, CAircraftModelList, int>,
            public IOrderableList<CAircraftModel, CAircraftModelList>,
            public Aviation::ICallsignObjectList<CAircraftModel, CAircraftModelList>,
            public Mixin::MetaType<CAircraftModelList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftModelList)
            using CSequence::CSequence;

            //! Empty constructor.
            CAircraftModelList();

            //! Construct from a base class object.
            CAircraftModelList(const CSequence<CAircraftModel> &other);

            //! Contains model string?
            bool containsModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Contains model with model string or id?
            bool containsModelStringOrDbKey(const Simulation::CAircraftModel &model, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Contains model for callsign?
            bool containsCallsign(const Aviation::CCallsign &callsign) const;

            //! Contains given combined type?
            bool containsCombinedType(const QString &combinedType) const;

            //! Contains any model with aircraft ICAO designator?
            bool containsModelsWithAircraftIcaoDesignator(const QString &aircraftDesignator) const;

            //! Contains any model with aircraft and airline ICAO designator?
            bool containsModelsWithAircraftAndAirlineIcaoDesignator(const QString &aircraftDesignator, const QString &airlineDesignator) const;

            //! Contains airline livery for given airline
            bool containsAirlineLivery(const Aviation::CAirlineIcaoCode &airline) const;

            //! Contains any model matching any of of passed simulators?
            bool containsMatchingSimulator(const CSimulatorInfo &simulators) const;

            //! Contains any model not matching any of of passed simulators?
            bool containsNotMatchingSimulator(const CSimulatorInfo &simulators) const;

            //! Contains military models?
            bool containsMilitary() const;

            //! Contains civilian models?
            bool containsCivilian() const;

            //! Contains both, military and civilian models?
            bool containsCivilianAndMilitary() const;

            //! Contains VTOL models?
            bool containsVtol() const;

            //! Contains any categorized model?
            bool containsCategory() const;

            //! Contains any model with 1st level?
            bool containsCategory(int firstLevel) const;

            //! Find by model string
            //! \remark normally CAircraftModelList::findFirstByModelStringOrDefault would be used
            CAircraftModelList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find empty model strings
            CAircraftModelList findEmptyModelStrings() const;

            //! Find first by model string
            CAircraftModel findFirstByModelStringOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by model string
            CAircraftModel findFirstByModelStringAliasOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by callsign
            CAircraftModel findFirstByCallsignOrDefault(const Aviation::CCallsign &callsign) const;

            //! Find models starting with
            CAircraftModelList findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by a given list of models by strings
            CAircraftModelList findByModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by excluding given list of models strings
            CAircraftModelList findByNotInModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by ICAO designators
            CAircraftModelList findByIcaoDesignators(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CAirlineIcaoCode &airlineIcaoCode) const;

            //! Find by ICAO of aircraft and airline
            CAircraftModelList findByAircraftAndAirline(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CAirlineIcaoCode &airlineIcaoCode) const;

            //! Find by designator and livery code
            CAircraftModelList findByAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

            //! Find by aircraft and livery
            CAircraftModelList findByAircraftAndLivery(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CLivery &livery) const;

            //! Find by the corresponding airline group
            CAircraftModelList findByAirlineGroup(const BlackMisc::Aviation::CAirlineIcaoCode &airline) const;

            //! Find by airline name and telephony
            CAircraftModelList findByAirlineNameAndTelephonyDesignator(const QString &name, const QString &telephony, bool onlyIfExistInModel = true) const;

            //! Find by airline name and telephony, similar to CAirlineIcaoCodeList::findByNamesOrTelephonyDesignator
            CAircraftModelList findByAirlineNamesOrTelephonyDesignator(const QString &name) const;

            //! Find by livery code
            CAircraftModelList findByLiveryCode(const Aviation::CLivery &livery) const;

            //! Models with file name
            CAircraftModelList findWithFileName() const;

            //! All models from given distributors
            CAircraftModelList findByDistributor(const CDistributor &distributor) const;

            //! All models from given distributors
            CAircraftModelList findByDistributors(const CDistributorList &distributors) const;

            //! Models with aircraft ICAO code set
            CAircraftModelList findWithAircraftDesignator() const;

            //! Models with aircraft ICAO code from list
            CAircraftModelList findWithAircraftDesignator(const QSet<QString> &designators) const;

            //! Models with a known aircraft ICAO code set
            CAircraftModelList findWithKnownAircraftDesignator() const;

            //! Find by manufacturer
            CAircraftModelList findByManufacturer(const QString &manufacturer) const;

            //! Models with aircraft family
            CAircraftModelList findByFamily(const QString &family) const;

            //! Models with aircraft family and color livery
            CAircraftModelList findByFamilyWithColorLivery(const QString &family) const;

            //! Models with aircraft family or designator and color livery
            CAircraftModelList findByDesignatorOrFamilyWithColorLivery(const Aviation::CAircraftIcaoCode &icao) const;

            //! Models with aircraft family or designators and color livery
            CAircraftModelList findByDesignatorsOrFamilyWithColorLivery(const QStringList &designators) const;

            //! Find by combined code, wildcards possible, e.g. L*P, *2J
            CAircraftModelList findByCombinedType(const QString &combinedType) const;

            //! Find by combined code and WTC, wildcards for combined code possible, e.g. L*P, *2J
            CAircraftModelList findByCombinedTypeAndWtc(const QString &combinedType, const QString &wtc) const;

            //! Combined type and color livery
            CAircraftModelList findByCombinedTypeWithColorLivery(const QString &combinedType) const;

            //! Combined WTC/livery
            CAircraftModelList findByCombinedTypeAndWtcWithColorLivery(const QString &combinedType, const QString &wtc) const;

            //! Combined type and manufacturer
            //! \remark kind of pseudo family, as in most cases the same combined type of a manufacturer means similar aircraft
            CAircraftModelList findByCombinedAndManufacturer(const Aviation::CAircraftIcaoCode &icao) const;

            //! Combined typeand manufacturer
            //! \remark kind of pseudo family, as in most cases the same combined type of a manufacturer means similar aircraft
            CAircraftModelList findByCombinedAndManufacturer(const QString &combinedType, const QString &manufacturer) const;

            //! Find color liveries
            CAircraftModelList findColorLiveries() const;

            //! @{
            //! Find models with the closest color distance
            CAircraftModelList findClosestColorDistance(const CRgbColor &fuselage, const CRgbColor &tail) const;
            CAircraftModelList findClosestFuselageColorDistance(const CRgbColor &color) const;
            //! @}

            //! Find by military flag, \c false returns civilian models
            CAircraftModelList findByMilitaryFlag(bool military) const;

            //! Find by VTOL flag, \c false returns non VTOL models
            CAircraftModelList findByVtolFlag(bool vtol) const;

            //! Find by model mode
            CAircraftModelList findByModelMode(CAircraftModel::ModelMode mode) const;

            //! Find by first level of category
            CAircraftModelList findByCategoryFirstLevel(int firstLevel) const;

            //! Find by category
            CAircraftModelList findByCategory(const Aviation::CAircraftCategory &category) const;

            //! Find by categories
            CAircraftModelList findByCategories(const Aviation::CAircraftCategoryList &categories) const;

            //! Model icon path
            QString findModelIconPathByModelString(const QString &modelString) const;

            //! Model icon path
            QString findModelIconPathByCallsign(const Aviation::CCallsign &callsign) const;

            //! Find models where the filename is not set or the file no longer exists
            CAircraftModelList findModelsWithoutExistingFile() const;

            //! Find models where the filename is set and the file exists
            CAircraftModelList findModelsWithExistingFile() const;

            //! Find duplicate model strings and return those models with at least 1 duplicate model string
            CAircraftModelList findDuplicateModelStrings() const;

            //! All models of the FS (FSX, P3D, FS9) family
            CAircraftModelList findFsFamilyModels() const;

            //! All models NOT of the FS (FSX, P3D, FS9) family
            CAircraftModelList findNonFsFamilyModels() const;

            //! @{
            //! With/without DB key
            CAircraftModelList findWithValidDbKey() const;
            CAircraftModelList findWithoutValidDbKey() const;
            CAircraftModelList findWithoutValidDbKey(int maxElements) const;
            //! @}

            //! @{
            //! Some finders for NON DB models, mostly for matching script results
            CAircraftModelList findNonDbModelsForAirline(const QString &airline) const;
            CAircraftModelList findNonDbModelsForAircraft(const QString &airline) const;
            CAircraftModelList findNonDbModelsForModelString(const QString &modelString) const;
            //! @}

            //! @{
            //! All included/excluded models
            CAircraftModelList findAllIncludedModels() const;
            CAircraftModelList findAllIncludedModels(int maxElements) const;
            CAircraftModelList findAllExcludedModels() const;
            CAircraftModelList findAllExcludedModels(int maxElements) const;
            //! @}

            //! Take a designator and find its family
            QString designatorToFamily(const Aviation::CAircraftIcaoCode &aircraftIcaoCode) const;

            //! Find for given simulator
            CAircraftModelList matchesSimulator(const CSimulatorInfo &simulator) const;

            //! Set simulator for all elements
            int setSimulatorInfo(const CSimulatorInfo &info);

            //! Which simulators are supported in this model list
            CSimulatorInfo simulatorsSupported() const;

            //! Is this here a FS family (P3D/FSX/FS9) model list?
            bool isLikelyFsFamilyModelList() const;

            //! Is this here a FS family (P3D/FSX) model list?
            bool isLikelyFsxFamilyModelList() const;

            //! Is this here a XPlane model list?
            bool isLikelyXPlaneModelList() const;

            //! Set mode for all elements
            int setModelMode(CAircraftModel::ModelMode mode);

            //! Set type for all elements
            int setModelType(CAircraftModel::ModelType type);

            //! Set center of gravity
            int setCG(const PhysicalQuantities::CLength &cg);

            //! Keep only those models with given model strings
            //! \return number of elements removed
            int keepModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity);

            //! Remove those models with given model strings
            //! \return number of elements removed
            bool removeModelWithString(const QString &modelString, Qt::CaseSensitivity sensitivity);

            //! Remove those models with given model strings
            //! \return number of elements removed
            int removeModelsWithString(const CAircraftModelList &models, Qt::CaseSensitivity sensitivity);

            //! Remove those models with given model strings
            //! \return number of elements removed
            int removeModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity);

            //! Remove if not matching simulator
            //! \return number of elements removed
            int removeIfNotMatchingSimulator(const CSimulatorInfo &needToMatch);

            //! Remove if having no model string
            //! \return number of elements removed
            int removeAllWithoutModelString();

            //! Remove if excluded CAircraftModel::Exclude
            //! \return number of elements removed
            int removeIfExcluded();

            //! Remove the XPlane flyable planes
            int removeXPlaneFlyablePlanes();

            //! Remove by distributor
            //! \return number of elements removed
            int removeByDistributor(const CDistributor &distributor);

            //! Remove if aircraft and livery
            //! \return number of elements removed
            int removeByAircraftAndLivery(const Aviation::CAircraftIcaoCode &aircraftIcao, const Aviation::CLivery &livery);

            //! Remove if aircraft and airline
            //! \return number of elements removed
            int removeByAircraftAndAirline(const Aviation::CAircraftIcaoCode &aircraftIcao, const Aviation::CAirlineIcaoCode &airline);

            //! Remove if NOT FS family model, ie. FSX/P3D/FS9
            //! \return number of elements removed
            int removeIfNotFsFamily();

            //! Remove those models of a particular file, but not in the given set
            //! \remark mostly used for FSX/FS9/P3D consolidation
            CAircraftModelList removeIfFileButNotInSet(const QString &fileName, const QSet<QString> &modelStrings);

            //! Replace or add based on model string
            //! \return element removed?
            bool replaceOrAddModelWithString(const CAircraftModel &addOrReplaceModel, Qt::CaseSensitivity sensitivity);

            //! Replace or add based on model string
            //! \return number of elements removed
            int replaceOrAddModelsWithString(const CAircraftModelList &addOrReplaceList, Qt::CaseSensitivity sensitivity);

            //! Model strings
            QStringList getModelStringList(bool sort = true) const;

            //! Model strings as set
            QSet<QString> getModelStringSet() const;

            //! Simulator counts
            CCountPerSimulator countPerSimulator() const;

            //! Model strings plus count
            QMap<QString, int> countPerModelString() const;

            //! Distributors plus count
            QMap<BlackMisc::Simulation::CDistributor, int> countPerDistributor() const;

            //! Aircraft ICAO plus count
            QMap<BlackMisc::Aviation::CAircraftIcaoCode, int> countPerAircraftIcao() const;

            //! Airline ICAO plus count
            QMap<BlackMisc::Aviation::CAirlineIcaoCode, int> countPerAirlineIcao() const;

            //! The airline with the max count
            Aviation::CAirlineIcaoCode getAirlineWithMaxCount() const;

            //! Which simulator(s) have the most entries?
            CSimulatorInfo simulatorsWithMaxEntries() const;

            //! Models with a color livery
            int countModelsWithColorLivery() const;

            //! Models with an airline livery
            int countModelsWithAirlineLivery() const;

            //! Count VTOL aircraft
            int countVtolAircraft() const;

            //! Count by mode
            int countByMode(CAircraftModel::ModelMode mode) const;

            //! Count military aircraft
            int countMilitaryAircraft() const;

            //! Count civilian aircraft
            int countCivilianAircraft() const;

            //! Number of different airlines
            int countDifferentAirlines() const;

            //! Count different combined types
            int countCombinedTypes() const;

            //! Models with aliases
            int countAliases() const;

            //! Sort by file path
            void sortByFileName();

            //! Update distributor, all models in list are set to given distributor
            void updateDistributor(const CDistributor &distributor);

            //! All distributors used with models of this list
            CDistributorList getDistributors(bool onlyDbDistributors = true) const;

            //! Aircraft ICAO codes from DB, without duplicates
            Aviation::CAircraftIcaoCodeList getAircraftIcaoCodesFromDb() const;

            //! Aircraft designators
            QSet<QString> getAircraftDesignators() const;

            //! Aircraft designators for airline
            //! \remark gives all aircraft flown by an airline
            QSet<QString> getAircraftDesignatorsForAirline(const Aviation::CAirlineIcaoCode &airlineCode) const;

            //! Aircraft ICAO codes for airline
            //! \remark gives all aircraft flown by an airline
            Aviation::CAircraftIcaoCodeList getAicraftIcaoCodesForAirline(const Aviation::CAirlineIcaoCode &airlineCode) const;

            //! Airline ICAO codes from DB, without duplicates
            Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesFromDb() const;

            //! Airline designators
            QSet<QString> getAirlineDesignators() const;

            //! Airline virtual designators
            QSet<QString> getAirlineVDesignators() const;

            //! Airline ICAO codes for given group, without duplicates
            Aviation::CAirlineIcaoCodeList getAirlineIcaoCodesForGroup(int groupId) const;

            //! Airline designators
            QSet<QString> getAirlineDesignatorsForGroup(int groupId) const;

            //! Airline virtual designators
            QSet<QString> getAirlineVDesignatorsForGroup(int groupId) const;

            //! All combined types
            QSet<QString> getCombinedTypes() const;

            //! All file names
            QSet<QString> getAllFileNames() const;

            //! All UNC file names
            //! \remark Windows on a "shared drive" paths
            QSet<QString> getAllUNCFileNames() const;

            //! All combined types as string
            QString getCombinedTypesAsString(const QString &separator = ", ") const;

            //! A set of all Aircraft/Airline ICAO codes
            QSet<QString> getAicraftAndAirlineDesignators(bool withDbId) const;

            //! A set of all Aircraft/Airline ICAO codes
            QString getAicraftAndAirlineDesignatorsAsString(bool withDbId, const QString &separator = ", ") const;

            //! Update aircraft ICAO
            void updateAircraftIcao(const Aviation::CAircraftIcaoCode &icao);

            //! Update livery
            void updateLivery(const Aviation::CLivery &livery);

            //! From given CDistributorList update the model`s distributor order
            int updateDistributorOrder(const CDistributorList &distributors);

            //! All file names normalized for DB
            void normalizeFileNamesForDb();

            //! Score by aircraft ICAO code
            ScoredModels scoreFull(const CAircraftModel &remoteModel, bool preferColorLiveries, bool ignoreZeroScores = true, CStatusMessageList *log = nullptr) const;

            //! Completer strings
            QStringList toCompleterStrings(bool sorted = true, const CSimulatorInfo &simulator = { CSimulatorInfo::All }) const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing() const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing(CAircraftModelList &validModels, CAircraftModelList &invalidModels) const;

            //! Validate distributors
            CStatusMessageList validateDistributors(const CDistributorList &distributors, CAircraftModelList &validModels, CAircraftModelList &invalidModels) const;

            //! Validate files (file exists etc.)
            CStatusMessageList validateFiles(CAircraftModelList &validModels, CAircraftModelList &invalidModels, bool ignoreEmptyFileNames, int stopAtFailedFiles, std::atomic_bool &wasStopped, const QString &simRootDirectory, bool alreadySortedByFn = false) const;

            //! To compact JSON format
            QJsonObject toMemoizedJson() const;

            //! From compact JSON format
            void convertFromMemoizedJson(const QJsonObject &json, bool fallbackToConvertToJson = false);

            //! To database JSON
            QJsonArray toDatabaseJson() const;

            //! To database JSON
            QString toDatabaseJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

            //! As HTML summary
            QString asHtmlSummary() const;

            //! What kind of models are represented here?
            QString coverageSummary(const QString &separator = "\n") const;

            //! What kind of models are represented here?
            //! \remark checking for some criteria in the given model
            QString coverageSummaryForModel(const CAircraftModel &checkModel, const QString &separator = "\n") const;

            //! A HTML summary of the data in the list
            QString htmlStatistics(bool aircraftStats, bool airlineStats) const;

            //! @{
            //! Save/load invalid models
            CStatusMessage saveInvalidModels() const;
            CStatusMessage loadInvalidModels();
            static const QString &invalidModelFileAndPath();
            static bool hasInvalidModelFile();
            //! @}

            //! @{
            //! Add as valid or invalid model (mutual exclusive)
            static void addAsValidOrInvalidModel(const CAircraftModel &model, bool valid, CAircraftModelList &validModels, CAircraftModelList &invalidModels);
            static void addAsValidOrInvalidModels(const CAircraftModelList &models, bool valid, CAircraftModelList &validModels, CAircraftModelList &invalidModels);
            //! @}

            //! Newer version
            static CAircraftModelList fromDatabaseJsonCaching(const QJsonArray &array,
                    const Aviation::CAircraftIcaoCodeList &aircraftIcaos = {},
                    const Aviation::CAircraftCategoryList &aircraftCategories = {},
                    const Aviation::CLiveryList &liveries = {},
                    const CDistributorList &distributors = {});

        private:
            //! Validate UNC paths (Windows)
            CStatusMessageList validateUncFiles(const QSet<QString> &uncFiles) const;
        };

        //! Model per callsign
        using CAircraftModelPerCallsign = QHash<Aviation::CCallsign, CAircraftModel>;

    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelPerCallsign)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CAircraftModel>)

#endif //guard
