/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRCRAFTMATCHER_H
#define BLACKCORE_AIRCRAFTMATCHER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/matchingstatistics.h"
#include "blackmisc/statusmessage.h"

#include <QFlags>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    class CLogCategoryList;
    class CStatusMessageList;
    namespace Aviation   { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    /*!
     * Matcher for all models.
     * \details Reads the model set (ie the models the user wants to use).
     *          Also Allows to reverse lookup a model (from network to DB data).
     */
    class BLACKCORE_EXPORT CAircraftMatcher : public QObject
    {
        Q_OBJECT

    public:
        //! Enabled matching mode flags
        enum MatchingModeFlag
        {
            ByModelString    = 1 << 0,
            ByIcaoData       = 1 << 1,
            ByFamily         = 1 << 2,
            ByLivery         = 1 << 3,
            ByCombinedType   = 1 << 4,
            All              = ByModelString | ByIcaoData | ByFamily | ByLivery | ByCombinedType
        };
        Q_DECLARE_FLAGS(MatchingMode, MatchingModeFlag)

        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor
        CAircraftMatcher(MatchingMode matchingMode = All, QObject *parent = nullptr);

        //! Destructor
        virtual ~CAircraftMatcher();

        //! Set the enabled matching modes
        void setMatchingModes(MatchingMode matchingModes);

        //! Get the closest matching aircraft model from set.
        //! Result depends on enabled modes.
        //! \sa MatchingModeFlag
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getClosestMatch(
            const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft,
            BlackMisc::CStatusMessageList *log = nullptr) const;

        //! Return the airline ICAO being valid
        //! \threadsafe
        static BlackMisc::Aviation::CAirlineIcaoCode failoverValidAirlineIcao(
            const BlackMisc::Aviation::CCallsign &callsign,
            const QString &primaryIcao, const QString &secondaryIcao,
            bool airlineFromCallsign, BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the corresponding data in DB and get best information for given data
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModel reverseLookupModel(
            const BlackMisc::Aviation::CCallsign &callsign,
            const BlackMisc::Aviation::CAircraftIcaoCode &networkAircraftIcao,
            const BlackMisc::Aviation::CAirlineIcaoCode &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString,
            BlackMisc::Simulation::CAircraftModel::ModelType type,
            BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the corresponding data in DB and get best information for following matching
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModel reverseLookupModel(
            const BlackMisc::Simulation::CAircraftModel &modelToLookup,
            const QString &networkLiveryInfo, BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        static BlackMisc::Aviation::CAircraftIcaoCode reverseLookupAircraftIcao(
            const BlackMisc::Aviation::CAircraftIcaoCode &icaoDesignator,
            const BlackMisc::Aviation::CCallsign &logCallsign = BlackMisc::Aviation::CCallsign(),
            BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        static BlackMisc::Aviation::CAirlineIcaoCode reverseLookupAirlineIcao(
            const BlackMisc::Aviation::CAirlineIcaoCode &icaoPattern,
            const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign(), BlackMisc::CStatusMessageList *log = nullptr);

        //! Lookup of standard livery
        //! \threadsafe
        static BlackMisc::Aviation::CLivery reverseLookupStandardLivery(
            const BlackMisc::Aviation::CAirlineIcaoCode &airline,
            const BlackMisc::Aviation::CCallsign &callsign, BlackMisc::CStatusMessageList *log = nullptr);

        //! Lookup of airline name
        //! \threadsafe
        static QString reverseLookupAirlineName(
            const QString &candidate, const BlackMisc::Aviation::CCallsign &callsign = {}, BlackMisc::CStatusMessageList *log = nullptr);

        //! Lookup of telephony designator
        //! \threadsafe
        static QString reverseLookupTelephonyDesignator(
            const QString &candidate, const BlackMisc::Aviation::CCallsign &callsign = {},
            BlackMisc::CStatusMessageList *log = nullptr);

        //! Turn callsign into airline
        //! \threadsafe
        static BlackMisc::Aviation::CAirlineIcaoCode callsignToAirline(const BlackMisc::Aviation::CCallsign &callsign, BlackMisc::CStatusMessageList *log = nullptr);

        //! Get the models
        const BlackMisc::Simulation::CAircraftModelList &getModelSet() const { return m_modelSet; }

        //! Models
        bool hasModels() const { return !m_modelSet.isEmpty(); }

        //! Set the models we want to use
        //! \note uses a set from "somewhere else" so it can also be used with arbitrary sets for testing
        int setModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulatorHint = {});

        //! Default model
        const BlackMisc::Simulation::CAircraftModel &getDefaultModel() const;

        //! Set default model, can be set by driver specific for simulator
        void setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel);

        //! The current statistics
        BlackMisc::Simulation::CMatchingStatistics getCurrentStatistics() const;

        //! Clear the statistics
        void clearMatchingStatistics();

        //! Evaluate if a statistics entry makes sense and add it
        void evaluateStatisticsEntry(const QString &sessionId, const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery);

    private:
        //! The search based implementation
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getClosestMatchSearchImplementation(MatchingMode mode, const BlackMisc::Simulation::CAircraftModelList &modelSet, const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackMisc::CStatusMessageList *log = nullptr) const;

        //! The score based implementation
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getClosestMatchScoreImplementation(MatchingMode mode, const BlackMisc::Simulation::CAircraftModelList &modelSet, const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackMisc::CStatusMessageList *log = nullptr) const;

        //! Get combined type default model, i.e. get a default model under consideration of the combined code such as "L2J"
        //! \see BlackMisc::Simulation::CSimulatedAircraft::getAircraftIcaoCombinedType
        //! \remark in any case a (default) model is returned
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getCombinedTypeDefaultModel(const BlackMisc::Simulation::CAircraftModelList &modelSet, const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackMisc::CStatusMessageList *log = nullptr) const;

        //! Search in models by key (aka model string)
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModel matchByExactModelString(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &models, BlackMisc::CStatusMessageList *log);

        //! Installed models by ICAO data
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByIcaoData(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &models, bool ignoreAirline, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Find model by aircraft family
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByFamily(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const QString &family, const BlackMisc::Simulation::CAircraftModelList &inList, const QString &modelSource, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Search for exact livery and aircraft ICAO code
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByLiveryAndIcaoCode(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Reduce by manufacturer
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByManufacturer(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, const QString &info, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Reduce by manufacturer
        //! \threadsafe
        static BlackMisc::Aviation::CAircraftIcaoCodeList ifPossibleReduceAircraftIcaoByManufacturer(const BlackMisc::Aviation::CAircraftIcaoCode &icaoCode, const BlackMisc::Aviation::CAircraftIcaoCodeList &inList, const QString &info, bool &reduced, const BlackMisc::Aviation::CCallsign &logCallsign, BlackMisc::CStatusMessageList *log);

        //! Reduce by airline ICAO
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByAirline(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, const QString &info, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Installed models by combined code (ie L2J, L1P, ...)
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByCombinedCode(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! By military flag
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByMilitaryFlag(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Scores to string for debugging
        static QString scoresToString(const BlackMisc::Simulation::ScoredModels &scores, int lastElements = 5);

        MatchingMode                               m_matchingMode = All;
        BlackMisc::Simulation::CAircraftModel      m_defaultModel;       //!< model to be used as default model
        BlackMisc::Simulation::CAircraftModelList  m_modelSet;           //!< models used for model matching
        BlackMisc::Simulation::CSimulatorInfo      m_simulator;          //!< simulator (optional)
        BlackMisc::Simulation::CMatchingStatistics m_statistics;         //!< matching statistics
        QString                                    m_modelSetInfo;
    };
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CAircraftMatcher::MatchingMode)

#endif // guard
