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
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/statusmessage.h"

#include <QFlags>
#include <QObject>
#include <QString>

namespace BlackMisc
{
    class CLogCategoryList;
    class CStatusMessageList;
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    /*!
     * Matcher for all models.
     * \details Reads all the mapping rules and all the available flight simulator models.
     *          Then all rules for models not existing are eliminated ( \sa synchronize ).
     *          Thereafter all existing models and mappings can be obtained from here.
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
            ByCombinedCode   = 1 << 4,
            All              = ByModelString | ByIcaoData | ByFamily | ByLivery | ByCombinedCode
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

        //! Get the closest matching aircraft model.
        //! Result depends on enabled modes.
        //! \sa MatchingModeFlag
        //! \threadsafe
        BlackMisc::Simulation::CAircraftModel getClosestMatch(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, BlackMisc::CStatusMessageList *log = nullptr) const;

        //! Try to find the corresponding data in DB and get best information for following matching
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModel reverselLookupModel(
            const BlackMisc::Aviation::CCallsign &callsign,
            const QString &networkAircraftIcao, const QString &networkAirlineIcao, const QString &networkLiveryInfo, const QString &networkModelString,
            BlackMisc::Simulation::CAircraftModel::ModelType type,
            BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the corresponding data in DB and get best information for following matching
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModel reverselLookupModel(const BlackMisc::Simulation::CAircraftModel &modelToLookup, const QString &networkLiveryInfo, BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        static BlackMisc::Aviation::CAircraftIcaoCode reverseLookupAircraftIcao(const QString &icaoDesignator, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign(), BlackMisc::CStatusMessageList *log = nullptr);

        //! Try to find the DB corresponding ICAO code
        //! \threadsafe
        static BlackMisc::Aviation::CAirlineIcaoCode reverseLookupAirlineIcao(const QString &icaoDesignator, const BlackMisc::Aviation::CCallsign &callsign = BlackMisc::Aviation::CCallsign(), BlackMisc::CStatusMessageList *log = nullptr);

        //! Lookup of standard livery
        //! \threadsafe
        static BlackMisc::Aviation::CLivery reverseLookupStandardLivery(const BlackMisc::Aviation::CAirlineIcaoCode &airline, const BlackMisc::Aviation::CCallsign &callsign, BlackMisc::CStatusMessageList *log = nullptr);

        //! Turn callsign into airline
        static BlackMisc::Aviation::CAirlineIcaoCode callsignToAirline(const BlackMisc::Aviation::CCallsign &callsign, BlackMisc::CStatusMessageList *log = nullptr);

        //! Get the models
        BlackMisc::Simulation::CAircraftModelList getModelSet() const { return m_modelSet; }

        //! Set the models we want to use
        int setModelSet(const BlackMisc::Simulation::CAircraftModelList &models);

        //! Default model
        const BlackMisc::Simulation::CAircraftModel &getDefaultModel() const;

        //! Set default model
        void setDefaultModel(const BlackMisc::Simulation::CAircraftModel &defaultModel);

    private:
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
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByManufacturer(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, const QString &info, bool &reduceed, BlackMisc::CStatusMessageList *log);

        //! Reduce by airline ICAO
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByAirline(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, const QString &info, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! Installed models by combined code (ie L2J, L1P, ...)
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByCombinedCode(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, const BlackMisc::Simulation::CAircraftModelList &inList, bool relaxIfNotFound, bool &reduced, BlackMisc::CStatusMessageList *log);

        //! My military flag
        //! \threadsafe
        static BlackMisc::Simulation::CAircraftModelList ifPossibleReduceByMilitaryFlag(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft, bool military, const BlackMisc::Simulation::CAircraftModelList &inList, bool &reduced, BlackMisc::CStatusMessageList *log);

        MatchingMode                              m_matchingMode = All;
        BlackMisc::Simulation::CAircraftModel     m_defaultModel;         //!< model to be used as default model
        BlackMisc::Simulation::CAircraftModelList m_modelSet;             //!< models used for model matching
    };
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CAircraftMatcher::MatchingMode)

#endif // guard
