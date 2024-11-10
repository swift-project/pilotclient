// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AIRCRAFTMATCHER_H
#define SWIFT_CORE_AIRCRAFTMATCHER_H

#include "core/webdataservicesms.h"
#include "core/swiftcoreexport.h"
#include "misc/simulation/aircraftmodelsetprovider.h"
#include "misc/simulation/aircraftmatchersetup.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/matchingscriptmisc.h"
#include "misc/simulation/matchingstatistics.h"
#include "misc/simulation/matchinglog.h"
#include "misc/simulation/categorymatcher.h"
#include "misc/statusmessage.h"
#include "misc/valueobject.h"
#include "misc/variant.h"

#include <QFlags>
#include <QObject>
#include <QString>
#include <QPair>
#include <QSet>

namespace swift::misc
{
    class CLogCategoryList;
    class CStatusMessageList;
    namespace aviation
    {
        class CCallsign;
    }
    namespace simulation
    {
        class CSimulatedAircraft;
    }
}

namespace swift::core
{
    //! Matcher for all models.
    //! \details Reads the model set (ie the models the user wants to use).
    //!          Also Allows to reverse lookup a model (from network to DB data).
    class SWIFT_CORE_EXPORT CAircraftMatcher :
        public QObject,
        public swift::misc::simulation::IAircraftModelSetProvider
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::simulation::IAircraftModelSetProvider)

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CAircraftMatcher(const swift::misc::simulation::CAircraftMatcherSetup &setup, QObject *parent = nullptr);

        //! Constructor
        CAircraftMatcher(QObject *parent = nullptr);

        //! Destructor
        //! \remark saves a log of removed models if any
        virtual ~CAircraftMatcher() override;

        //! Copy constructor
        CAircraftMatcher(const CAircraftMatcher &) = delete;

        //! Copy assignment operator
        CAircraftMatcher &operator=(const CAircraftMatcher &) = delete;

        //! Set the setup
        bool setSetup(const swift::misc::simulation::CAircraftMatcherSetup &setup);

        //! Get the setup
        swift::misc::simulation::CAircraftMatcherSetup getSetup() const { return m_setup; }

        //! Get the closest matching aircraft model from set.
        //! Result depends on setup.
        //! \sa swift::misc::simulation::CAircraftMatcherSetup
        //! \threadsafe
        swift::misc::simulation::CAircraftModel getClosestMatch(
            const swift::misc::simulation::CSimulatedAircraft &remoteAircraft,
            swift::misc::simulation::MatchingLog whatToLog,
            swift::misc::CStatusMessageList *log,
            bool useMatchingScript) const;

        //! Return an valid airline ICAO code
        //! \threadsafe
        static swift::misc::aviation::CAirlineIcaoCode failoverValidAirlineIcaoDesignator(
            const swift::misc::aviation::CCallsign &callsign,
            const QString &primaryIcao, const QString &secondaryIcao,
            bool airlineFromCallsign, const QString &airlineName, const QString &airlineTelephony, bool useWebServices, swift::misc::CStatusMessageList *log = nullptr);

        //! Return an valid airline ICAO code from a given model list
        //! \remarks model list could be the model set
        //! \threadsafe
        static swift::misc::aviation::CAirlineIcaoCode failoverValidAirlineIcaoDesignator(
            const swift::misc::aviation::CCallsign &callsign,
            const QString &primaryIcao, const QString &secondaryIcao,
            bool airlineFromCallsign,
            const QString &airlineName, const QString &airlineTelephony,
            const swift::misc::simulation::CAircraftModelList &models,
            swift::misc::CStatusMessageList *log = nullptr);

        //! Return an valid airline ICAO code from a given model list and use webservices if NOT found
        //! \remarks model list could be the model set
        //! \threadsafe
        static swift::misc::aviation::CAirlineIcaoCode failoverValidAirlineIcaoDesignatorModelsFirst(const swift::misc::aviation::CCallsign &callsign,
                                                                                                     const QString &primaryIcao, const QString &secondaryIcao,
                                                                                                     bool airlineFromCallsign,
                                                                                                     const QString &airlineName, const QString &airlineTelephony,
                                                                                                     const swift::misc::simulation::CAircraftModelList &models,
                                                                                                     swift::misc::CStatusMessageList *log = nullptr);

        //! Run the network reverse lookup script
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::simulation::MatchingScriptReturnValues reverseLookupScript(const swift::misc::simulation::CAircraftModel &inModel, const swift::misc::simulation::CAircraftMatcherSetup &setup, const swift::misc::simulation::CAircraftModelList &modelSet, swift::misc::CStatusMessageList *log);

        //! Run the matching stage lookup script
        //! \threadsafe
        static swift::misc::simulation::MatchingScriptReturnValues matchingStageScript(const swift::misc::simulation::CAircraftModel &inModel, const swift::misc::simulation::CAircraftModel &matchedModel, const swift::misc::simulation::CAircraftMatcherSetup &setup, const swift::misc::simulation::CAircraftModelList &modelSet, swift::misc::CStatusMessageList *log);

        //! Run the matching script
        //! \threadsafe
        static swift::misc::simulation::MatchingScriptReturnValues matchingScript(const QString &js,
                                                                                  const swift::misc::simulation::CAircraftModel &inModel, const swift::misc::simulation::CAircraftModel &matchedModel, const swift::misc::simulation::CAircraftMatcherSetup &setup,
                                                                                  const swift::misc::simulation::CAircraftModelList &modelSet, swift::misc::simulation::MatchingScript ms,
                                                                                  swift::misc::CStatusMessageList *log);

        //! Try to find the corresponding data in DB and get best information for given data
        //! \threadsafe
        //! \ingroup reverselookup
        //! \remark NOT running matching script
        static swift::misc::simulation::CAircraftModel reverseLookupModel(const swift::misc::aviation::CCallsign &callsign,
                                                                          const swift::misc::aviation::CAircraftIcaoCode &networkAircraftIcao,
                                                                          const swift::misc::aviation::CAirlineIcaoCode &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString,
                                                                          const swift::misc::simulation::CAircraftMatcherSetup &setup,
                                                                          const swift::misc::simulation::CAircraftModelList &modelSet,
                                                                          swift::misc::simulation::CAircraftModel::ModelType type,
                                                                          swift::misc::CStatusMessageList *log);

        //! Try to find the corresponding data in DB and get best information for following matching
        //! \threadsafe
        //! \ingroup reverselookup
        //! \remark NOT running matching script
        static swift::misc::simulation::CAircraftModel reverseLookupModel(
            const swift::misc::simulation::CAircraftModel &modelToLookup,
            const QString &networkLiveryInfo,
            const swift::misc::simulation::CAircraftMatcherSetup &setup,
            const swift::misc::simulation::CAircraftModelList &modelSet,
            swift::misc::CStatusMessageList *log);

        //! Try to find the corresponding data in DB and get best information for following matching
        //! \threadsafe
        //! \ingroup reverselookup
        //! \remark Running matching script
        static swift::misc::simulation::CAircraftModel reverseLookupModelMs(
            const swift::misc::simulation::CAircraftModel &modelToLookup,
            const QString &networkLiveryInfo,
            const swift::misc::simulation::CAircraftMatcherSetup &setup,
            const swift::misc::simulation::CAircraftModelList &modelSet,
            swift::misc::CStatusMessageList *log);

        //! Try to find model by model string
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::simulation::CAircraftModel reverseLookupModelStringInDB(
            const QString &modelString,
            const swift::misc::aviation::CCallsign &callsign,
            bool doLookupString, swift::misc::CStatusMessageList *log);

        //! Try to find model by model string in set
        //! \threadsafe
        //! \remark mostly needed to work witn NON-DB values in matching script
        //! \ingroup reverselookup
        static swift::misc::simulation::CAircraftModel reverseLookupModelStringInSet(
            const QString &modelString,
            const swift::misc::aviation::CCallsign &callsign, const swift::misc::simulation::CAircraftModelList &modelSet,
            bool useNonDbEntries, swift::misc::CStatusMessageList *log);

        //! Try to find model by id
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::simulation::CAircraftModel reverseLookupModelId(int id, const swift::misc::aviation::CCallsign &callsign, swift::misc::CStatusMessageList *log);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::aviation::CAircraftIcaoCode reverseLookupAircraftIcao(
            const swift::misc::aviation::CAircraftIcaoCode &icaoDesignator,
            const swift::misc::aviation::CCallsign &logCallsign = {},
            swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup of ICAO by id
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::aviation::CAircraftIcaoCode reverseLookupAircraftIcaoId(int id, const swift::misc::aviation::CCallsign &logCallsign, swift::misc::CStatusMessageList *log = nullptr);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::aviation::CAirlineIcaoCode reverseLookupAirlineIcao(
            const swift::misc::aviation::CAirlineIcaoCode &icaoPattern,
            const swift::misc::aviation::CCallsign &callsign = {}, swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup of standard livery
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::aviation::CLivery reverseLookupStandardLivery(
            const swift::misc::aviation::CAirlineIcaoCode &airline,
            const swift::misc::aviation::CCallsign &callsign, swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup of livery by id
        //! \threadsafe
        //! \ingroup reverselookup
        static swift::misc::aviation::CLivery reverseLookupLiveryId(int id, const swift::misc::aviation::CCallsign &logCallsign, swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup by ids
        //! \threadsafe
        //! \ingroup reverselookup
        static int reverseLookupByIds(const swift::misc::simulation::DBTripleIds &ids, swift::misc::aviation::CAircraftIcaoCode &aircraftIcao, swift::misc::aviation::CLivery &livery, const swift::misc::aviation::CCallsign &logCallsign, swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup of airline name
        //! \threadsafe
        //! \ingroup reverselookup
        static QString reverseLookupAirlineName(
            const QString &candidate, const swift::misc::aviation::CCallsign &callsign = {}, swift::misc::CStatusMessageList *log = nullptr);

        //! Lookup of telephony designator
        //! \threadsafe
        //! \ingroup reverselookup
        static QString reverseLookupTelephonyDesignator(
            const QString &candidate, const swift::misc::aviation::CCallsign &callsign = {},
            swift::misc::CStatusMessageList *log = nullptr);

        //! Is this aircraft designator known?
        //! \threadsafe
        static bool isKnownAircraftDesignator(
            const QString &candidate, const swift::misc::aviation::CCallsign &callsign = {},
            swift::misc::CStatusMessageList *log = nullptr);

        //! Is this aircraft designator known?
        //! \threadsafe
        static bool isKnownModelString(
            const QString &candidate, const swift::misc::aviation::CCallsign &callsign = {},
            swift::misc::CStatusMessageList *log = nullptr);

        //! Search among the airline aircraft
        //! \remark only works if an airline is know
        //! \threadsafe
        static swift::misc::aviation::CAircraftIcaoCode searchAmongAirlineAircraft(
            const QString &icaoString,
            const swift::misc::aviation::CAirlineIcaoCode &airline,
            const swift::misc::aviation::CCallsign &callsign = {},
            swift::misc::CStatusMessageList *log = nullptr);

        //! Turn callsign into airline
        //! \threadsafe
        static swift::misc::aviation::CAirlineIcaoCode callsignToAirline(const swift::misc::aviation::CCallsign &callsign, swift::misc::CStatusMessageList *log = nullptr);

        //! \copydoc swift::misc::simulation::IAircraftModelSetProvider::getModelSet
        virtual swift::misc::simulation::CAircraftModelList getModelSet() const override { return m_modelSet; }

        //! Model set as reference
        virtual const swift::misc::simulation::CAircraftModelList &getModelSetRef() const { return m_modelSet; }

        //! Model set count
        virtual int getModelSetCount() const override { return m_modelSet.sizeInt(); }

        //! Models
        bool hasModels() const { return !m_modelSet.isEmpty(); }

        //! Set the models we want to use
        //! \note uses a set from "somewhere else" so it can also be used with arbitrary sets for testing
        int setModelSet(const swift::misc::simulation::CAircraftModelList &models, const swift::misc::simulation::CSimulatorInfo &simulator, bool forced);

        //! Remove a model for matching
        //! \remark effective until new set is set
        void disableModelsForMatching(const swift::misc::simulation::CAircraftModelList &removedModels, bool incremental);

        //! The disabled models for matching
        swift::misc::simulation::CAircraftModelList getDisabledModelsForMatching() const { return m_disabledModels; }

        //! Restore the models removed with CAircraftMatcher::disableModelForMatching
        void restoreDisabledModels();

        //! Default model
        const swift::misc::simulation::CAircraftModel &getDefaultModel() const { return m_defaultModel; }

        //! Set default model, can be set by driver specific for simulator
        void setDefaultModel(const swift::misc::simulation::CAircraftModel &defaultModel);

        //! The current statistics
        const swift::misc::simulation::CMatchingStatistics &getCurrentStatistics() const { return m_statistics; }

        //! Clear the statistics
        void clearMatchingStatistics() { m_statistics.clear(); }

        //! Evaluate if a statistics entry makes sense and add it
        void evaluateStatisticsEntry(const QString &sessionId, const swift::misc::aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

        //! Adding a model failed
        void addingRemoteModelFailed(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

    signals:
        //! Setup changed
        void setupChanged();

    private:
        //! Save the disabled models if any
        bool saveDisabledForMatchingModels();

        //! The search based implementation
        static swift::misc::simulation::CAircraftModelList getClosestMatchStepwiseReduceImplementation(
            const swift::misc::simulation::CAircraftModelList &modelSet, const swift::misc::simulation::CAircraftMatcherSetup &setup,
            const swift::misc::simulation::CCategoryMatcher &categoryMatcher, const swift::misc::simulation::CSimulatedAircraft &remoteAircraft,
            swift::misc::simulation::MatchingLog whatToLog, swift::misc::CStatusMessageList *log = nullptr);

        //! The score based implementation
        static swift::misc::simulation::CAircraftModelList getClosestMatchScoreImplementation(const swift::misc::simulation::CAircraftModelList &modelSet, const swift::misc::simulation::CAircraftMatcherSetup &setup, const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, int &maxScore, swift::misc::simulation::MatchingLog whatToLog, swift::misc::CStatusMessageList *log = nullptr);

        //! Get combined type default model, i.e. get a default model under consideration of the combined code such as "L2J"
        //! \see swift::misc::simulation::CSimulatedAircraft::getAircraftIcaoCombinedType
        //! \remark in any case a (default) model is returned
        static swift::misc::simulation::CAircraftModel getCombinedTypeDefaultModel(const swift::misc::simulation::CAircraftModelList &modelSet, const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModel &defaultModel, swift::misc::simulation::MatchingLog whatToLog, swift::misc::CStatusMessageList *log = nullptr);

        //! Search in models by key (aka model string)
        //! \threadsafe
        static swift::misc::simulation::CAircraftModel matchByExactModelString(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &models, swift::misc::simulation::MatchingLog whatToLog, swift::misc::CStatusMessageList *log);

        //! Installed models by ICAO data
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByIcaoData(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &models, const swift::misc::simulation::CAircraftMatcherSetup &setup, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Find model by aircraft family
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByFamily(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, bool allowPseudoFamily, const swift::misc::simulation::CAircraftModelList &inList, bool &reduced, QString &usedFamily, swift::misc::CStatusMessageList *log);

        //! Find model by aircraft family
        //! \remark pseudo family searches for same combined type and manufacturer
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByFamily(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const QString &family, bool allowPseudoFamily, const swift::misc::simulation::CAircraftModelList &inList, const QString &hint, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Search for exact livery and aircraft ICAO code
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByLiveryAndAircraftIcaoCode(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Reduce by manufacturer
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByManufacturer(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, const QString &info, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Reduce by manufacturer
        //! \threadsafe
        static swift::misc::aviation::CAircraftIcaoCodeList ifPossibleReduceAircraftIcaoByManufacturer(const swift::misc::aviation::CAircraftIcaoCode &icaoCode, const swift::misc::aviation::CAircraftIcaoCodeList &inList, const QString &info, bool &reduced, const swift::misc::aviation::CCallsign &logCallsign, swift::misc::CStatusMessageList *log);

        //! Reduce by aircraft ICAO
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByAircraft(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, const QString &info, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Reduce by aircraft ICAO or family
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByAircraftOrFamily(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, bool allowPseudoFamily, const swift::misc::simulation::CAircraftModelList &inList, const swift::misc::simulation::CAircraftMatcherSetup &setup, const QString &info, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Reduce by airline ICAO
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByAirline(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, const swift::misc::simulation::CAircraftMatcherSetup &setup, const QString &info, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Reduce by airline name/telephone designator
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceModelsByAirlineNameTelephonyDesignator(const swift::misc::aviation::CCallsign &cs, const QString &airlineName, const QString &telephony, const swift::misc::simulation::CAircraftModelList &inList, const QString &info, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Installed models by combined code (ie L2J, L1P, ...)
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByCombinedType(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, const swift::misc::simulation::CAircraftMatcherSetup &setup, bool &reduced, swift::misc::CStatusMessageList *log);

        //! By military flag
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByMilitaryFlag(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, bool &reduced, swift::misc::CStatusMessageList *log);

        //! By VTOL flag
        //! \threadsafe
        static swift::misc::simulation::CAircraftModelList ifPossibleReduceByVTOLFlag(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft, const swift::misc::simulation::CAircraftModelList &inList, bool &reduced, swift::misc::CStatusMessageList *log);

        //! Scores to string for debugging
        //! \threadsafe
        static QString scoresToString(const swift::misc::simulation::ScoredModels &scores, int lastElements = 5);

        //! Designator to object
        //! \threadsafe
        static swift::misc::aviation::CAirlineIcaoCode stringToAirlineIcaoObject(const swift::misc::aviation::CCallsign &cs, const QString &designator, const QString &airlineName, const QString &airlineTelephony, bool useSwiftDbData, swift::misc::CStatusMessageList *log);

        //! Valid designator, allows to check against swift DB
        //! \threadsafe
        static bool isValidAirlineIcaoDesignator(const QString &designator, bool checkAgainstSwiftDb);

        //! Use pseudo family
        static bool constexpr UsePseudoFamily = true;

        swift::misc::simulation::CAircraftMatcherSetup m_setup; //!< setup
        swift::misc::simulation::CAircraftModel m_defaultModel; //!< model to be used as default model
        swift::misc::simulation::CAircraftModelList m_modelSet; //!< models used for model matching
        swift::misc::simulation::CAircraftModelList m_disabledModels; //!< disabled models for matching
        swift::misc::simulation::CSimulatorInfo m_simulator; //!< simulator (optional)
        swift::misc::simulation::CMatchingStatistics m_statistics; //!< matching statistics
        swift::misc::simulation::CCategoryMatcher m_categoryMatcher; //!< the category matcher
        QString m_modelSetInfo; //!< info string
    };
} // namespace

#endif // guard
