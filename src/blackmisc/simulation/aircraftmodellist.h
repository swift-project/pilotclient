/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELLIST_H

#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/orderablelist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <Qt>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CCallsign;
        class CLivery;
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
            public Mixin::MetaType<CAircraftModelList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftModelList)

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

            //! Find by model string
            //! \remark normally CAircraftModelList::findFirstByModelStringOrDefault would be used
            CAircraftModelList findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by model string
            CAircraftModel findFirstByModelStringOrDefault(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find first by callsign
            CAircraftModel findFirstByCallsignOrDefault(const Aviation::CCallsign &callsign) const;

            //! Find models starting with
            CAircraftModelList findModelsStartingWith(const QString &modelString, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive) const;

            //! Find by a given list of models by strings
            CAircraftModelList findByModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by a given list of models trings
            CAircraftModelList findByNotInModelStrings(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity) const;

            //! Find by model string
            CAircraftModelList findByIcaoDesignators(const Aviation::CAircraftIcaoCode &aircraftIcaoCode, const Aviation::CAirlineIcaoCode &airlineIcaoCode) const;

            //! Find by model string
            CAircraftModelList findByAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

            //! Find by livery code
            CAircraftModelList findByLiveryCode(const Aviation::CLivery &livery) const;

            //! With file name
            CAircraftModelList findWithFileName() const;

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

            //! Find by combined code, wildcards possible, e.g. L*P, *2J
            CAircraftModelList findByCombinedType(const QString &combinedType) const;

            //! Find by combined code and WTC, wildcards for combined code possible, e.g. L*P, *2J
            CAircraftModelList findByCombinedTypeAndWtc(const QString &combinedType, const QString &wtc) const;

            //! \copydoc findByCombinedType
            CAircraftModelList findByCombinedTypeWithColorLivery(const QString &combinedType) const;

            //! \copydoc findByCombinedTypeAndWtc
            CAircraftModelList findByCombinedTypeAndWtcWithColorLivery(const QString &combinedType, const QString &wtc) const;

            //! Find models with color liveries
            CAircraftModelList findColorLiveries() const;

            //! Find by military flag, \c false returns civilian models
            CAircraftModelList findByMilitaryFlag(bool military) const;

            //! Find by VTOL flag, \c false returns non VTOL models
            CAircraftModelList findByVtolFlag(bool vtol) const;

            //! Model icon path
            QString findModelIconPathByModelString(const QString &modelString) const;

            //! Model icon path
            QString findModelIconPathByCallsign(const Aviation::CCallsign &callsign) const;

            //! All models of the FS (FSX, P3D, FS9) family
            CAircraftModelList getAllFsFamilyModels() const;

            //! All included models
            CAircraftModelList getAllIncludedModels() const;

            //! Take a designator and find its family
            QString designatorToFamily(const Aviation::CAircraftIcaoCode &aircraftIcaoCode) const;

            //! Find for given simulator
            CAircraftModelList matchesSimulator(const Simulation::CSimulatorInfo &simulator) const;

            //! Set simulator for all elements
            int setSimulatorInfo(const Simulation::CSimulatorInfo &info);

            //! Which simulators are supported in that model list
            CSimulatorInfo simulatorsSupported() const;

            //! Set mode for all elements
            int setModelMode(Simulation::CAircraftModel::ModelMode mode);

            //! Keep only those models with given model strings
            //! \return number of elements removed
            int keepModelsWithString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity);

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

            //! Replace or add based on model string
            //! \return number of elements removed
            int replaceOrAddModelsWithString(const CAircraftModelList &addOrReplaceList, Qt::CaseSensitivity sensitivity);

            //! Model strings
            QStringList getModelStringList(bool sort = true) const;

            //! Model strings as set
            QSet<QString> getModelStringSet() const;

            //! Simulator counts
            CCountPerSimulator countPerSimulator() const;

            //! Which simulator(s) have the most entries?
            CSimulatorInfo simulatorsWithMaxEntries() const;

            //! Models with a color livery
            int countModelsWithColorLivery() const;

            //! Models with an airline livery
            int countModelsWithAirlineLivery() const;

            //! Count VTOL aircraft
            int countVtolAircraft() const;

            //! Count military aircraft
            int countMilitaryAircraft() const;

            //! Count civilian aircraft
            int countCivilianAircraft() const;

            //! Number of different airlines
            int countDifferentAirlines() const;

            //! Count different combined types
            int countCombinedTypes() const;

            //! Update distributor, all models in list are set to given distributor
            void updateDistributor(const CDistributor &distributor);

            //! All distributors used with models of this list
            CDistributorList getDistributors(bool onlyDbDistributors = true) const;

            //! Aircraft designators
            QSet<QString> getAircraftDesignators() const;

            //! Airline designators
            QSet<QString> getAirlineDesignators() const;

            //! Airline virtual designators
            QSet<QString> getAirlineVDesignators() const;

            //! All combined types
            QSet<QString> getCombinedTypes() const;

            //! All combined types as string
            QString getCombinedTypesAsString(const QString &separator = ", ") const;

            //! Update aircraft ICAO
            void updateAircraftIcao(const Aviation::CAircraftIcaoCode &icao);

            //! Update livery
            void updateLivery(const Aviation::CLivery &livery);

            //! From given CDistributorList update the model`s distributor order
            int updateDistributorOrder(const CDistributorList &distributors);

            //! File name normalized for DB
            void normalizeFileNamesForDb();

            //! Score by aircraft ICAO code
            ScoredModels scoreFull(const CAircraftModel &remoteModel, bool preferColorLiveries, bool ignoreZeroScores = true, CStatusMessageList *log = nullptr) const;

            //! Completer strings
            QStringList toCompleterStrings(bool sorted = true, const Simulation::CSimulatorInfo &simulator = { Simulation::CSimulatorInfo::All }) const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing() const;

            //! Validate for publishing
            CStatusMessageList validateForPublishing(CAircraftModelList &validModels, CAircraftModelList &invalidModels) const;

            //! Validate distributors
            CStatusMessageList validateDistributors(const Simulation::CDistributorList &distributors, CAircraftModelList &validModels, CAircraftModelList &invalidModels) const;

            //! To compact JSON format
            QJsonObject toMemoizedJson() const;

            //! From compact JSON format
            void convertFromMemoizedJson(const QJsonObject &json);

            //! To database JSON
            QJsonArray toDatabaseJson() const;

            //! To database JSON
            QString toDatabaseJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Compact) const;

            //! As HTML summary
            QString asHtmlSummary() const;

            //! What kind of models are represented here
            QString coverageSummary(const QString &separator = "\n") const;
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Simulation::CAircraftModelList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Simulation::CAircraftModel>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Simulation::CAircraftModel>)

#endif //guard
