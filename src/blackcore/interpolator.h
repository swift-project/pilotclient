/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_INTERPOLATOR_H
#define BLACKCORE_INTERPOLATOR_H

#include "blackcoreexport.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/worker.h"
#include <QHash>
#include <QList>

namespace BlackCore
{
    //! Interpolator, calculation inbetween positions
    class BLACKCORE_EXPORT IInterpolator :
        public BlackMisc::CContinuousWorker,
        public BlackMisc::Simulation::CRemoteAircraftAwareReadOnly
    {
        Q_OBJECT

    public:
        //! Situations per callsign
        typedef QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftSituationList> CSituationsPerCallsign;

        //! Parts per callsign
        typedef QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftPartsList> CPartsPerCallsign;

        //! Virtual destructor
        virtual ~IInterpolator() {}

        //! Log category
        static QString getMessageCategory() { return "swift.interpolator"; }

        //! Status of interpolation
        struct BLACKCORE_EXPORT InterpolationStatus
        {
        public:
            bool changedPosition = false;        //!< position was changed
            bool interpolationSucceeded = false; //!< interpolation succeeded (means enough values, etc.)

            //! all OK
            bool allTrue() const;

            //! Reset to default values
            void reset();
        };

        //! Status regarding parts
        struct PartsStatus
        {
            bool supportsParts = false;   //!< supports parts for given callsign

            //! all OK
            bool allTrue() const;

            //! Reset to default values
            void reset();
        };

        //! Current interpolated situation
        //! \threadsafe
        virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
            const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc,
            InterpolationStatus &status, const CSituationsPerCallsign *situationsPerCallsign = nullptr) const = 0;

        //! Do a complete calculation for all know callsigns.
        //! \param currentTimeMsSinceEpoch if no value is passed current time is used
        //! \threadsafe
        //!
        virtual BlackMisc::Aviation::CAircraftSituationList getInterpolatedSituations(qint64 currentTimeMsSinceEpoch = -1);

        //! All situations for all callsigns (in map as per callsign)
        //! \threadsafe
        CSituationsPerCallsign getSituationsByCallsign() const;

        //! Parts before given offset time (aka pending parts)
        //! \threadsafe
        virtual BlackMisc::Aviation::CAircraftPartsList getAndRemovePartsBeforeTime(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTime, PartsStatus &partsStatus);

        //! Clear all data
        //! \threadsafe
        virtual void clear();

        //! Does know callsign?
        //! \threadsafe
        virtual bool hasDataForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! All situations for given callsign
        //! \threadsafe
        BlackMisc::Aviation::CAircraftSituationList getSituationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! All parts for given callsign
        //! \threadsafe
        BlackMisc::Aviation::CAircraftPartsList getPartsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Enable debug messages
        void enableDebugMessages(bool enabled);

        //! Force sorting (latest first), not required if order can be guaranteed
        void forceSortingOfAddedValues(bool sort);

        static const qint64 TimeOffsetMs = 6000;           //!< offset for interpolation
        static const int MaxSituationsPerCallsign = 6;     //!< How many situations per callsign
        static const int MaxPartsPerCallsign = 3;          //!< How many parts per callsign
        static const int MaxKeptInterpolationRequests = 3; //!< How many requests are stored

    private slots:
        //! New situation got added
        //! \threadsafe
        void ps_onAddedAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Added aircraft parts
        //! \threadsafe
        void ps_onAddedAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! Removed aircraft
        //! \threadsafe
        void ps_onRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

    protected:
        //! Constructor
        IInterpolator(BlackMisc::Simulation::IRemoteAircraftProviderReadOnly *provider, const QString &workerName, QObject *parent = nullptr);

        bool m_withDebugMsg = false;              //!< allows to disable debug messages
        bool m_forceSortWhenAddingValues = false; //!< force sorting (latest first) when adding values
        BlackMisc::Aviation::CCallsignSet m_aircraftSupportingParts; //!< aircraft supporting parts

        // hashs, because not sorted by key but keeping order
        CSituationsPerCallsign m_situationsByCallsign; //!< situations
        CPartsPerCallsign      m_partsByCallsign;      //!< parts

        // locks
        mutable QReadWriteLock m_lockSituations; //!< lock for situations
        mutable QReadWriteLock m_lockParts;      //!< lock for parts

    };

} // namespace

#endif // guard
